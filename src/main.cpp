// main.cpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
// For `getpid` function
#include <unistd.h>

#include <vector>
#include <map>
#include <iostream>

#include "./constants.hpp"
#include "./HMM.hpp"
#include "./Agent.hpp"
#include "./Game.hpp"
#include "./analysis.hpp"

using namespace std;

double perSiteMutationRate = 0.005;
int generation = 0;
int repeats = 1;
int numAgents = 100;
int numGenerations = 10000;
char trialName[1000];
double sensorNoise = 0.0;

void saveLOD(Agent *agent, FILE *statsFile, FILE *genomeFile);


int main(int argc, char *argv[]) {
    vector<Agent*> agent;
    vector<Agent*> nextGen;
    Agent *masterAgent;
    int i, j, who = 0;
    Game *game;
    double maxFitness;
    FILE *LOD;
    FILE *genomeFile;
    LOD = fopen(argv[2], "w+t");
    genomeFile = fopen(argv[3], "w+t");
    int localSeed = atoi(argv[5]);
    if (localSeed != -1) {
        srand(localSeed);
    } else {
        srand(getpid());
    }
    agent.resize(numAgents);
    game = new Game(argv[1]);

    // Larissa
    game->nowUpdate = floor(numGenerations / 2);

    sensorNoise = atof(argv[6]);
    masterAgent = new Agent;
    masterAgent->setupEmptyAgent(5000);
    masterAgent->setupPhenotype();
    for (i = 0; i < agent.size(); i++) {
        agent[i] = new Agent;
        agent[i]->inherit(masterAgent, 0.01, 0);
    }
    nextGen.resize(agent.size());
    masterAgent->nrPointingAtMe--;
    cout << "Setup complete." << endl;
    while (generation < numGenerations) {
        for (i = 0; i < agent.size(); i++) {
            agent[i]->fitness = 0.0;
            agent[i]->fitnesses.clear();
        }
        for (i = 0; i < agent.size(); i++) {
            for (j = 0; j < repeats; j++) {
                game->executeGame(agent[i], sensorNoise, j);
                agent[i]->fitnesses.push_back((float)agent[i]->correct);
            }
        }
        if (generation == game->nowUpdate) {
            for (i = 0; i < agent.size(); i++) {
                makeSingleAgentAnalysis(agent[i], argv[4], i);
            }
        }

        maxFitness = 0.0;

        for (i = 0; i < agent.size(); i++) {
            agent[i]->fitness = 1.0;
            for (j = 0; j < repeats; j++)
                agent[i]->fitness *= agent[i]->fitnesses[j];
            if (repeats <= 1) {
              // Larissa: This for one repeat
              agent[i]->fitness = pow(1.02, agent[i]->fitness);
            } else {
              agent[i]->fitness = pow(agent[i]->fitness, (1.0 / repeats));
              agent[i]->fitness = pow(1.02, agent[i]->fitness);
            }
            if (agent[i]->fitness > maxFitness) {
                who = i;
                maxFitness = agent[i]->fitness;
            }
        }

        cout << "Generation " << generation << ": [fitness] " <<
            (double)maxFitness << " [correct/incorrect] " << agent[who]->correct <<
            "/" << agent[who]->incorrect << " [percent correct] " <<
            (float)agent[who]->correct / (83.0 * 82.0) << endl;

        for (i = 0; i < agent.size(); i++) {
            Agent *d;
            d = new Agent;
            do {
                j = rand() % (int)agent.size();
            } while (randDouble > (agent[j]->fitness / maxFitness));
            d->inherit(agent[j], perSiteMutationRate, generation);
            nextGen[i] = d;
        }
        for (i = 0; i < agent.size(); i++) {
            agent[i]->nrPointingAtMe--;
            if (agent[i]->nrPointingAtMe == 0)
                delete agent[i];
            agent[i] = nextGen[i];
        }
        agent = nextGen;
        generation++;
    }
    // Larissa: put noise to 0 for analysis
    makeFullAnalysis(game, agent[0], argv[4], 0);
    saveLOD(agent[0], LOD, genomeFile);
    return 0;
}

void saveLOD(Agent *agent, FILE *statsFile, FILE *genomeFile) {
    vector<Agent*> list;
    Agent *localAgent = agent;
    while (localAgent != NULL) {
        list.push_back(localAgent);
        localAgent = localAgent->ancestor;
    }
    // Start with the most-evolved animat and trace backwards through the line
    // of descent
    for (int agentIndex = (int)list.size() - 1; agentIndex > 0; agentIndex--) {
        agent = list[agentIndex];
        if ((agent->born & LOD_RECORD_INTERVAL) == 0) {
            // Larissa: set noise to 0 for analysis
            fprintf(statsFile, "%i   %i  %i", agent->born, agent->correct,
                    agent->incorrect);
            for (int i = 0; i < agent->numCorrectByPattern.size(); i++) {
                fprintf(statsFile, " %i", agent->numCorrectByPattern[i]);
            }
            fprintf(statsFile, "\n");
            // TODO(wmayner) don't think we need the genome file; apparently
            // nobody understands it anyway :'(
            agent->saveGenome(genomeFile);
        }
    }
}
