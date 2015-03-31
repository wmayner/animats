//
//  PyInterface.cpp
//  CPP-conceptual-Info
//
//  Created by Jaime on 9/15/14.
//  Copyright (c) 2014 Jaime. All rights reserved.
//

// Abstract class that provides utility methods for C++ functions that wrap Python modules.

#include "PyInterface.h"

PyInterface::PyInterface() {}
PyInterface::~PyInterface()
{
    Py_Finalize();
}

PyObject* PyInterface::vec_of_vecs_to_list_of_lists(std::vector<std::vector<double> >& matrix)
{
    long outer_len = matrix.size();
    // Create the new empty list
    PyObject *pList = PyList_New(outer_len);
    // Fill the List
    std::vector<double> innerVec;
    PyObject *innerList = NULL;
    int success;
    for (int i=0; i<outer_len; i++) {
        innerVec = matrix[i];
        innerList = PyInterface::vector_to_list(innerVec);
        if (innerList == NULL) {
            PyErr_Print();
            std::cout << "ERROR: Could not create inner list." << std::endl;
            Py_DECREF(innerList);
        }
        success = PyList_SetItem(pList, i, innerList);
        if (success != 0) {
            perror("ERROR: vector_to_list: Couldn't insert item");
        }
    }
    innerVec.clear();
    return pList;
}

PyObject* PyInterface::vector_to_list(std::vector<double>& vec) {
    // Create the new list object
    PyObject *pList = PyList_New(vec.size());
    PyObject *item = NULL;
    // Fill the list with items
    int success;
    for (int index=0; index<vec.size(); index++) {
        item = PyFloat_FromDouble(vec[index]);
        if (item == NULL) {
            std::cout << "ERROR: item = PyFloat_FromDouble(vec[index] is NULL." << std::endl;
            PyErr_Print();
            Py_DECREF(item);
        }
        PyList_SetItem(pList, index, item);
        if (success != 0) {
            perror("ERROR: vector_to_list: Couldn't insert item");
        }
    }
    return pList;
}

PyObject* PyInterface::vector_to_list_long(std::vector<long>& vec) {
    // Create the new list object
    PyObject *pList = PyList_New(vec.size());
    PyObject *item = NULL;
    // Fill the list with items
    int success = 1;
    for (int index=0; index<vec.size(); index++) {
        item = PyLong_FromDouble(vec[index]);
        success = PyList_SetItem(pList, index, item);
        if (success != 0) {
            perror("ERROR: vector_to_list: Couldn't insert item");
        }
    }
    if (success != 0) {
        Py_DECREF(pList);
    }
    return pList;
}
