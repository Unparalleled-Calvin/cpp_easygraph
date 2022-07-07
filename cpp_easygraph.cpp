#include "Graph.h"

BOOST_PYTHON_MODULE(cpp_easygraph)
{
    boost::python::class_<Graph>("Graph")
        .def("add_node", py::raw_function(&add_node))
        .def("add_nodes", &add_nodes, (py::arg("nodes_for_adding"), py::arg("nodes_attr") = py::list()))
        .def("add_edge", py::raw_function(&add_edge))
        .def("add_edges", &add_edges, (py::arg("edges_for_adding"), py::arg("edges_attr") = py::list()));
}