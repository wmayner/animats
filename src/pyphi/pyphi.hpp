//  pyphi.h

#ifndef SRC_PYPHI_H_
#define SRC_PYPHI_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <Python.h>

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <cmath>

#include "./globalConst.h"
#include "./tHMM.h"
#include "./PyInterface.h"

class tPyPhi : public PyInterface {
    public:
        // Declare the default constructor
        tPyPhi();
        // Declare the default destructor
        ~tPyPhi();

        PyObject* pyphi_network( vector<vector<double> >& c_tpm, vector<long>& current_st, vector<long>& past_state,vector<vector<double> >& conn_mat);  // create network constructor from tpm and current and past state
        PyObject* pyphi_subsystem(vector<long>& node_indices,  PyObject* network ) ; //
        double pyphi_bigphi(PyObject* subsystem);
        double pyphi_conceptual_information(PyObject* subsystem);
    public:
        PyObject *network_constructor;
        PyObject *subsystem_constructor;
        PyObject *compute;
        PyObject *big_phi;
        PyObject *conceptual_information;
        // Reference to the imported PyPhi module
        PyObject* pyphi_m;
};

#endif  // SRC_PYPHI_H_
