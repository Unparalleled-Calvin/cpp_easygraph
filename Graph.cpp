#include "Graph.h"

void _add_one_node(Graph& self, py::object one_node_for_adding, py::dict node_attr = py::dict()) {
	int id;
	if (self.node_to_id.contains(one_node_for_adding)) {
		id = py::extract<int>(self.node_to_id[one_node_for_adding]);
	}
	else {
		id = ++(self.id);
		self.id_to_node[id] = one_node_for_adding;
		self.node_to_id[one_node_for_adding] = id;
	}
	py::list items = py::list(node_attr.items());
	self.node[id] = std::map<std::string, float>();
	for (int i = 0; i<len(items);i++) {
		py::tuple kv = py::extract<py::tuple>(items[i]);
		std::string key = py::extract<std::string>(kv[0]);
		float value = py::extract<float>(kv[1]);
		self.node[id].insert(std::make_pair(key, value ));
	}
}

py::object add_node(py::tuple args, py::dict kwargs) {
	Graph& self = py::extract<Graph&>(args[0]);
	py::object one_node_for_adding = args[1];
	py::dict node_attr = kwargs;
	_add_one_node(self, one_node_for_adding, node_attr);
	return py::object();
}

py::object add_nodes(Graph& self, py::list nodes_for_adding, py::list nodes_attr) {
	//Graph& self = py::extract<Graph&>(self_);
	if (py::len(nodes_attr) != 0) {
		if (py::len(nodes_for_adding) != py::len(nodes_attr)) {
			PyErr_Format(PyExc_AssertionError, "Nodes and Attributes lists must have same length.");
			return py::object();
		}
	}
	for (int i = 0;i < py::len(nodes_for_adding);i++) {
		py::object one_node_for_adding = nodes_for_adding[i];
		py::dict node_attr;
		if (py::len(nodes_attr)) {
			node_attr = py::extract<py::dict>(nodes_attr[i]);
		}
		else {
			node_attr = py::dict();
		}
		_add_one_node(self, one_node_for_adding, node_attr);
	}
	return py::object();
}

void _add_one_edge(Graph& self, py::object u_of_edge,py::object v_of_edge, py::dict edge_attr) {
	int u, v;
	if (self.node_to_id.contains(u_of_edge)) {
		_add_one_node(self, u_of_edge);
		u = self.id;
	}
	else {
		u = py::extract<int>(self.node_to_id[u_of_edge]);
	}
	if (self.node_to_id.contains(v_of_edge)) {
		_add_one_node(self, v_of_edge);
		v = self.id;
	}
	else {
		v = py::extract<int>(self.node_to_id[v_of_edge]);
	}
	py::list items = py::list(edge_attr.items());
	self.adj[u][v] = std::map<std::string, float>();
	self.adj[v][u] = std::map<std::string, float>();
	for (int i = 0; i < len(items);i++) {
		py::tuple kv = py::extract<py::tuple>(items[i]);
		std::string key = py::extract<std::string>(kv[0]);
		float value = py::extract<float>(kv[1]);
		self.adj[u][v].insert(std::make_pair(key, value));
		self.adj[v][u].insert(std::make_pair(key, value));
	}
}

py::object add_edge(py::tuple args, py::dict kwargs) {
	Graph& self = py::extract<Graph&>(args[0]);
	py::object u_of_edge = args[1], v_of_edge = args[2];
	py::dict edge_attr = kwargs;
	_add_one_edge(self, u_of_edge, v_of_edge, edge_attr);
	return py::object();
}

py::object add_edges(Graph& self, py::list edges_for_adding, py::list edges_attr) {
	if (py::len(edges_attr) != 0) {
		if (py::len(edges_for_adding) != py::len(edges_attr)) {
			PyErr_Format(PyExc_AssertionError, "Edges and Attributes lists must have same length.");
			return py::object();
		}
	}
	for (int i = 0;i < py::len(edges_for_adding);i++) {
		py::tuple one_edge_for_adding = py::extract<py::tuple>(edges_for_adding[i]);
		py::dict edge_attr;
		if (py::len(edges_attr)) {
			edge_attr = py::extract<py::dict>(edges_attr[i]);
		}
		else {
			edge_attr = py::dict();
		}
		_add_one_edge(self, one_edge_for_adding[0], one_edge_for_adding[1], edge_attr);
	}
	return py::object();
}