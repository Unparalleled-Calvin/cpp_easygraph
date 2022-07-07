#pragma once
#define BOOST_PYTHON_STATIC_LIB

#include "Common.h"

struct Graph
{
	std::unordered_map<int, std::map<std::string, float>>node;
	std::unordered_map<int, std::unordered_map<int, std::map<std::string, float>>> adj;
	py::dict node_to_id, id_to_node, graph;
	int id;
};

py::object add_node(py::tuple args, py::dict kwargs);
py::object add_nodes(Graph& self_, py::list nodes_for_adding, py::list nodes_attr);
py::object add_edge(py::tuple args, py::dict kwargs);
py::object add_edges(Graph& self, py::list edges_for_adding, py::list edges_attr);