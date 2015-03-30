// measures.cpp

#include "measures.h"


double mutualInformation(vector<int> A, vector<int> B) {
    set<int> nrA,nrB;
    set<int>::iterator aI,bI;
    map< int, map<int, double> > pXY;
    map<int, double> pX,pY;
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

double entropy(vector<int> list) {
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

double predictiveI(vector<int> A) {
    vector<int> S,I;
    S.clear(); I.clear();
    for(int i=0;i<A.size();i++){
        S.push_back((A[i]>>12)&15);
        I.push_back(A[i]&3);
    }
    return mutualInformation(S, I);
}

double nonPredictiveI(vector<int> A) {
    vector<int> S,I;
    S.clear(); I.clear();
    for(int i=0;i<A.size();i++){
        S.push_back((A[i]>>12)&15);
        I.push_back(A[i]&3);
    }
    return entropy(I)-mutualInformation(S, I);
}

double predictNextInput(vector<int> A) {
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
