// convert.hpp

#ifndef SRC_PYPHI_CONVERT_H_
#define SRC_PYPHI_CONVERT_H_

#include <stdio.h>
#include <stdlib.h>
#include <Python.h>

#include <iostream>
#include <vector>

PyObject* vec_of_vecs_to_list_of_lists(std::vector< std::vector<double> >& matrix);
PyObject* prob_vec_of_vecs_to_list_of_lists(std::vector< std::vector<double> >& matrix);
PyObject* vector_to_list(std::vector<double>& vec);
PyObject* vector_to_list_long(std::vector<long>& vec);

#endif  // SRC_PYPHI_CONVERT_H_
