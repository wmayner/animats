// HMM.h

#ifndef _SRC_HMM_H_
#define _SRC_HMM_H_

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <deque>
#include <iostream>

#include "./constants.h"

using namespace std;

class HMMU{
 public:
	vector< vector<unsigned char> > hmm;
	vector<unsigned int> sums;
	vector<unsigned char> ins, outs;
	unsigned char posFBNode, negFBNode;
	unsigned char nrPos, nrNeg;
	vector<int> posLevelOfFB, negLevelOfFB;
	deque<unsigned char> chosenInPos, chosenInNeg, chosenOutPos, chosenOutNeg;

	unsigned char _xDim, _yDim;
	HMMU();
	~HMMU();
	void setup(vector<unsigned char> &genome, int start);
	void setupQuick(vector<unsigned char> &genome, int start);
	void update(unsigned char *states, unsigned char *newStates);
    void deterministicUpdate(unsigned char *states, unsigned char *newStates);
	void show(void);
};

#endif  // _SRC_HMM_H_
