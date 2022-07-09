#include "Graph.h"

BOOST_PYTHON_MODULE(cpp_easygraph)
{
    boost::python::class_<Graph>("Graph")
        .def("add_node", py::raw_function(&add_node))
        .def("add_nodes", &add_nodes, (py::arg("nodes_for_adding"), py::arg("nodes_attr") = py::list()))
        .def("add_nodes_from", py::raw_function(&add_nodes_from))
        .def("add_edge", py::raw_function(&add_edge))
        .def("add_edges", &add_edges, (py::arg("edges_for_adding"), py::arg("edges_attr") = py::list()))
        .def("add_edges_from", py::raw_function(&add_edges_from))
        .def("add_edges_from_file", &add_edges_from_file, (py::arg("file"), py::arg("weighted") = false))
        .def("add_weighted_edge", &add_weighted_edge, (py::arg("u_of_edge"), py::arg("v_of_edge"), py::arg("weight")));
}