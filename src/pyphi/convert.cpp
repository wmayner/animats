// convert.cpp
//
// Utility methods for C++ functions that wrap Python modules.

#include "./convert.hpp"


PyObject* vec_of_vecs_to_list_of_lists(std::vector<std::vector<double> >& matrix)
{
    long outer_len = matrix.size();
    // Create the new empty list
    PyObject *py_outer_list = PyList_New(outer_len);
    // Fill the List
    std::vector<double> inner_vector;
    PyObject *py_inner_list = NULL;
    int success = -1;
    for (int i = 0; i < outer_len; i++) {
        inner_vector = matrix[i];
        py_inner_list = PyInterface::vector_to_list(inner_vector);
        if (py_inner_list == NULL) {
            PyErr_Print();
            std::cout << "ERROR: Could't create inner list." << std::endl;
            Py_DECREF(py_inner_list);
        }
        success = PyList_SetItem(py_outer_list, i, py_inner_list);
        if (success != 0) {
            perror("ERROR: vector_to_list: Couldn't insert item");
        }
    }
    inner_vector.clear();
    return py_outer_list;
}

PyObject* vector_to_list(std::vector<double>& vec) {
    // Create the new list object
    PyObject *py_outer_list = PyList_New(vec.size());
    PyObject *item = NULL;
    // Fill the list with items
    int success = -1;
    for (int index = 0; index < vec.size(); index++) {
        item = PyFloat_FromDouble(vec[index]);
        if (item == NULL) {
            std::cout << "ERROR: item = PyFloat_FromDouble(vec[index] is NULL." << std::endl;
            PyErr_Print();
            Py_DECREF(item);
        }
        PyList_SetItem(py_outer_list, index, item);
    }
    return py_outer_list;
}

PyObject* vector_to_list_long(std::vector<long>& vec) {
    // Create the new list object
    PyObject *py_outer_list = PyList_New(vec.size());
    PyObject *item = NULL;
    // Fill the list with items
    int success = -1;
    for (int index = 0; index < vec.size(); index++) {
        item = PyLong_FromDouble(vec[index]);
        success = PyList_SetItem(py_outer_list, index, item);
        if (success != 0) {
            perror("ERROR: vector_to_list: Couldn't insert item");
        }
    }
    if (success != 0) {
        Py_DECREF(py_outer_list);
    }
    return py_outer_list;
}
