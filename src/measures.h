/*
 *  measures.h
 *
 *  Created by Will Mayner on 3/30/15.
 *
 */

#ifndef SRC_MEASURES_H
#define SRC_MEASURES_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <vector>
#include <map>
#include <set>

using namespace std;

double mutualInformation(vector<int> A, vector<int> B);
double ei(vector<int> A, vector<int> B, int theMask);
double predictiveI(vector<int> A);
double nonPredictiveI(vector<int> A);
double predictNextInput(vector<int> A);
double entropy(vector<int> list);

#endif  // SRC_MEASURES_H
