// Agent.cpp

#include <vector>

#include "Agent.hpp"


Agent::Agent() {
    nrPointingAtMe = 1;
    ancestor = NULL;
    for (int i = 0; i < NUM_NODES; i++) {
        states[i] = 0;
        newStates[i] = 0;
    }
    ID = masterID++;
    hmmus.clear();
}

Agent::~Agent() {
    for (int i = 0; i < (int)hmmus.size(); i++) {
        delete hmmus[i];
    }
    if (ancestor != NULL) {
        ancestor->nrPointingAtMe--;
        if (ancestor->nrPointingAtMe == 0)
            delete ancestor;
    }
}

void Agent::injectStartCodons() {
    for (int i = 0; i < (int)genome.size(); i++)
        genome[i] = rand() & 255;
    for (int i = 0; i < 4; i++) {
        int j = rand() % ((int)genome.size() - 100);
        genome[j] = 42;
        genome[j + 1]= 213;
        for (int k = 2; k < 20; k++)
            genome[j + k] = rand() & 255;
    }
}

void Agent::setupEmptyAgent(int nucleotides) {
    genome.resize(nucleotides);
    for (int i = 0; i < nucleotides; i++) {
        genome[i] = 127;
    }
    setupPhenotype();
}

void Agent::inherit(Agent *parent, int generation) {
    int nucleotides = (int)parent->genome.size();
    vector<unsigned char> buffer;
    born = generation;
    ancestor = parent;
    parent->nrPointingAtMe++;
    genome.clear();
    genome.resize(parent->genome.size());
    // Mutation
    for (int i = 0; i < nucleotides; i++) {
        if (randDouble < MUTATION_RATE) {
            genome[i] = rand() & 255;
        } else {
            genome[i] = parent->genome[i];
        }
    }
    if ((randDouble < DUPLICATION_PROB) &&
            ((int)genome.size() < MAX_GENOME_LENGTH)) {
        int w = (MIN_DUP_DEL_WIDTH + rand()) & MAX_DUP_DEL_WIDTH;
        // Duplication
        int s = rand() % ((int)genome.size() - w);
        int o = rand() % (int)genome.size();
        buffer.clear();
        buffer.insert(buffer.begin(), genome.begin() + s,
                genome.begin() + s + w);
        genome.insert(genome.begin() + o, buffer.begin(), buffer.end());
    }
    if ((randDouble < DELETION_PROB) &&
            ((int)genome.size() > MIN_GENOME_LENGTH)) {
        // Deletion
        int w = (MIN_DUP_DEL_WIDTH + rand()) & MAX_DUP_DEL_WIDTH;
        int s = rand() % ((int)genome.size() - w);
        genome.erase(genome.begin() + s, genome.begin() + s + w);
    }
    setupPhenotype();
    fitness = 0.0;
}

void Agent::setupPhenotype() {
    if (hmmus.size() != 0) {
        for (int i = 0; i < (int)hmmus.size(); i++) {
            delete hmmus[i];
        }
    }
    hmmus.clear();
    HMMU *hmmu;
    for (int i = 0; i < (int)genome.size(); i++) {
        if ((genome[i] == 42) && (genome[(i + 1) % (int)genome.size()] == 213)) {
            hmmu = new HMMU(genome, i);
            hmmus.push_back(hmmu);
        }
    }
}

void Agent::resetBrain(void) {
    for (int i = 0; i < NUM_NODES; i++)
        states[i] = 0;
}

void Agent::updateStates(void) {
    for (int i = 0; i < (int)hmmus.size(); i++) {
        hmmus[i]->update(&states[0], &newStates[0]);
    }
    for (int i = 0; i < NUM_NODES; i++) {
        states[i] = newStates[i];
        newStates[i] = 0;
    }
}

void Agent::loadAgent(char* filename) {
    FILE *f = fopen(filename, "r+t");
    int i;
    genome.clear();
    while (!(feof(f))) {
        fscanf(f, "%i    ", &i);
        genome.push_back((unsigned char)(i & 255));
    }
    fclose(f);
    setupPhenotype();
}

void Agent::saveTPM(FILE *f) {
    fprintf(f,"past_state,current_state\n");
    // For all possible states...
    for (int i = 0; i < NUM_STATES; i++) {
        // Set the animat to that state
        for (int j = 0; j < NUM_NODES; j++)
            states[j]= (i >> j) & 1;
        // Write that state to the first column
        for (int j = 0; j < (NUM_NODES - 1); j++)
            fprintf(f, "%i", (i >> j) & 1);
        fprintf(f, "%i,", (i >> (NUM_NODES - 1)) & 1);
        // Get the next state
        updateStates();
        // Write that state to the second column
        for (int j = 0; j < (NUM_NODES - 1); j++) {
            fprintf(f, "%i", states[j]);
        }
        fprintf(f, "%i\n", (i >> (NUM_NODES - 1)) & 1);
    }
}

void Agent::appendGenomeToFile(FILE *f) {
    fprintf(f, "%i,", born);
    for (int i = 0; i < (int)genome.size() - 1; i++) {
        fprintf(f, "%i ", genome[i]);
    }
    fprintf(f, "%i", genome[genome.size() - 1]);
    fprintf(f, "\n");
}

void Agent::saveEdgeList(char *filename) {
    FILE *f = fopen(filename, "w+t");
    fprintf(f, "source,target\n");
    for (int i = 0; i < (int)hmmus.size(); i++) {
        for (int j = 0; j < (int)hmmus[i]->ins.size(); j++) {
            for (int k = 0; k < (int)hmmus[i]->outs.size(); k++) {
                fprintf(f, "%i,%i\n", hmmus[i]->ins[j], hmmus[i]->outs[k]);
            }
        }
    }
    fclose(f);
}
