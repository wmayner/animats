/*
 *  analysis.cpp
 *
 *  Created by Will Mayner on 3/27/15.
 */

#include "analysis.h"

void makeFullAnalysis(Game *game, Agent *agent, char *fileLead, double sensorNoise){
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
            table = game->executeGame(agent, sensorNoise);
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

void makeSingleAgentAnalysis(Agent *agent,char *fileLead, int agent_num){
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
