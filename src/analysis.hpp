// analysis.h

#ifndef SRC_ANALYSIS_H_
#define SRC_ANALYSIS_H_

#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "./constants.hpp"
#include "./Agent.hpp"
#include "./Game.hpp"

using namespace std;

void makeFullAnalysis(Game *game, Agent *agent, char *fileLead, double
        sensorNoise);
void makeSingleAgentAnalysis(Agent *agent, char *fileLead, int agent_num);

#endif  // SRC_ANALYSIS_H_
