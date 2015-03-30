/*
 *  HMM.h
 *  HMMBrain
 *
 *  Created by Arend on 9/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _HMM_h_included_
#define _HMM_h_included_

#include <vector>
#include <deque>
#include <iostream>
#include "globalConst.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class HMMU{
public:
	vector<vector<unsigned char> > hmm;
	vector<unsigned int> sums;
	vector<unsigned char> ins,outs;
	unsigned char posFBNode,negFBNode;
	unsigned char nrPos,nrNeg;
	vector<int> posLevelOfFB,negLevelOfFB;
	deque<unsigned char> chosenInPos,chosenInNeg,chosenOutPos,chosenOutNeg;

	unsigned char _xDim,_yDim;
	HMMU();
	~HMMU();
	void setup(vector<unsigned char> &genome, int start);
	void setupQuick(vector<unsigned char> &genome, int start);
	void update(unsigned char *states,unsigned char *newStates);
    void deterministicUpdate(unsigned char *states,unsigned char *newStates);
	void show(void);

};

#endif
