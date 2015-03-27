#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
// For getpid function
#include <unistd.h>

#include <vector>
#include <map>
#include <iostream>

#include "globalConst.h"
#include "tHMM.h"
#include "tAgent.h"
#include "tGame.h"

#define randDouble ((double)rand() / (double)RAND_MAX)

using namespace std;

double perSiteMutationRate = 0.005;
int update = 0;
int repeats = 1;
int maxAgent = 100;
int totalGenerations = 4000;
char trialName[1000];
double sensorNoise = 0.0;

void saveLOD(tGame *game, tAgent *agent, FILE *statsFile, FILE *genomeFile);

int main(int argc, char *argv[]) {
    vector<tAgent*> agent;
    vector<tAgent*> nextGen;
    tAgent *masterAgent;
    int i, j, who = 0;
    tGame *game;
    double maxFitness;
    FILE *resFile;
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
    agent.resize(maxAgent);
    game = new tGame(argv[1]);

    // Larissa
    game->nowUpdate = floor(totalGenerations / 2);

    sensorNoise = atof(argv[6]);
    masterAgent = new tAgent;
    masterAgent->setupRandomAgent(5000);
    masterAgent->setupPhenotype();
    for (i = 0; i < agent.size(); i++) {
        agent[i] = new tAgent;
        agent[i]->inherit(masterAgent, 0.01, 0);
    }
    nextGen.resize(agent.size());
    masterAgent->nrPointingAtMe--;
    cout << "Setup complete." << endl;
    while (update < totalGenerations) {
        for (i = 0; i < agent.size(); i++) {
            agent[i]->fitness = 0.0;
            agent[i]->fitnesses.clear();
        }
        for (i = 0; i < agent.size(); i++) {
            for (j = 0; j < repeats; j++) {
                game->executeGame(agent[i], NULL, sensorNoise, j);
                agent[i]->fitnesses.push_back((float)agent[i]->correct);
            }
        }
        if (update == game->nowUpdate) {
            for (i = 0; i < agent.size(); i++) {
                game->makeSingleAgentAnalysis(agent[i], argv[4], i);
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

        cout << update << " " << (double)maxFitness << ""
            " " << agent[who]->correct << "/" << agent[who]->incorrect << ""
            " " << (float)agent[who]->correct / (83.0 * 82.0) << endl;

        for (i = 0; i < agent.size(); i++) {
            tAgent *d;
            d = new tAgent;
            // TODO(wmayner) change to while?
            do {
                j = rand()%(int)agent.size();
            } while (randDouble > (agent[j]->fitness / maxFitness));
            d->inherit(agent[j], perSiteMutationRate, update);
            nextGen[i] = d;
        }
        for (i = 0; i < agent.size(); i++) {
            agent[i]->retire();
            agent[i]->nrPointingAtMe--;
            if (agent[i]->nrPointingAtMe == 0)
                delete agent[i];
            agent[i] = nextGen[i];
        }
        agent = nextGen;
        update++;
    }
    // Larissa: put noise to 0 for analysis
    game->makeFullAnalysis(agent[0], argv[4], 0);
    saveLOD(game, agent[0], LOD, genomeFile);
    // agent[0]->ancestor->ancestor->saveGenome(genomeFile);
    return 0;
}

void saveLOD(tGame *game, tAgent *agent, FILE *statsFile, FILE *genomeFile) {
    vector<tAgent*> list;
    tAgent *localAgent = agent;
    while (localAgent != NULL) {
        list.push_back(localAgent);
        localAgent = localAgent->ancestor;
    }
    for (int i = (int)list.size() - 1; i > 0; i--) {
        agent = list[i];
        // TODO(wmayner) fix spacing with & below? and spelling of interval?
        if ((agent->born&LOD_record_Intervall) == 0) {
            // Larissa: set noise to 0 for analysis
            vector< vector<int> > T = game->executeGameLogStates(agent, 0);
            double R = game->computeRGiven(T[2], T[3], T[4], 4, 2, 4);
            fprintf(statsFile, "%i   %i  %i  %f", agent->born, agent->correct,
                    agent->incorrect, R);
            for (int i = 0; i < agent->differentialCorrects.size(); i++) {
                fprintf(statsFile, " %i", agent->differentialCorrects[i]);
            }
            fprintf(statsFile, "\n");
            agent->saveGenome(genomeFile);
        }
    }
}
