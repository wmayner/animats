// Agent.h

#ifndef _SRC_AGENT_H_
#define _SRC_AGENT_H_

#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "constants.h"
#include "HMM.h"
#include "ANN.h"

//#define useANN

using namespace std;

static int masterID=0;


class tDot{
public:
	double xPos,yPos;
};


class Agent{
public:
	vector<HMMU*> hmmus;
	vector<unsigned char> genome;
	vector<tDot> dots;
#ifdef useANN
	ANN *ANN;
#endif

	Agent *ancestor;
	unsigned int nrPointingAtMe;
	unsigned char states[numNodes], newStates[numNodes];
	double fitness, convFitness;
	vector<double> fitnesses;
	int food;

	double xPos, yPos, direction;
	double sX, sY;
	bool foodAlreadyFound;
	int steps, bestSteps, totalSteps;
	int ID, nrOfOffspring;
	bool saved;
	bool retired;
	int born;
	int correct, incorrect;
    vector<int> numCorrectByPattern;

	Agent();
	~Agent();
	void setupRandomAgent(int nucleotides);
	void loadAgent(char* filename);
	void loadAgentWithTrailer(char* filename);
	void setupPhenotype(void);
	void inherit(Agent *from, double mutationRate, int theTime);
	unsigned char * getStatesPointer(void);
	void updateStates(void);
	void resetBrain(void);
	void ampUpStartCodons(void);
	void showBrain(void);
	void showPhenotype(void);
	void saveToDot(char *filename);
	void saveToDotFullLayout(char *filename);

	void initialize(int x, int y, int d);
	Agent* findLMRCA(void);
	void saveFromLMRCAtoNULL(FILE *statsFile, FILE *genomeFile);
	void saveLOD(FILE *statsFile, FILE *genomeFile);
	void retire(void);
	void setupDots(int x, int y, double spacing);
	void saveLogicTable(FILE *f);
    void saveLogicTableSingleAnimat(FILE *f);
	void saveGenome(FILE *f);
    void saveEdgeList(char *filename);
};

#endif  // _SRC_AGENT_H_
