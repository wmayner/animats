// HMM.cpp

#include <vector>

#include "./HMM.hpp"


HMMU::HMMU() {}

HMMU::~HMMU() {
    hmm.clear();
    sums.clear();
    ins.clear();
    outs.clear();
}

void HMMU::setup(vector<unsigned char> &genome, int start) {
    int i, j, k;
    ins.clear();
    outs.clear();
    k = (start + 2) % genome.size();

    numInputs = 1 + (genome[(k++) % genome.size()] & 3);
    numOutputs = 1 + (genome[(k++) % genome.size()] & 3);
    ins.resize(numOutputs);
    outs.resize(numInputs);
    for (i = 0; i < numOutputs; i++)
        ins[i] = genome[(k + i) % genome.size()] & (NUM_NODES - 1);
    for (i = 0; i < numInputs; i++)
        outs[i] = genome[(k + 4 + i) % genome.size()] & (NUM_NODES - 1);
    // TODO(wmayner) parametrize this hardcoded constant
    k = k + 16;
    hmm.resize(1 << numOutputs);
    sums.resize(1 << numOutputs);
    for (i = 0; i < (1 << numOutputs); i++) {
        hmm[i].resize(1 << numInputs);
        for (j = 0; j < (1 << numInputs); j++) {
            hmm[i][j] = genome[(k + j + ((1 << numInputs) * i)) % genome.size()];
            if (hmm[i][j] == 0)
                hmm[i][j] = 1;
            sums[i] += hmm[i][j];
        }
    }
}

void HMMU::setupQuick(vector<unsigned char> &genome, int start) {
    int i, j;
    ins.clear();
    outs.clear();

    // This keeps track of where we are in the genome.
    int scan = (start + 2) % genome.size();

    numInputs = 1 + (genome[(scan++) % genome.size()] & 3);
    numOutputs = 1 + (genome[(scan++) % genome.size()] & 3);
    ins.resize(numOutputs);
    outs.resize(numInputs);
    for (i = 0; i < numOutputs; i++)
        ins[i] = genome[(scan + i) % genome.size()] & (NUM_NODES - 1);
    for (i = 0; i < numInputs; i++)
        outs[i] = genome[(scan + 4 + i) % genome.size()] & (NUM_NODES - 1);

    scan += 16;
    hmm.resize(1 << numOutputs);
    sums.resize(1 << numOutputs);
    for (i = 0; i < (1 << numOutputs); i++) {
        hmm[i].resize(1 << numInputs);
        for (j = 0; j < (1 << numInputs); j++) {
            hmm[i][j] = 0;
        }
        hmm[i][genome[(scan + j + ((1 << numInputs) * i)) % genome.size()] &
                ((1 << numInputs) - 1)] = 255;
        sums[i] = 255;
    }
}

void HMMU::update(unsigned char *currentStates, unsigned char *nextStates) {
    int i;
    // Encode the given states as an integer to index into the TPM
    int pastStateIndex = 0;
    for (i = 0; i < ins.size(); i++)
        pastStateIndex = (pastStateIndex << 1) + ((currentStates[ins[i]]) & 1);
    // Get the next state
    int nextStateIndex = 0;
    if (DETERMINISTIC) {
        // Find the index of the 1 in this row
        while (1 > hmm[pastStateIndex][nextStateIndex]) {
            nextStateIndex++;
        }
    } else {
        // Randomly pick a column index with probabilities weighted by the entries
        // in the column.
        int r = 1 + (rand() % (sums[pastStateIndex] - 1));
        while (r > hmm[pastStateIndex][nextStateIndex]) {
            // Decrease the random threshold because it's given that we didn't
            // pick column nextStateIndex, which we would have with probability
            // hmm[pastStateIndex][nextStateIndex].
            r -= hmm[pastStateIndex][nextStateIndex];
            nextStateIndex++;
        }
    }
    // The index of the column we chose is the next state (we take the its bits
    // as the next states of individual nodes)
    for (i = 0; i < outs.size(); i++) {
        nextStates[outs[i]] |= (nextStateIndex >> i) & 1;
    }
}
