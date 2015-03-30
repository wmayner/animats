/*
 *  HMM.cpp
 *  HMMBrain
 *
 *  Created by Arend on 9/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "HMM.h"
#include <iostream>
//#define feedbackON

HMMU::HMMU(){
}

HMMU::~HMMU(){
	hmm.clear();
	sums.clear();
	ins.clear();
	outs.clear();
	posLevelOfFB.clear();
	negLevelOfFB.clear();
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
}
void HMMU::setup(vector<unsigned char> &genome, int start){
	int i,j,k;
	ins.clear();
	outs.clear();
	k=(start+2)%genome.size();

	_xDim=1+(genome[(k++)%genome.size()]&3);
	_yDim=1+(genome[(k++)%genome.size()]&3);
	posFBNode=genome[(k++)%genome.size()]&(numNodes-1);
	negFBNode=genome[(k++)%genome.size()]&(numNodes-1);
	nrPos=genome[(k++)%genome.size()]&3;
	nrNeg=genome[(k++)%genome.size()]&3;
	//cout<<"setup "<<(int)genome[start+2]<<" "<<(int)xDim<<" "<<(int)yDim<<endl;
	ins.resize(_yDim);
	outs.resize(_xDim);
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(numNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%genome.size()]&(numNodes-1);
	for(i=0;i<nrPos;i++)
		posLevelOfFB[i]=(int)(1+genome[(k+8+i)%genome.size()]);
	for(i=0;i<nrNeg;i++)
		negLevelOfFB[i]=(int)(1+genome[(k+12+i)%genome.size()]);
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();

	k=k+16;
	hmm.resize(1<<_yDim);
	sums.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		hmm[i].resize(1<<_xDim);
		for(j=0;j<(1<<_xDim);j++){
//			hmm[i][j]=(genome[(k+j+((1<<yDim)*i))%genome.size()]&1)*255;
			hmm[i][j]=genome[(k+j+((1<<_xDim)*i))%genome.size()];
			if(hmm[i][j]==0) hmm[i][j]=1;
			sums[i]+=hmm[i][j];
		}
	}
}

void HMMU::setupQuick(vector<unsigned char> &genome, int start){
	int i,j,k;
	ins.clear();
	outs.clear();
	k=(start+2)%genome.size(); //Larissa: The % (mod) is to make the genome circular

	_xDim=1+(genome[(k++)%genome.size()]&3);
	_yDim=1+(genome[(k++)%genome.size()]&3);
	posFBNode=genome[(k++)%genome.size()]&(numNodes-1);
	negFBNode=genome[(k++)%genome.size()]&(numNodes-1);
	nrPos=genome[(k++)%genome.size()]&3;
	nrNeg=genome[(k++)%genome.size()]&3;
	//cout<<"setup "<<(int)genome[start+2]<<" "<<(int)xDim<<" "<<(int)yDim<<endl;
	ins.resize(_yDim);
	outs.resize(_xDim);
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(numNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%genome.size()]&(numNodes-1);
	for(i=0;i<nrPos;i++)
		posLevelOfFB[i]=(int)(1+genome[(k+8+i)%genome.size()]);
	for(i=0;i<nrNeg;i++)
		negLevelOfFB[i]=(int)(1+genome[(k+12+i)%genome.size()]);
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();

	k=k+16;
	hmm.resize(1<<_yDim);
	sums.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		hmm[i].resize(1<<_xDim);
		for(j=0;j<(1<<_xDim);j++)
			hmm[i][j]=0;
		hmm[i][genome[(k+j+((1<<_xDim)*i))%genome.size()]&((1<<_xDim)-1)]=255;
		sums[i]=255;
	}

}

void HMMU::update(unsigned char *states,unsigned char *newStates){
	int I=0;
	int i,j,r;
#ifdef feedbackON
    cout<<"FeedbackON"<<endl;
	if((nrPos!=0)&&(states[posFBNode]==1)){
		for(i=0;i<chosenInPos.size();i++){
            // A random number is drawn because the program allows for non-deterministic mechanisms
			mod=(unsigned char)(rand()%(int)posLevelOfFB[i]);
			if((hmm[chosenInPos[i]][chosenOutPos[i]]+mod)<255){
				hmm[chosenInPos[i]][chosenOutPos[i]]+=mod;
				sums[chosenInPos[i]]+=mod;
			}
		}
	}
	if((nrNeg!=0)&&(states[negFBNode]==1)){
		for(i=0;i<chosenInNeg.size();i++){
			mod=(unsigned char)(rand()%(int)negLevelOfFB[i]);
			if((hmm[chosenInNeg[i]][chosenOutNeg[i]]-mod)>0){
				hmm[chosenInNeg[i]][chosenOutNeg[i]]-=mod;
				sums[chosenInNeg[i]]-=mod;
			}
		}
	}
#endif
	for(i=0;i<ins.size();i++)
		I=(I<<1)+((states[ins[i]])&1);
    // r is a random number between probably 1 and 255 and is only important for probabilistic gates
	r=1+(rand()%(sums[I]-1));
	j=0;
    // for deterministic gates the while loop doesn't do anything except increase j until hmm[I][j] is 255
	while(r>hmm[I][j]){
        r-=hmm[I][j];
		j++;
	}
	for(i=0;i<outs.size();i++)
		newStates[outs[i]]|=(j>>i)&1;
		//newStates[outs[i]]=(j>>i)&1;
#ifdef feedbackON
	chosenInPos.push_back(I);
	chosenInNeg.push_back(I);
	chosenOutPos.push_back(j);
	chosenOutNeg.push_back(j);
	while(chosenInPos.size()>nrPos) chosenInPos.pop_front();
	while(chosenOutPos.size()>nrPos) chosenOutPos.pop_front();
	while(chosenInNeg.size()>nrNeg) chosenInNeg.pop_front();
	while(chosenOutNeg.size()>nrNeg) chosenOutNeg.pop_front();
#endif
}

void HMMU::deterministicUpdate(unsigned char *states,unsigned char *newStates){
	int I=0;
	int i,j;
	for(i=0;i<ins.size();i++)
		I=(I<<1)+((states[ins[i]])&1);
	j=0;
    // r above doesn't do anything else then checking if hmm is larger than 0
	while(1>hmm[I][j]){
		j++;
	}
	for(i=0;i<outs.size();i++)
		newStates[outs[i]]|=(j>>i)&1;
    //newStates[outs[i]]=(j>>i)&1;
}

void HMMU::show(void){
	int i,j;
	cout<<"INS: ";
	for(i=0;i<ins.size();i++)
		cout<<(int)ins[i]<<" ";
	cout<<endl;
	cout<<"OUTS: ";
	for(i=0;i<outs.size();i++)
		cout<<(int)outs[i]<<" ";
	cout<<endl;
	for(i=0;i<hmm.size();i++){
		for(j=0;j<hmm[i].size();j++)
			cout<<" "<<(double)hmm[i][j]/sums[i];
		cout<<endl;
	}
	cout<<endl;
	cout<<"posFB: "<<(int)posFBNode<<" negFB: "<<(int)negFBNode<<endl;
	cout<<"posQue:"<<endl;
	for(i=0;i<posLevelOfFB.size();i++)
		cout<<(int)posLevelOfFB[i]<<" ";
	cout<<endl;
	cout<<"negQue:"<<endl;
	for(i=0;i<negLevelOfFB.size();i++)
		cout<<(int)negLevelOfFB[i]<<" ";
	cout<<endl;
/*
	for(i=0;i<hmm.size();i++){
		for(j=0;j<hmm[i].size();j++)
			cout<<(int)hmm[i][j]<<" ";
		cout<<endl;
	}
	*/
//	cout<<"------"<<endl;
}
