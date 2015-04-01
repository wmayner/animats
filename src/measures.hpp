// measures.h

#ifndef SRC_MEASURES_H_
#define SRC_MEASURES_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vector>
#include <map>
#include <set>

#include "./constants.hpp"

using namespace std;

double mutualInformation(vector<int> A, vector<int> B);
double ei(vector<int> A, vector<int> B, int theMask);
double predictiveI(vector<int> A);
double nonPredictiveI(vector<int> A);
double predictNextInput(vector<int> A);
double entropy(vector<int> list);

#endif  // SRC_MEASURES_H_
