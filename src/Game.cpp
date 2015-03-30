/*
 *  Game.cpp
 *  HMMBrain
 *
 *  Created by Arend on 9/23/10.
 */

#include <math.h>

#include "Game.h"

// TODO(wmayner) figure out and document cruel black magic voodoo sorcery
#define KISSRND (                                                        \
    ((((rndZ = 36969 * (rndZ & 65535) + (rndZ >> 16)) << 16) +           \
      (rndW = 18000 * (rndW & 65535) + (rndW >> 16)) )                   \
     ^(rndY = 69069 * rndY + 1234567)) +                                 \
    (rndX ^= (rndX << 17), rndX ^= (rndX >> 13), rndX ^= (rndX << 5))    \
)
#define INTABS(number) (((((0x80) << ((sizeof(int) - 1) << 3)) & number) \
            ? (~number) + 1 : number))

#define randDouble ((double)rand() / (double)RAND_MAX)

int rndX, rndY, rndZ, rndW;


Game::Game(char* filename) {
    FILE *f = fopen(filename, "r+w");
    int i;
    patterns.clear();
    while (!feof(f)) {
        fscanf(f, "%i  ", &i);
        patterns.push_back(i&65535);
    }
    fclose(f);
}

Game::~Game() {}

double Game::agentDependentRandDouble(void) {
    int A = KISSRND;
    return (double)((INTABS(A)) & 65535) / (double)65535;
}

int Game::agentDependentRandInt(void) {
    int A = KISSRND;
    return (INTABS(A));
}

void Game::applyNoise(Agent *agent, double sensorNoise) {
    // Larissa: If I don't have noise in evaluation, then I can just use random
    // numbers always.
    // if (agentDependentRandDouble() < sensorNoise) {
    if (randDouble < sensorNoise) {
        agent->states[0] = !agent->states[0];
    }
    // if (agentDependentRandDouble() < sensorNoise)
    if (randDouble < sensorNoise) {
        agent->states[1] = !agent->states[1];
    }
}

