/*
 *  Game.h
 *  HMMBrain
 *
 *  Created by Arend on 9/23/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _Game_h_included_
#define _Game_h_included_

#include "globalConst.h"
#include "Agent.h"
#include <vector>
#include <map>
#include <set>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

#define xDim 256
#define yDim 16
#define startMazes 1
#define cPI 3.14159265

class Game{
public:
    vector<int> patterns;
	void executeGame(Agent* agent,FILE *f,double sensorNoise,int repeat);
	Game(char* filename);
	~Game();
	double mutualInformation(vector<int> A,vector<int>B);
	double ei(vector<int> A,vector<int> B,int theMask);
	double computeAtomicPhi(vector<int>A,int states);
	double predictiveI(vector<int>A);
	double nonPredictiveI(vector<int>A);
	double predictNextInput(vector<int>A);
	double computeR(vector<vector<int> > table,int howFarBack);
	double computeOldR(vector<vector<int> > table);
	double entropy(vector<int> list);

    vector<vector<int> > executeGameLogStates(Agent* agent,double sensorNoise);
	void analyseKO(Agent* agent,int which, int setTo,double sensorNoise);

    void represenationPerNodeSummary(Agent* agent,char* filename,double sensorNoise);
    void makeFullAnalysis(Agent *agent,char *fileLead,double sensorNoise);
    void makeSingleAgentAnalysis(Agent *agent,char *fileLead,int agent_num);
    double computeRGiven(vector<int>W,vector<int>S,vector<int>B,int nrWstates,int nrSstates,int nrBstates);
    void applyNoise(Agent *agent,double sensorNoise);
    double agentDependentRandDouble(void);
    int agentDependentRandInt(void);
    int nowUpdate;
};
#endif
