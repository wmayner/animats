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


class PyPhi {
    PyObject* pyphi;
    PyObject* network_constructor;
    PyObject* subsystem_constructor;
    PyObject* compute;
    PyObject* py_big_phi;
    PyObject* py_conceptual_information;
 public:
    PyPhi();
    ~PyPhi();

    PyObject* network(vector< vector<double> >& c_tpm, vector<long>&
            current_state, vector<long>& past_state, vector< vector<double> >&
            c_cm);
    PyObject* subsystem(vector<long>& node_indices, PyObject* network);

    double big_phi(PyObject* subsystem);
    double conceptual_information(PyObject* subsystem);
};

#endif  // SRC_PYPHI_H_
