#pragma once
#define BOOST_PYTHON_STATIC_LIB

#include "Common.h"

struct Graph
{
	std::unordered_map<int, std::map<std::string, float>>node;
	std::unordered_map<int, std::unordered_map<int, std::map<std::string, float>>> adj;
	py::dict node_to_id, id_to_node, graph;
	int id;

	typedef std::unordered_map<int, std::map<std::string, float> > edge_attr_dict_factory;
	typedef std::map<std::string, float> node_attr_dict_factory;
};

py::object __init__(py::tuple args, py::dict kwargs);
py::object __iter__(py::object self);
py::object __len__(py::object self);
py::object __contains__(py::object self, py::object node);
py::object __getitem__(py::object self, py::object node);
py::object add_node(py::tuple args, py::dict kwargs);
py::object add_nodes(Graph& self, py::list nodes_for_adding, py::list nodes_attr);
py::object add_nodes_from(py::tuple args, py::dict kwargs);
py::object add_edge(py::tuple args, py::dict kwargs);
py::object add_edges(Graph& self, py::list edges_for_adding, py::list edges_attr);
py::object add_edges_from(py::tuple args, py::dict attr);
py::object add_edges_from_file(Graph& self, py::str file, py::object weighted);
py::object add_weighted_edge(Graph& self, py::object u_of_edge, py::object v_of_edge, float weight);
py::object copy(py::object self);
py::dict degree(py::object self, py::str weight);
py::object neighbors(py::object self, py::object node);
py::object nodes_subgraph(py::object self, py::list from_nodes);
py::object ego_subgraph(py::object self, py::object center);