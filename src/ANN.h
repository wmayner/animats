/*
 *  ANN.h
 *  HMM_representations
 *
 *  Created by Arend on 12/20/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ANN_h_included_
#define _ANN_h_included_

#include <vector>
#include <deque>
#include <iostream>
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;
class tLayer{
public:
	vector<vector<double> > weights;
	vector<double> inStates,outStates;
	void setup(int n_in,int n_out);
	void update(bool useTanH);
};

class ANN{
public:
	vector<tLayer> layers;
	void setup(void);
	void inherit(ANN *ancestor,double mutationRate);
	void saveLOD(FILE *genomeFile);
	void load(char *filename);
	void update(unsigned char *states);
	void resetBrain(void);
};

#endif
