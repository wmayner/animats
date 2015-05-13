// analysis.cpp

#include <vector>

#include "./analysis.hpp"


void makeFullAnalysis(Game *game, Agent *agent, char *fileLead, double
        sensorNoise) {
    char filename[1000];
    FILE *f;
    int i, j;
    vector< vector<int> > table;

    vector<Agent*> lineage;
    Agent *localAgent = agent;
    while (localAgent != NULL) {
        lineage.push_back(localAgent);
        localAgent = localAgent->ancestor;
    }
    for (int gen = 0; gen < (int)lineage.size(); gen++) {
        agent = lineage[gen];
        if ((agent->born % LOD_RECORD_INTERVAL) == 0) {
            // TPM transition list
            sprintf(filename, "%s_gen-%i_tpm.csv", fileLead, agent->born);
            f = fopen(filename, "w+t");
            agent->saveTPM(f);
            // Lifetime transition list
            sprintf(filename, "%s_gen-%i_transitions.csv", fileLead, agent->born);
            f = fopen(filename, "w+t");
            fprintf(f, "past_state,current_state\n");
            table = game->executeGame(agent, sensorNoise);
            for (j = 0; j < (int)table[0].size(); j++)  {
                for (i = 0; i < NUM_NODES - 1; i++)
                    fprintf(f, "%i", (table[0][j] >> i) & 1);
                fprintf(f, "%i,", (table[0][j] >> i) & 1);
                for (i = 0; i < NUM_NODES; i++) {
                    fprintf(f, "%i", (table[1][j] >> i) & 1);
                }
                fprintf(f, "\n");
            }
            fclose(f);
            // Edge list
            sprintf(filename, "%s_gen-%i_edges.csv", fileLead, agent->born);
            agent->saveEdgeList(filename);
        }
        agent = agent->ancestor;
    }
}

void makeSingleAgentAnalysis(Agent *agent, char *fileLead, int agent_num) {
    char filename[1000];
    FILE *f;
    // TPM
    sprintf(filename, "%s_gen-%i_ind-%i_tpm.csv", fileLead, agent->born,
        agent_num);
    f = fopen(filename, "w+t");
    agent->saveTPM(f);
    // Number correct
    sprintf(filename, "%s_gent-%i_ind-%i_fitness.csv", fileLead, agent->born,
        agent_num);
    if (agent_num == 0) {
        f = fopen(filename, "w+t");
    } else {
        f = fopen(filename, "a");
    }
    fprintf(f, "correct\n%i\n", agent->correct);
    fclose(f);
    // Edge list
    sprintf(filename, "%s_gen-%i_ind-%i_edges.csv", fileLead, agent->born,
        agent_num);
    agent->saveEdgeList(filename);
}
