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

    _xDim = 1 + (genome[(k++) % genome.size()] & 3);
    _yDim = 1 + (genome[(k++) % genome.size()] & 3);
    ins.resize(_yDim);
    outs.resize(_xDim);
    for (i = 0; i < _yDim; i++)
        ins[i] = genome[(k + i) % genome.size()] & (NUM_NODES - 1);
    for (i = 0; i < _xDim; i++)
        outs[i] = genome[(k + 4 + i) % genome.size()] & (NUM_NODES - 1);
    // TODO(wmayner) parametrize this hardcoded constant
    k = k + 16;
    hmm.resize(1 << _yDim);
    sums.resize(1 << _yDim);
    for (i = 0; i < (1 << _yDim); i++) {
        hmm[i].resize(1 << _xDim);
        for (j = 0; j < (1 << _xDim); j++) {
            hmm[i][j] = genome[(k + j + ((1 << _xDim) * i)) % genome.size()];
            if (hmm[i][j] == 0)
                hmm[i][j] = 1;
            sums[i] += hmm[i][j];
        }
    }
}

void HMMU::setupQuick(vector<unsigned char> &genome, int start) {
    int i, j, k;
    ins.clear();
    outs.clear();

    // Larissa: The % (mod) is to make the genome circular
    k = (start + 2) % genome.size();

    _xDim = 1 + (genome[(k++) % genome.size()] & 3);
    _yDim = 1 + (genome[(k++) % genome.size()] & 3);
    ins.resize(_yDim);
    outs.resize(_xDim);
    for (i = 0; i < _yDim; i++)
        ins[i] = genome[(k + i) % genome.size()] & (NUM_NODES - 1);
    for (i = 0; i < _xDim; i++)
        outs[i] = genome[(k + 4 + i) % genome.size()] & (NUM_NODES - 1);

    k += 16;
    hmm.resize(1 << _yDim);
    sums.resize(1 << _yDim);
    for (i = 0; i < (1 << _yDim); i++) {
        hmm[i].resize(1 << _xDim);
        for (j = 0; j < (1 << _xDim); j++) {
            hmm[i][j] = 0;
        }
        hmm[i][genome[(k + j+ ((1 << _xDim) * i)) % genome.size()] &
                ((1 << _xDim) - 1)] = 255;
        sums[i] = 255;
    }
}

void HMMU::update(unsigned char *states, unsigned char *newStates) {
    int I = 0;
    int i, j, r;
    for (i = 0; i < ins.size(); i++)
        I = (I << 1) + ((states[ins[i]]) & 1);
    // r is a random number between probably 1 and 255 and is only important
    // for probabilistic gates
    r = 1 + (rand() % (sums[I] - 1));
    j = 0;
    // for deterministic gates the while loop doesn't do anything except
    // increase j until hmm[I][j] is 255
    while (r > hmm[I][j]) {
        r -= hmm[I][j];
        j++;
    }
    for (i = 0; i < outs.size(); i++) {
        newStates[outs[i]] |= (j >> i) & 1;
        // newStates[outs[i]] = (j >> i) & 1;
    }
}

void HMMU::deterministicUpdate(
        unsigned char *states, unsigned char *newStates) {
    int I = 0;
    int i , j;
    for (i = 0; i < ins.size(); i++)
        I = (I << 1) + ((states[ins[i]]) & 1);
    j = 0;
    // r above doesn't do anything else then checking if hmm is larger than 0
    while (1 > hmm[I][j]) {
        j++;
    }
    for (i = 0; i < outs.size(); i++) {
        newStates[outs[i]] |= (j >> i) & 1;
    }
    // newStates[outs[i]] = (j >> i) & 1;
}

void HMMU::show(void) {
    int i, j;
    cout << "INS: ";
    for (i = 0; i < ins.size(); i++) {
        cout << (int)ins[i] <<" ";
    }
    cout << endl;
    cout << "OUTS: ";
    for (i = 0; i < outs.size(); i++) {
        cout << (int)outs[i] << " ";
    }
    cout << endl;
    for (i = 0; i < hmm.size(); i++) {
        for (j = 0; j < hmm[i].size(); j++) {
            cout << " " << (double)hmm[i][j] / sums[i];
        }
        cout << endl;
    }
    // for(i=0;i<hmm.size();i++){
    //     for(j=0;j<hmm[i].size();j++)
    //         cout<<(int)hmm[i][j]<<" ";
    //     cout<<endl;
    // }
}
