/*
 *  analysis.h
 *
 *  Created by Will Mayner on 3/27/15.
 *
 */

#ifndef SRC_ANALYSIS_H_
#define SRC_ANALYSIS_H_

#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include "constants.h"
#include "Agent.h"
#include "Game.h"

using namespace std;

void makeFullAnalysis(Game *game, Agent *agent, char *fileLead, double sensorNoise);
void makeSingleAgentAnalysis(Agent *agent, char *fileLead, int agent_num);

#endif  // SRC_ANALYSIS_H_
