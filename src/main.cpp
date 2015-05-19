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

using std::vector;
using std::map;
using std::endl;
using std::cout;

int generation = 0;
int repeats = 1;
double sensorNoise = 0.0;

void saveLODandGenomes(Agent *agent, FILE *statsFile, FILE *genomeFile);

int main(int argc, char *argv[]) {
    vector<Agent*> agent;
    vector<Agent*> nextGen;
    Agent *masterAgent;
    int i, j, who = 0;
    Game *game;
    double maxFitness;
    FILE *LODFile;
    FILE *genomeFile;
    LODFile = fopen(argv[2], "w+t");
    genomeFile = fopen(argv[3], "w+t");
    int localSeed = atoi(argv[5]);
    if (localSeed != -1) {
        srand(localSeed);
    } else {
        srand(getpid());
    }
    agent.resize(NUM_AGENTS);
    nextGen.resize(agent.size());

    game = new Game(argv[1]);

    sensorNoise = atof(argv[6]);

    masterAgent = new Agent;
    masterAgent->setupEmptyAgent(5000);
    masterAgent->setupPhenotype();
    for (i = 0; i < (int)agent.size(); i++) {
        agent[i] = new Agent;
        agent[i]->inherit(masterAgent, generation);
    }

    cout << "Setup complete." << endl;

    int startTime = time(NULL);
    while (generation < NUM_GENERATIONS) {
        for (i = 0; i < (int)agent.size(); i++) {
            agent[i]->fitness = 0.0;
            agent[i]->fitnesses.clear();
        }
        for (i = 0; i < (int)agent.size(); i++) {
            for (j = 0; j < repeats; j++) {
                game->executeGame(agent[i], sensorNoise, j);
                agent[i]->fitnesses.push_back((float)agent[i]->correct);
            }
        }

        maxFitness = 0.0;

        for (i = 0; i < (int)agent.size(); i++) {
            agent[i]->fitness = 1.0;
            for (j = 0; j < repeats; j++)
                agent[i]->fitness *= agent[i]->fitnesses[j];
            if (repeats <= 1) {
              // Larissa: This for one repeat
              agent[i]->fitness = pow(FITNESS_BASE, agent[i]->fitness);
            } else {
              agent[i]->fitness = pow(agent[i]->fitness, (1.0 / repeats));
              agent[i]->fitness = pow(FITNESS_BASE, agent[i]->fitness);
            }
            if (agent[i]->fitness > maxFitness) {
                who = i;
                maxFitness = agent[i]->fitness;
            }
        }

        if (generation % PRINT_INTERVAL == 0) {
          cout << "Generation " << generation << ": [fitness] " <<
            (double)maxFitness << " [correct/incorrect] " <<
            agent[who]->correct << "/" << agent[who]->incorrect << endl;
        }

        generation++;

        // Variant of roulette-wheel selection that uses the maximum fitness
        // rather than the average.
        // See http://en.wikipedia.org/wiki/Fitness_proportionate_selection
        for (i = 0; i < (int)agent.size(); i++) {
            Agent *d;
            d = new Agent;
            do {
                j = rand() % (int)agent.size();
            } while (randDouble > (agent[j]->fitness / maxFitness));
            d->inherit(agent[j], generation);
            nextGen[i] = d;
        }
        // Delete lines of descent that died out.
        for (i = 0; i < (int)agent.size(); i++) {
            agent[i]->nrPointingAtMe--;
            if (agent[i]->nrPointingAtMe == 0)
                delete agent[i];
        }
        // Replace current population with offspring.
        agent = nextGen;
    }

    int endTime = time(NULL);
    cout << "Finished simulating " << NUM_GENERATIONS << " generations. Elapsed time: " << (endTime - startTime) << " seconds." << endl;
    // Larissa: set sensor noise to 0 for analysis
    makeFullAnalysis(game, agent[0], argv[4], 0);
    saveLODandGenomes(agent[who], LODFile, genomeFile);
    return 0;
}

void saveLODandGenomes(Agent *agent, FILE *LODFile, FILE *genomeFile) {
    vector<Agent*> lineage;
    Agent *localAgent = agent;
    while (localAgent != NULL) {
        lineage.push_back(localAgent);
        localAgent = localAgent->ancestor;
    }
    // Start with the most-evolved animat and trace backwards through the line
    // of descent, saving stats and genome every (LOD_RECORD_INTERVAL)th
    // generation.
    //
    // Write CSV field names for LOD file
    fprintf(genomeFile, "gen,genome\n");
    // Write CSV field names for LOD file
    fprintf(LODFile, "gen,correct,incorrect");
    Agent *firstAgent = lineage[lineage.size() - 1];
    for (int i = 0; i < (int)firstAgent->numCorrectByPattern.size(); i++) {
        fprintf(LODFile, ",correct_pattern_%i", i);
    }
    fprintf(LODFile, "\n");
    // Skip final population whose fitness hasn't been evaluated
    for (int gen = (int)lineage.size() - 2; gen >= 0; gen--) {
        agent = lineage[gen];
        if ((agent->born % LOD_RECORD_INTERVAL) == 0) {
            // Append data to LOD file
            fprintf(LODFile, "%i,%i,%i", agent->born, agent->correct,
                agent->incorrect);
            for (int i = 0; i < (int)agent->numCorrectByPattern.size(); i++) {
                fprintf(LODFile, ",%i", agent->numCorrectByPattern[i]);
            }
            fprintf(LODFile, "\n");
            // Record genome
            agent->appendGenomeToFile(genomeFile);
        }
    }
}
