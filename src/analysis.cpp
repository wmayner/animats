// analysis.cpp

#include <vector>

#include "./analysis.hpp"

void makeFullAnalysis(Game *game, Agent *finalAgent, char *fileLead, double
        sensorNoise) {
    char filename[1000];
    FILE *f;
    vector< vector<int> > table;

    vector<Agent*> lineage;
    Agent *agent = finalAgent;
    while (agent != NULL) {
        lineage.push_back(agent);
        agent = agent->ancestor;
    }
    for (int i = 0; i < (int)lineage.size(); i++) {
        agent = lineage[i];
        if ((agent->born % LOD_RECORD_INTERVAL) == 0) {
            // TPM transition list
            sprintf(filename, "%sgen-%i_tpm.csv", fileLead, agent->born);
            f = fopen(filename, "w+t");
            agent->saveTPM(f);

            // Lifetime transition list
            sprintf(filename, "%sgen-%i_transitions.csv", fileLead, agent->born);
            f = fopen(filename, "w+t");
            fprintf(f, "past_state,current_state\n");

            table = game->executeGame(agent, sensorNoise);
            for (int j = 0; j < (int)table[0].size(); j++)  {
                for (int k = 0; k < NUM_NODES - 1; k++) {
                    fprintf(f, "%i", (table[0][j] >> k) & 1);
                }
                fprintf(f, "%i,", (table[0][j] >> (NUM_NODES - 1)) & 1);
                for (int k = 0; k < NUM_NODES; k++) {
                    fprintf(f, "%i", (table[1][j] >> k) & 1);
                }
                fprintf(f, "\n");
            }
            fclose(f);

            // Edge list
            sprintf(filename, "%sgen-%i_edges.csv", fileLead, agent->born);
            agent->saveEdgeList(filename);
        }
    }
}

void makeSingleAgentAnalysis(Agent *agent, char *fileLead, int agent_num) {
    char filename[1000];
    FILE *f;
    // TPM
    sprintf(filename, "%sgen-%i_ind-%i_tpm.csv", fileLead, agent->born,
        agent_num);
    f = fopen(filename, "w+t");
    agent->saveTPM(f);
    // Number correct
    sprintf(filename, "%sgen-%i_ind-%i_fitness.csv", fileLead, agent->born,
        agent_num);
    if (agent_num == 0) {
        f = fopen(filename, "w+t");
    } else {
        f = fopen(filename, "a");
    }
    fprintf(f, "correct\n%i\n", agent->correct);
    fclose(f);
    // Edge list
    sprintf(filename, "%sgen-%i_ind-%i_edges.csv", fileLead, agent->born,
        agent_num);
    agent->saveEdgeList(filename);
}
