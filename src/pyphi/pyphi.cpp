// pyphi.cpp

#include "./pyphi.hpp"

using namespace std;


PyPhi::~PyPhi() {
    Py_Finalize();
}

PyPhi::PyPhi() {
    cout << "Initializing PyPhi...\n" << endl;

    // Initialize the Python interpreter
    Py_Initialize();
    if (!Py_IsInitialized()) {
        printf("ERROR: Unable to initialize Python interpreter.\n");
    } else {
        printf("Python interpreter initialized.\n");
    }
    wcout << "Prefix: " << Py_GetPrefix() << Py_GetExecPrefix() << Py_GetProgramFullPath() << endl;
    wcout << "Module Path:" << Py_GetPath() << endl;
    wcout << "Version:" << Py_GetVersion() << "Platform:" << Py_GetPlatform() << endl;
    wcout << "Compiler:" << Py_GetCompiler() << "Build Info:" << Py_GetBuildInfo() << endl;

    // Add the virtualenv to the Python path
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.insert(0, " + VIRTUALENV_PATH + ")");

    // Import PyPhi
    pyphi = PyImport_ImportModule("pyphi");
    if (pyphi == NULL)
    {
        PyErr_Print();
        cout << "ERROR: Can't import PyPhi." << endl;
    }

    // Get references to the PyPhi functions we want to expose

    network_constructor = PyObject_GetAttrString(pyphi_m, "Network");
    if (network_constructor == NULL) {
        PyErr_Print();
        Py_DECREF(network_constructor);
        cout << "ERROR: Can't get reference to pyphi.Network class." << endl;
    }

    subsystem_constructor = PyObject_GetAttrString(pyphi_m, "Subsystem");
    if (subsystem_constructor == NULL) {
        PyErr_Print();
        Py_DECREF(subsystem_constructor);
        cout << "ERROR: Can't get reference to pyphi.Subsystem class." << endl;
    }

    compute = PyImport_ImportModule("pyphi.compute");
    if (compute == NULL) {
        cout << "ERROR: Can't import the pyphi.compute module." << endl;
    }

    py_big_phi = PyObject_GetAttrString(compute, "big_phi");
    Py_DECREF(compute);
    if (py_big_phi == NULL) {
        cout << "ERROR: Can't get reference to pyphi.compute.big_phi." << endl;
        PyErr_Print();
        Py_DECREF(py_big_phi);
    }

    py_conceptual_information = PyObject_GetAttrString(compute, "conceptual_information");
    Py_DECREF(py_compute);
    if (py_conceptual_information == NULL) {
        cout << "ERROR: Can't get reference to the pyphi.compute.conceptual_information function." << endl;
        PyErr_Print();
        Py_DECREF(py_conceptual_information);
    }
}

PyObject* PyPhi::network(vector< vector<double> >& c_tpm, vector<long>&
        current_state, vector<long>& past_state, vector< vector<double> >&
        c_cm) {
    PyObject *network;
    PyObject* py_tpm = convert::vec_of_vecs_to_list_of_lists(c_tpm);
    PyObject* py_cm = convert::vec_of_vecs_to_list_of_lists(c_cm);
    // TODO(wmayner) parameterize number of nodes
    PyObject *network_args = Py_BuildValue("O (i,i,i,i,i,i,i,i) (i,i,i,i,i,i,i,i) O", py_tpm,
        current_st[0], current_st[1], current_st[2],current_st[3], current_st[4], current_st[5],current_st[6], current_st[7],
        past_state[0], past_state[1], past_state[2],past_state[3], past_state[4], past_state[5],past_state[6], past_state[7],
        py_cm);
    if (network_args == NULL) {
        PyErr_Print();
        Py_DECREF(network_args);
        cout << "ERROR: Can't create pyphi.Network arguments." << endl;
    }
    network = PyObject_CallObject(network_constructor, network_args);
    Py_DECREF(network_args);
    Py_DECREF(py_tpm);
    Py_DECREF(py_cm);
    if (network == NULL) {
        PyErr_Print();
        Py_DECREF(network);
        cout << "ERROR: Can't create pyphi.Network instance." << endl;
    }
    return network;
}

PyObject* PyPhi::subsystem(vector<long>& node_indices, PyObject* network) {
    PyObject *subsystem;

    PyObject* py_node_indices = convert::vector_to_list_long(node_indices);
    PyObject* subsystem_args = Py_BuildValue("O O", py_node_indices, network);
    if (subsystem_args == NULL) {
        PyErr_Print();
        Py_DECREF(subsystem_args);
        cout << "ERROR: Can't build pyphi.Subsystem arguments." <<endl;
    }
    subsystem = PyObject_CallObject(subsystem_constructor, subsystem_args);
    Py_DECREF(py_node_indices);
    Py_DECREF(subsystem_args);
    if (subsystem == NULL) {
        PyErr_Print();
        Py_DECREF(subsystem);
        cout << "ERROR: Can't create pyphi.Subsystem instance." << endl;
    }
    return subsystem;
}

double PyPhi::big_phi(PyObject* subsystem) {
    PyObject *py_result = PyObject_CallFunction(py_big_phi, "O", subsystem);
    if (py_result == NULL) {
        cout << "ERROR: big_phi returned NULL." << endl;
        PyErr_Print();
        Py_DECREF(py_result);
    }
    double c_result = PyFloat_AsDouble(py_result);
    Py_DECREF(py_result);
    return c_result;
}

double PyPhi::conceptual_information(PyObject* subsystem) {
    PyObject *py_result = PyObject_CallFunction(py_conceptual_information, "O",
            subsystem);
    if (py_result == NULL) {
        cout << "ERROR: conceptual_information returned NULL." << endl;
        PyErr_Print();
        Py_DECREF(py_result);
    }
    double c_result = PyFloat_AsDouble(py_result);
    Py_DECREF(py_result);
    return c_result;
}
