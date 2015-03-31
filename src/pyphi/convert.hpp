//
//  PyInterface.h
//  CPP-conceptual-Info
//
//  Created by Jaime on 9/15/14.
//  Copyright (c) 2014 Jaime. All rights reserved.
//

#ifndef __CPP_conceptual_Info__PyInterface__
#define __CPP_conceptual_Info__PyInterface__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <Python.h>


class PyInterface {
    public:
        // Declare the default constructor
        PyInterface();
        // Declare the default destructor
        ~PyInterface();

    PyObject* vec_of_vecs_to_list_of_lists(std::vector<std::vector<double> >& matrix);
    PyObject* prob_vec_of_vecs_to_list_of_lists(std::vector<std::vector<double> >& matrix);
    PyObject* vector_to_list(std::vector<double>& vec);
    PyObject* vector_to_list_long(std::vector<long>& vec);
};


#endif /* defined(__CPP_conceptual_Info__PyInterface__) */