vector< vector<int> > Game::executeGame(Agent* agent, double sensorNoise, int repeat) {
    int world, botPos, blockPos;
    int i, j, k, l, m, past_state, current_state;
    int action;
    // This holds the state transitions over the animat's lifetime.
    vector< vector<int> > stateTransitions;
    stateTransitions.clear();
    stateTransitions.resize(2);
    // Make random seeds unique from one another by including index.
    rndW = agent->ID + repeat;
    rndX = ~(agent->ID + repeat);
    rndY = (agent->ID + repeat)^0b01010101010101010101010101010101;
    rndZ = (agent->ID + repeat)^0b10101010101010101010101010101010;

    agent->fitness = 1.0;
    agent->correct = agent->incorrect = 0;

    bool hit;

    agent->differentialCorrects.resize(patterns.size());

    // Record the number of correct outcomes for each different type of block
    for (i = 0; i < agent->differentialCorrects.size(); i++) {
        agent->differentialCorrects[i] = 0;
    }

    // Block sizes
    for (i = 0; i < patterns.size(); i++) {
        // Directions
        for (j = -1; j < 2; j+=2) {
            // Block fall
            for (k = 0; k < 16; k++) {
                // Larissa: Change environment after 30,000 Gen, if patterns is
                // 1 7 15 3 it changes from 2 blocks with 1 7 to 4 blocks with
                // 1 7 15 3
                world = patterns[i];
                // if (agent->born > nowUpdate || i < 2) {
                // if (agent->born > nowUpdate) {
                //     world = patterns[i];
                // } else{
                //     // world = patterns[i-2];
                //     if (i == 0 || i == 2) world = 7;
                //     else if (i==1 || i == 3) world = 15;
                // }
                agent->resetBrain();
                botPos = k;
                blockPos = 0;
                // World loop
                for (l = 0; l < loopTicks; l++) {
                    // for(m=0;m<16;m++)
                    //     printf("%i",(world>>m)&1);
                    // printf("\n");
                    // AH: Sensors have no noise in them now.
                    agent->states[0] = (world >> botPos) & 1;
                    // agent->states[1] = 0;
                    agent->states[1] = (world >> ((botPos + 2) & 15)) & 1;
                    // Larissa: Set to 0 to evolve animats with just one sensor.
                    // if (agent->born > nowUpdate) {
                    //     agent->states[0] = 0;
                    //     agent->states[1] = (world >> ((botPos + 2) & 15)) & 1;
                    // }
                    // AH: Apply noise does apply noise to them now.
                    applyNoise(agent, sensorNoise);
                    // Set motors to 0 to prevent reading from them.
                    agent->states[6] = 0; agent->states[7] = 0;

                    past_state = 0;
                    for (int n = 0; n < 8; n++) {
                        // Set the nth bit to the nth node's state
                        past_state |= (agent->states[n] & 1) << n;
                    }
                    stateTransitions[0].push_back(past_state);

                    agent->updateStates();

                    current_state = 0;
                    for(int n = 0; n < 8; n++) {
                        // Set the nth bit to the nth node's state
                        current_state |= (agent->states[n] & 1) << n;
                    }
                    stateTransitions[1].push_back(current_state);

                    // Larissa: limit to one motor.
                    // agent->states[7]=0;
                    // if (agent->born < nowUpdate) {
                    //     agent->states[7] = 0;
                    // }
                    action = agent->states[6] + (agent->states[7] << 1);

                    // Move animat
                    // Larissa: this makes the animat stop moving:
                    // action = 0;
                    switch (action) {
                        // No motors on.
                        case 0:
                            // Don't move.
                            break;
                        // Both motors on.
                        case 3:
                            // Don't move.
                            break;
                        // Left motor on.
                        case 1:
                            // Move right.
                            // TODO(wmayner) replace with constant
                            botPos = (botPos + 1) & 15;
                            break;
                        // Right motor on.
                        case 2:
                            // Move left.
                            // TODO(wmayner) replace with constant
                            botPos = (botPos - 1) & 15;
                            break;
                    }

                    // Move the block.
                    if (j == -1) {
                        // Left.
                        world = ((world >> 1) & 65535) + ((world & 1) << 15);
                    } else {
                        // Right.
                        world = ((world << 1) & 65535) + ((world >> 15) & 1);
                    }
                }

                // Check for hit.
                hit = false;
                for (m = 0; m < 3; m++) {
                    if (((world >> ((botPos + m) & 15)) & 1) == 1) {
                        hit = true;
                    }
                }

                // Update fitness.
                if ((i & 1) == 0) {
                    // TODO(wmayner) replace 1.01s with constant
                    if (hit) {
                        agent->correct++;
                        agent->fitness *= 1.01;
                        agent->differentialCorrects[i]++;
                    } else {
                        agent->fitness /= 1.01;
                        agent->incorrect++;
                    }
                } else {
                    if (hit) {
                        agent->incorrect++;
                        agent->fitness /= 1.01;
                    } else {
                        agent->correct++;
                        agent->fitness *= 1.01;
                        agent->differentialCorrects[i]++;
                    }
                }
            }  // Block fall
        }  // Directions
    }  // Block sizes
    return stateTransitions;
}  // executeGame

double Game::mutualInformation(vector<int> A,vector<int>B){
    set<int> nrA,nrB;
    set<int>::iterator aI,bI;
    map<int,map<int,double> > pXY;
    map<int,double> pX,pY;
    int i;
    double c=1.0/(double)A.size();
    double I=0.0;
    for(i=0;i<A.size();i++){
        nrA.insert(A[i]);
        nrB.insert(B[i]);
        pX[A[i]]=0.0;
        pY[B[i]]=0.0;
    }
    for(aI=nrA.begin();aI!=nrA.end();aI++)
        for(bI=nrB.begin();bI!=nrB.end();bI++){
            pXY[*aI][*bI]=0.0;
        }
    for(i=0;i<A.size();i++){
        pXY[A[i]][B[i]]+=c;
        pX[A[i]]+=c;
        pY[B[i]]+=c;
    }
    for(aI=nrA.begin();aI!=nrA.end();aI++)
        for(bI=nrB.begin();bI!=nrB.end();bI++)
            if((pX[*aI]!=0.0)&&(pY[*bI]!=0.0)&&(pXY[*aI][*bI]!=0.0))
                I+=pXY[*aI][*bI]*log2(pXY[*aI][*bI]/(pX[*aI]*pY[*bI]));
    return I;

}

double Game::entropy(vector<int> list){
    map<int, double> p;
    map<int,double>::iterator pI;
    int i;
    double H=0.0;
    double c=1.0/(double)list.size();
    for(i=0;i<list.size();i++)
        p[list[i]]+=c;
    for (pI=p.begin();pI!=p.end();pI++) {
            H+=p[pI->first]*log2(p[pI->first]);
    }
    return -1.0*H;
}

