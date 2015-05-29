// analysis.cpp

#include <vector>

#include "./analysis.hpp"

void saveLODandGenomes(Agent *finalAgent, FILE *LODFile, FILE *genomeFile) {
    vector<Agent*> lineage;
    Agent *agent = finalAgent;
    while (agent != NULL) {
        lineage.push_back(agent);
        agent = agent->ancestor;
    }
    // Start with the most-evolved animat and trace backwards through the line
    // of descent, saving stats and genome every (LOD_RECORD_INTERVAL)th
    // generation.
    //
    // Write CSV field names for LOD file
    fprintf(genomeFile, "gen,genome\n");
    // Write CSV field names for LOD file
    fprintf(LODFile, "gen,correct,incorrect");
    Agent *firstAgent = lineage[lineage.size() - 1];
    for (int i = 0; i < (int)firstAgent->numCorrectByPattern.size(); i++) {
        fprintf(LODFile, ",correct_pattern_%i", i);
    }
    fprintf(LODFile, "\n");
    for (int i = (int)lineage.size() - 1; i >= 0; i--) {
        agent = lineage[i];
        if ((agent->born % LOD_RECORD_INTERVAL) == 0) {
            // Append data to LOD file
            fprintf(LODFile, "%i,%i,%i", agent->born, agent->correct,
                agent->incorrect);
            for (int j = 0; j < (int)agent->numCorrectByPattern.size(); j++) {
                fprintf(LODFile, ",%i", agent->numCorrectByPattern[j]);
            }
            fprintf(LODFile, "\n");
            // Record genome
            agent->appendGenomeToFile(genomeFile);
        }
    }
}

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
