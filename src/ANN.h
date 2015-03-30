// ANN.h

#ifndef _SRC_ANN_H
#define _SRC_ANN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vector>
#include <deque>
#include <iostream>

#include "constants.h"

using namespace std;
class Layer{
public:
	vector<vector<double> > weights;
	vector<double> inStates, outStates;
	void setup(int n_in, int n_out);
	void update(bool useTanH);
};

class ANN{
public:
	vector<Layer> layers;
	void setup(void);
	void inherit(ANN *ancestor, double mutationRate);
	void saveLOD(FILE *genomeFile);
	void load(char *filename);
	void update(unsigned char *states);
	void resetBrain(void);
};

#endif  // _SRC_ANN_H
