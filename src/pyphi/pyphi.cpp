// pyphi.cpp

#include "pyphi.h"

using namespace std;

tPyPhi::~tPyPhi()
{
    Py_Finalize();
}

tPyPhi::tPyPhi()
{
    cout << "INITIALIZING pyphi INSTANCE\n" << endl;
    // Initialize the Python interpreter
    Py_Initialize();
    if( !Py_IsInitialized() ) {
        printf("Unable to initialize Python interpreter.\n");
    }
    wcout << "Prefix: " << Py_GetPrefix() << Py_GetExecPrefix() << Py_GetProgramFullPath()<<endl;
    wcout << "Module Path:" << Py_GetPath()<< endl;
    wcout << "Version:" << Py_GetVersion()<< "Platform" << Py_GetPlatform()<<endl;
    wcout << "Compiler:" << Py_GetCompiler()<< "Build Info" << Py_GetBuildInfo()<<endl;

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path += ['/Users/jagomez/.virtualenvs/matching/lib/python3.4/site-packages']");
    PyRun_SimpleString("print(sys.path)");

    // Modify the path to include the PyPhi installation in the virtualenvs
    // Import the PyPhi Module
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    pyphi_m = PyImport_ImportModule("pyphi");
    if (pyphi_m==NULL)
    {
        PyErr_Print();
        cout << "ERROR: Could not import pyphi." << endl;
    }
    // Set PyPhi options
    // ~~~~~~~~~~~~~~~~~
    // Get references to the PyPhi functions we'll use
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Get reference to Network class.
    network_constructor = PyObject_GetAttrString(pyphi_m, "Network");
    if (network_constructor == NULL) {
        // Print the last Python exception
        PyErr_Print();
        Py_DECREF(network_constructor);
        cout << "ERROR: Couldn 't get reference to pyphi.Network." <<endl;
    }
    // Get reference to Subsystem class.
    subsystem_constructor = PyObject_GetAttrString(pyphi_m, "Subsystem"); //class pyphi.subsystem.Subsystem
    if (subsystem_constructor == NULL) {
        // Print the last Python exception
        PyErr_Print();
        Py_DECREF(subsystem_constructor);
        cout << "ERROR: Couldn't get reference to pyphi.Subsystem." <<endl;
    }
    // Get reference to the big_phi function.
    compute = PyImport_ImportModule("pyphi.compute");
    if (compute == NULL) {
        cout << "ERROR: Could not import pyphi.compute." << endl;
    }
    big_phi = PyObject_GetAttrString(compute, "big_phi");
    Py_DECREF(compute);
    // Check we get the object of the class
    if (big_phi == NULL) {
        cout << "ERROR: Couldn't get reference to pyphi.compute.big_phi." <<endl;
        PyErr_Print();
        Py_DECREF(big_phi);
    }
    conceptual_information = PyObject_GetAttrString(compute, "conceptual_information");
    Py_DECREF(compute);
    // Check we get the object of the class
    if (conceptual_information == NULL) {
        cout << "ERROR: Couldn't get reference to pyphi.compute.conceptual_information." <<endl;
        PyErr_Print();
        Py_DECREF(conceptual_information);
    }
}

PyObject* tPyPhi::pyphi_network(vector<vector<double> >& c_tpm, vector<long>& current_st, vector<long>& past_state, vector<vector<double> >& conn_matrix)
{
    PyObject *network;
    vector<double> row;
    vector<vector<double> > prob_ctpm;
    PyObject* py_tpm = PyInterface::vec_of_vecs_to_list_of_lists(c_tpm);
    PyObject* py_cm = PyInterface::vec_of_vecs_to_list_of_lists(conn_matrix);
    PyObject *network_args = Py_BuildValue("O (i,i,i,i,i,i,i,i) (i,i,i,i,i,i,i,i) O",
        py_tpm,
        current_st[0], current_st[1], current_st[2],current_st[3], current_st[4], current_st[5],current_st[6], current_st[7],
        past_state[0], past_state[1], past_state[2],past_state[3], past_state[4], past_state[5],past_state[6], past_state[7],
        py_cm);
    if (network_args==NULL) {
        // Print the last Python exception
        PyErr_Print();
        Py_DECREF(network_args);
        cout << "ERROR: Cannot create network arguments." <<endl;
    }
    network = PyObject_CallObject(network_constructor, network_args);
    Py_DECREF(network_args);
    Py_DECREF(py_tpm);
    Py_DECREF(py_cm);
    // Check that the creation of the Network succeeded
    if (network==NULL) {
        // Print the last Python exception
        PyErr_Print();
        Py_DECREF(network);
        cout << "ERROR: Cannot create network instance." <<endl;
    }
    row.clear();
    //row.shrink_to_fit();
    prob_ctpm.clear();
    //prob_ctpm.shrink_to_fit();
    return network;
}

PyObject* tPyPhi::pyphi_subsystem(vector<long>& node_indices, PyObject* network )
{
    PyObject *subsystem;

    // node_indices has variable size, optimize this, horrendus
    PyObject* py_node_indices = PyInterface::vector_to_list_long(node_indices);
    PyObject* subsystem_args = Py_BuildValue("O O",
                                   py_node_indices,
                                   network);
    if (subsystem_args==NULL) {
        // Print the last Python exception
        PyErr_Print();
        Py_DECREF(subsystem_args);
        cout << "ERROR: can not build argument list for object instance subsystem." <<endl;
    }
    // Create a new instance of the class subsystem by calling the class with the arguments
    subsystem = PyObject_CallObject(subsystem_constructor, subsystem_args);
    Py_DECREF(py_node_indices);
    Py_DECREF(subsystem_args);
    if (subsystem==NULL) {
        // Print the last Python exception
        PyErr_Print();
        Py_DECREF(subsystem);
        cout << "ERROR: Couldn't create Subsystem object." <<endl;
    }
    // Last, call the method big_phi of the module pyphi.compute, passing as an argument reference
    // to "myobject" which is a reference to a susbsystem object
    return subsystem;
}

double tPyPhi::pyphi_bigphi(PyObject* subsystem) {
    PyObject *big_phi_result = PyObject_CallFunction(big_phi, "O", subsystem);
    if (big_phi_result==NULL) {
        cout << "ERROR: big_phi_result is NULL." << endl;
        PyErr_Print();
        Py_DECREF(big_phi_result);
    }
    double ccbigphi = PyFloat_AsDouble(big_phi_result);
    Py_DECREF(big_phi_result);
    return ccbigphi;
}


double tPyPhi::pyphi_conceptual_information(PyObject* subsystem) {
    PyObject *conc_inf_result = PyObject_CallFunction(conceptual_information, "O", subsystem);
    if (conc_inf_result==NULL) {
        cout << "ERROR: conc_inf_result is NULL." << endl;
        PyErr_Print();
        Py_DECREF(conc_inf_result);
    }
    double conc_in = PyFloat_AsDouble(conc_inf_result);
    Py_DECREF(conc_inf_result);
    return conc_in;
}
