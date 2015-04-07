// Agent.cpp

#include "Agent.hpp"


Agent::Agent(){
	nrPointingAtMe=1;
	ancestor = NULL;
	for(int i = 0; i < NUM_NODES; i++) {
		states[i] = 0;
		newStates[i] = 0;
	}
	ID=masterID;
	masterID++;
	saved=false;
	hmmus.clear();
	nrOfOffspring=0;
	totalSteps=0;
	food=0;
}

Agent::~Agent(){
	for(int i=0;i<hmmus.size();i++)
		delete hmmus[i];
	if(ancestor!=NULL){
		ancestor->nrPointingAtMe--;
		if(ancestor->nrPointingAtMe==0)
			delete ancestor;
	}
}

void Agent::setupEmptyAgent(int nucleotides) {
	genome.resize(nucleotides);
	for (int i = 0; i < nucleotides; i++) {
		genome[i]=127;
    }
	setupPhenotype();
}
void Agent::loadAgent(char* filename){
	FILE *f=fopen(filename,"r+t");
	int i;
	genome.clear();
	while(!(feof(f))){
		fscanf(f,"%i	",&i);
		genome.push_back((unsigned char)(i&255));
	}
    fclose(f);
	setupPhenotype();
}
void Agent::loadAgentWithTrailer(char* filename){
	FILE *f=fopen(filename,"r+t");
	int i;
	genome.clear();
	fscanf(f,"%i	",&i);
	while(!(feof(f))){
		fscanf(f,"%i	",&i);
		genome.push_back((unsigned char)(i&255));
	}
	setupPhenotype();
}


void Agent::ampUpStartCodons(void){
	int i,j;
	for(i=0;i<genome.size();i++)
		genome[i]=rand()&255;
	for(i=0;i<4;i++)
	{
		j=rand()%(genome.size()-100);
		genome[j]=42;
		genome[j+1]=(255-42);
		for(int k=2;k<20;k++)
			genome[j+k]=rand()&255;
	}
}

void Agent::inherit(Agent *from,double mutationRate,int theTime){
	int nucleotides=from->genome.size();
	int i,s,o,w;
	vector<unsigned char> buffer;
	born=theTime;
	ancestor=from;
	from->nrPointingAtMe++;
	from->nrOfOffspring++;
	genome.clear();
	genome.resize(from->genome.size());
	for(i=0;i<nucleotides;i++)
		if(((double)rand()/(double)RAND_MAX)<mutationRate)
			genome[i]=rand()&255;
		else
			genome[i]=from->genome[i];
	if((((double)rand()/(double)RAND_MAX)<0.05)&&(genome.size()<20000)){
		//duplication
		w=15+rand()&511;
		s=rand()%(genome.size()-w);
		o=rand()%genome.size();
		buffer.clear();
		buffer.insert(buffer.begin(),genome.begin()+s,genome.begin()+s+w);
		genome.insert(genome.begin()+o,buffer.begin(),buffer.end());
	}
	if((((double)rand()/(double)RAND_MAX)<0.02)&&(genome.size()>1000)){
		//deletion
		w=15+rand()&511;
		s=rand()%(genome.size()-w);
		genome.erase(genome.begin()+s,genome.begin()+s+w);
	}
	setupPhenotype();
	fitness=0.0;
}

void Agent::setupPhenotype(void){
	int i;
	HMMU *hmmu;
	if(hmmus.size()!=0)
		for(i=0;i<hmmus.size();i++)
			delete hmmus[i];
	hmmus.clear();
	for(i=0;i<genome.size();i++){
		if((genome[i]==42)&&(genome[(i+1)%genome.size()]==(255-42))){
			hmmu=new HMMU(genome, i);
			hmmus.push_back(hmmu);
		}
	}
}

void Agent::resetBrain(void){
	for(int i=0;i<NUM_NODES;i++)
		states[i]=0;
}

void Agent::updateStates(void){
	int i;
	for(i=0;i<hmmus.size();i++)
		hmmus[i]->update(&states[0],&newStates[0]);
	for(i=0;i<NUM_NODES;i++){
		states[i]=newStates[i];
		newStates[i]=0;
	}
	totalSteps++;
}

void Agent::saveLogicTable(FILE *f){
	int i,j;
	fprintf(f,"0_t0,1_t0,2_t0,3_t0,4_t0,5_t0,6_t0,7_t0,,0_t1,1_t1,2_t1,3_t1,4_t1,5_t1,6_t1,7_t1\n");
	for(i=0;i<256;i++){
		for(j=0;j<8;j++){
			fprintf(f,"%i,",(i>>j)&1);
			states[j]=(i>>j)&1;
		}
		updateStates();
		for(j=0;j<8;j++){
			fprintf(f,",%i",states[j]);
		}
		fprintf(f,"\n");
	}
}

void Agent::saveLogicTableSingleAnimat(FILE *f){
	int i,j,k;
	fprintf(f,"0_t0,1_t0,2_t0,3_t0,4_t0,5_t0,6_t0,7_t0,,0_t1,1_t1,2_t1,3_t1,4_t1,5_t1,6_t1,7_t1\n");
	for(i=0;i<256;i++){
		for(j=0;j<8;j++){
			fprintf(f,"%i,",(i>>j)&1);
			states[j]=(i>>j)&1;
		}
        // update States deterministically, without using random number generator
        for(k=0;k<hmmus.size();k++)
            hmmus[k]->update(&states[0],&newStates[0]);
        for(k=0;k<NUM_NODES;k++){
            states[k]=newStates[k];
            newStates[k]=0;
        }
		for(j=0;j<8;j++){
			fprintf(f,",%i",states[j]);
		}
		fprintf(f,"\n");
	}
}

void Agent::saveGenome(FILE *f){
	int i;
	for(i=0;i<genome.size();i++)
		fprintf(f,"%i	",genome[i]);
	fprintf(f,"\n");
}

void Agent::saveEdgeList(char *filename){
	FILE *f=fopen(filename,"w+t");
	int i,j,k;
	for(i=0;i<hmmus.size();i++){
		for(j=0;j<hmmus[i]->ins.size();j++){
			for(k=0;k<hmmus[i]->outs.size();k++)
				fprintf(f,"%i   %i\n",hmmus[i]->ins[j],hmmus[i]->outs[k]);
		}
	}
	fclose(f);
}


