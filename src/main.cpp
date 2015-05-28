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

int repeats = 1;
double sensorNoise = 0.0;

void saveLODandGenomes(Agent *agent, FILE *statsFile, FILE *genomeFile);

int main(int argc, char *argv[]) {
    Game *game;
    FILE *LODFile = fopen(argv[2], "w+t");
    FILE *genomeFile = fopen(argv[3], "w+t");

    int localSeed = atoi(argv[5]);
    if (localSeed != -1) {
        srand(localSeed);
    } else {
        srand(getpid());
    }

    // Population and offspring containers.
    vector<Agent*> agent;
    agent.resize(NUM_AGENTS);
    vector<Agent*> nextGen;
    nextGen.resize(agent.size());

    game = new Game(argv[1]);

    sensorNoise = atof(argv[6]);

    // Prototypical Agent from which all descend.
    Agent *masterAgent = new Agent;
    masterAgent->setupEmptyAgent(5000);
    masterAgent->setupPhenotype();
    masterAgent->born = 0;

    // Agent whose line of descent will be traced in the analysis.
    Agent *bestAgent = masterAgent;

    int generation = 1;
    // Make the initial population.
    for (int i = 0; i < (int)agent.size(); i++) {
        agent[i] = new Agent;
        agent[i]->inherit(masterAgent, generation);
        // Set fitness to 1 so agents will be selected in the first selection.
        agent[i]->fitness = 1.0;
    }

    int startTime = time(NULL);

    int who = 0;
    double maxFitness = 1.0;

    cout << "Beginning simulation of " << NUM_GENERATIONS << " generations." <<
      endl;

    while (generation < NUM_GENERATIONS + 1) {
        // Variant of roulette-wheel selection that uses the maximum fitness
        // rather than the average.
        // See http://en.wikipedia.org/wiki/Fitness_proportionate_selection
        for (int i = 0; i < (int)agent.size(); i++) {
            Agent *d;
            d = new Agent;
            int j;
            do {
                j = rand() % (int)agent.size();
            } while (randDouble > (agent[j]->fitness / maxFitness));
            d->inherit(agent[j], generation);
            nextGen[i] = d;
        }
        // Delete lines of descent that died out.
        for (int i = 0; i < (int)agent.size(); i++) {
            agent[i]->nrPointingAtMe--;
            if (agent[i]->nrPointingAtMe == 0)
                delete agent[i];
        }
        // Replace current population with offspring.
        agent = nextGen;


        // Fitness evaluation.
        for (int i = 0; i < (int)agent.size(); i++) {
            agent[i]->fitnesses.clear();
            for (int j = 0; j < repeats; j++) {
                game->executeGame(agent[i], sensorNoise, j);
                agent[i]->fitnesses.push_back((float)agent[i]->correct);
            }
        }
        maxFitness = 0.0;
        for (int i = 0; i < (int)agent.size(); i++) {
            agent[i]->fitness = 1.0;
            // Set fitness to the number correct.
            for (int j = 0; j < repeats; j++)
                agent[i]->fitness *= agent[i]->fitnesses[j];
            if (repeats <= 1) {
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

        bestAgent = agent[who];
        generation++;

        if (generation % PRINT_INTERVAL == 0) {
          cout << "Generation " << generation << ": [fitness] " <<
            (double)bestAgent->fitness << " [correct/incorrect] " <<
            bestAgent->correct << "/" << bestAgent->incorrect << endl;
        }
    }

    int endTime = time(NULL);
    cout << "Finished simulating " << NUM_GENERATIONS <<
      " generations. Elapsed time: " << (endTime - startTime) << " seconds." <<
      endl;
    // Larissa: set sensor noise to 0 for analysis
    makeFullAnalysis(game, bestAgent, argv[4], 0);
    cout << "Finished doing full analysis." << endl;
    saveLODandGenomes(bestAgent, LODFile, genomeFile);
    cout << "Saved LOD and genomes." << endl;
    fclose(LODFile);
    fclose(genomeFile);
    return 0;
}

void saveLODandGenomes(Agent *finalAgent, FILE *LODFile, FILE *genomeFile) {
    vector<Agent*> lineage;
    Agent *agent = finalAgent;
    while (agent != NULL) {
        lineage.push_back(agent);
        agent = agent->ancestor;
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
    for (int i = (int)lineage.size() - 1; i >= 0; i--) {
        agent = lineage[i];
        if ((agent->born % LOD_RECORD_INTERVAL) == 0) {
            // Append data to LOD file
            fprintf(LODFile, "%i,%i,%i", agent->born, agent->correct,
                agent->incorrect);
            for (int j = 0; j < (int)agent->numCorrectByPattern.size(); j++) {
                fprintf(LODFile, ",%i", agent->numCorrectByPattern[j]);
            }
            fprintf(LODFile, "\n");
            // Record genome
            agent->appendGenomeToFile(genomeFile);
        }
    }
}