double Game::computeR(vector<vector<int> > table,int howFarBack){
    double Iwh,Iws,Ish,Hh,Hs,Hw,Hhws,delta,R;
    int i;
    for(i=0;i<howFarBack;i++){
        table[0].erase(table[0].begin());
        table[1].erase(table[1].begin());
        table[2].erase(table[2].begin()+(table[2].size()-1));
    }
    table[4].clear();
    for(i=0;i<table[0].size();i++){
        table[4].push_back((table[0][i]<<14)+(table[1][i]<<10)+table[2][i]);
    }
    Iwh=mutualInformation(table[0],table[2]);
    Iws=mutualInformation(table[0],table[1]);
    Ish=mutualInformation(table[1],table[2]);
    Hh=entropy(table[2]);
    Hs=entropy(table[1]);
    Hw=entropy(table[0]);
    Hhws=entropy(table[4]);
    delta=Hhws+Iwh+Iws+Ish-Hh-Hs-Hw;
    R=Iwh-delta;
    return R;
}

double Game::computeOldR(vector<vector<int> > table){
    double Ia,Ib;
    Ia=mutualInformation(table[0], table[2]);
    Ib=mutualInformation(table[1], table[2]);
    return Ib-Ia;
}

double Game::predictiveI(vector<int>A){
    vector<int> S,I;
    S.clear(); I.clear();
    for(int i=0;i<A.size();i++){
        S.push_back((A[i]>>12)&15);
        I.push_back(A[i]&3);
    }
    return mutualInformation(S, I);
}

double Game::nonPredictiveI(vector<int>A){
    vector<int> S,I;
    S.clear(); I.clear();
    for(int i=0;i<A.size();i++){
        S.push_back((A[i]>>12)&15);
        I.push_back(A[i]&3);
    }
    return entropy(I)-mutualInformation(S, I);
}

double Game::predictNextInput(vector<int>A){
    vector<int> S,I;
    S.clear(); I.clear();
    for(int i=0;i<A.size();i++){
        S.push_back((A[i]>>12)&15);
        I.push_back(A[i]&3);
    }
    S.erase(S.begin());
    I.erase(I.begin()+I.size()-1);
    return mutualInformation(S, I);
}

void Game::makeFullAnalysis(Agent *agent,char *fileLead,double sensorNoise){
    char filename[1000];
    FILE *f;
    int i,j;
    vector<vector<int> > table;
    while(agent!=NULL){
        if((agent->born&LOD_record_Intervall)==0){
            //state to state table
            sprintf(filename,"%s_%i_FullLogicTable.txt",fileLead,agent->born);
            f=fopen(filename,"w+t");
            agent->saveLogicTable(f);
            fclose(f);
            //state to state table for only the lifetime
            sprintf(filename,"%s_%i_LifetimeLogicTable.txt",fileLead,agent->born);
            f = fopen(filename,"w+t");
            table = this->executeGame(agent, sensorNoise);
            for(i=0;i<8;i++)
                fprintf(f,"T0_%i,",i);
            fprintf(f,",");
            for(i=0;i<8;i++)
                fprintf(f,"T1_%i,",i);
            fprintf(f,"\n");
            for(j=0;j<table[0].size();j++){
                for(i=0;i<8;i++)
                    fprintf(f,"%i,",(table[0][j]>>i)&1);
                fprintf(f,",");
                for(i=0;i<8;i++)
                    fprintf(f,"%i,",(table[1][j]>>i)&1);
                fprintf(f,"\n");
            }
            fclose(f);
            //dot file
            sprintf(filename,"%s_%i_EdgeList.txt",fileLead,agent->born);
            agent->saveEdgeList(filename);
        }
        agent=agent->ancestor;
    }
}

void Game::makeSingleAgentAnalysis(Agent *agent,char *fileLead, int agent_num){
    char filename[1000];
    FILE *f;
    //state to state table
    sprintf(filename,"%s_%i_%i_FullLogicTable.txt",fileLead,agent->born,agent_num);
    f=fopen(filename,"w+t");
    agent->saveLogicTableSingleAnimat(f);
    fclose(f);
    //fitness value
    sprintf(filename,"%s_%i_Fitness.txt",fileLead,agent->born);
    if(agent_num==0)
        f=fopen(filename,"w+t");
    else
        f=fopen(filename,"a");
    fprintf(f,"%i",agent->correct);
    fprintf(f,"\n");
    fclose(f);
    //dot file
    sprintf(filename,"%s_%i_%i_EdgeList.txt",fileLead,agent->born, agent_num);
    agent->saveEdgeList(filename);
}
