#include "Graph.h"

BOOST_PYTHON_MODULE(cpp_easygraph)
{
    boost::python::class_<Graph>("Graph", py::no_init)
        .def("__init__", py::raw_function(&__init__))
        .def(py::init<>())
        .def("__iter__", &__iter__)
        .def("__len__", &__len__)
        .def("__contains__", &__contains__, (py::arg("node")))
        .def("__getitem__", &__getitem__, (py::arg("node")))
        .def("add_node", py::raw_function(&add_node))
        .def("add_nodes", &add_nodes, (py::arg("nodes_for_adding"), py::arg("nodes_attr") = py::list()))
        .def("add_nodes_from", py::raw_function(&add_nodes_from))
        .def("add_edge", py::raw_function(&add_edge))
        .def("add_edges", &add_edges, (py::arg("edges_for_adding"), py::arg("edges_attr") = py::list()))
        .def("add_edges_from", py::raw_function(&add_edges_from))
        .def("add_edges_from_file", &add_edges_from_file, (py::arg("file"), py::arg("weighted") = false))
        .def("add_weighted_edge", &add_weighted_edge, (py::arg("u_of_edge"), py::arg("v_of_edge"), py::arg("weight")))
        .def("copy", &copy)
        .def("degree", &degree, (py::arg("weight") = py::str("weight")))
        .def("neighbors", &neighbors, (py::arg("node")))
        .def("all_neighbors", &neighbors, (py::arg("node")))
        .def("nodes_subgraph", &nodes_subgraph, (py::arg("from_nodes")))
        .def("ego_subgraph", &ego_subgraph, (py::arg("center")));
}