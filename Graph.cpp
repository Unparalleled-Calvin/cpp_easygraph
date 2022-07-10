#include "Graph.h"

py::object __init__(py::tuple args, py::dict kwargs) {
	py::object fake_self = args[0];
	py::object self = fake_self.attr("__class__")();
	Graph& self_ = py::extract<Graph&>(self);
	py::dict graph_attr = kwargs;
	self_.graph.update(graph_attr);
	return py::object();
}

py::object __iter__(py::object self) {
	return self.attr("node").attr("__iter__");
}

py::object __len__(py::object self) {
	Graph& self_ = py::extract<Graph&>(self);
	return py::object(py::len(self_.node_to_id));
}

py::object __contains__(py::object self, py::object node) {
	Graph& self_ = py::extract<Graph&>(self);
	return self_.node_to_id.contains(node);
}

py::object __getitem__(py::object self, py::object node) {
	return self.attr("adj")[node];
}

int _add_one_node(Graph& self, py::object one_node_for_adding, py::dict node_attr = py::dict()) {
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
	self.node[id] = Graph::node_attr_dict_factory();
	self.adj[id] = Graph::edge_attr_dict_factory();
	for (int i = 0; i<len(items);i++) {
		py::tuple kv = py::extract<py::tuple>(items[i]);
		std::string key = py::extract<std::string>(kv[0]);
		float value = py::extract<float>(kv[1]);
		self.node[id].insert(std::make_pair(key, value));
	}
	return id;
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

py::object add_nodes_from(py::tuple args, py::dict kwargs) {
	Graph& self = py::extract<Graph&>(args[0]);
	py::tuple nodes_for_adding = py::tuple(args.slice(1, py::slice_nil()));
	for (int i = 0;i < py::len(nodes_for_adding);i++) {
		bool newnode;
		py::dict attr = kwargs;
		py::dict newdict, ndict;
		py::object n = nodes_for_adding[i];
		try {
			newnode = !self.node_to_id.contains(n);
			newdict = attr;
		}
		catch (const py::error_already_set&) {
			PyObject* type, * value, * traceback;
			PyErr_Fetch(&type, &value, &traceback);
			if (PyErr_GivenExceptionMatches(PyExc_TypeError, type) ){
				py::tuple n_pair = py::extract<py::tuple>(n);
				n = n_pair[0];
				ndict = py::extract<py::dict>(n_pair[1]);
				newnode = !self.node_to_id.contains(n);
				newdict = attr.copy();
				newdict.update(ndict);
			}
			else {
				PyErr_Restore(type, value, traceback);
				return py::object();
			}
		}
		if (newnode) {
			if (n == py::object()) {
				PyErr_Format(PyExc_ValueError, "None cannot be a node");
				return py::object();
			}
			_add_one_node(self, n);
		}
		int id = py::extract<int>(self.node_to_id(n));
		py::list items = py::list(newdict.items());
		for (int i = 0; i < len(items);i++) {
			py::tuple kv = py::extract<py::tuple>(items[i]);
			std::string key = py::extract<std::string>(kv[0]);
			float value = py::extract<float>(kv[1]);
			self.node[id].insert(std::make_pair(key, value));
		}
	}
	return py::object();
}

void _add_one_edge(Graph& self, py::object u_of_edge,py::object v_of_edge, py::dict edge_attr) {
	int u, v;
	if (self.node_to_id.contains(u_of_edge)) {
		u = _add_one_node(self, u_of_edge);
	}
	else {
		u = py::extract<int>(self.node_to_id[u_of_edge]);
	}
	if (self.node_to_id.contains(v_of_edge)) {
		v = _add_one_node(self, v_of_edge);
	}
	else {
		v = py::extract<int>(self.node_to_id[v_of_edge]);
	}
	py::list items = py::list(edge_attr.items());
	self.adj[u][v] = Graph::node_attr_dict_factory();
	self.adj[v][u] = Graph::node_attr_dict_factory();
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

py::object add_edges_from(py::tuple args, py::dict attr) {
	Graph& self = py::extract<Graph&>(args[0]);
	py::tuple ebunch_to_add = py::tuple(args.slice(1, py::slice_nil()));
	for (int i = 0;i < len(ebunch_to_add);i++) {
		py::list e = py::list(ebunch_to_add[i]);
		py::object u, v;
		py::dict dd;
		switch (len(e)) {
		case 2: {
			u = e[0];
			v = e[1];
			break;
		}
		case 3: {
			u = e[0];
			v = e[1];
			dd = py::extract<py::dict>(e[2]);
			break;
		}
		default: {
			PyErr_Format(PyExc_ValueError, "Edge tuple %R must be a 2 - tuple or 3 - tuple.", e.ptr());
			return py::object();
		}
		}
		int u_id, v_id;
		if (!self.node_to_id.contains(u)) {
			if (u == py::object()) {
				PyErr_Format(PyExc_ValueError, "None cannot be a node");
				return py::object();
			}
			u_id = _add_one_node(self, u);
		}
		if (!self.node_to_id.contains(v)) {
			if (v == py::object()) {
				PyErr_Format(PyExc_ValueError, "None cannot be a node");
				return py::object();
			}
			v_id = _add_one_node(self, v);
		}
		auto datadict = self.adj[u_id].find(v_id) == self.adj[u_id].end() ? Graph::node_attr_dict_factory() : self.adj[u_id][v_id];
		py::list items = py::list(attr);
		items.extend(py::list(dd));
		for (int i = 0;i < py::len(items);i++) {
			py::tuple item = py::extract<py::tuple>(items[i]);
			std::string key = py::extract<std::string>(item[0]);
			float value = py::extract<float>(item[1]);
			datadict.insert(std::make_pair(key, value));
		}
		//Warning: in Graph.py the edge attr is directed assigned by the dict extended from the original attr 
		self.adj[u_id][v_id].insert(datadict.begin(), datadict.end());
		self.adj[v_id][u_id].insert(datadict.begin(), datadict.end());
	}
	return py::object();
}

py::object add_edges_from_file(Graph& self, py::str file, py::object weighted) {
	struct commactype : std::ctype<char> {
		commactype() : std::ctype<char>(get_table()) {}
		std::ctype_base::mask const* get_table() {
			std::ctype_base::mask* rc = 0;
			if (rc == 0) {
				rc = new std::ctype_base::mask[std::ctype<char>::table_size];
				std::fill_n(rc, std::ctype<char>::table_size, std::ctype_base::mask());
				rc[','] = std::ctype_base::space;
				rc[' '] = std::ctype_base::space;
				rc['\t'] = std::ctype_base::space;
				rc['\n'] = std::ctype_base::space;
				rc['\r'] = std::ctype_base::space;
			}
			return rc;
		}
	};

	std::ios::sync_with_stdio(0);
	std::string file_path = py::extract<std::string>(file);
	std::ifstream in;
	in.open(file_path);
	if (!in.is_open()) {
		PyErr_Format(PyExc_FileNotFoundError, "Please check the file and make sure the path only contains English");
		return py::object();
	}
	in.imbue(std::locale(std::locale(), new commactype));
	std::string data, key("weight");
	std::string su, sv;
	float weight;
	while (in >> su >> sv) {
		py::object pu(su), pv(sv);
		int u, v;
		if (!self.node_to_id.contains(pu)) {
			u = _add_one_node(self, pu);
		}
		else {
			u = py::extract<int>(self.node_to_id[pu]);
		}
		if (!self.node_to_id.contains(pv)) {
			v = _add_one_node(self, pv);
		}
		else {
			v = py::extract<int>(self.node_to_id[pv]);
		}
		if (weighted) {
			in >> weight;
			self.adj[u][v][key] = self.adj[v][u][key] = weight;
		}
		else {
			if (self.adj[u].find(v) == self.adj[u].end()) {
				self.adj[u][v] = Graph::node_attr_dict_factory();
			}
			if (self.adj[v].find(u) == self.adj[v].end()) {
				self.adj[v][u] = Graph::node_attr_dict_factory();
			}
		}
	}
	in.close();
	return py::object();
}

py::object add_weighted_edge(Graph& self, py::object u_of_edge, py::object v_of_edge, float weight) {
	py::dict edge_attr;
	edge_attr["weight"] = weight;
	_add_one_edge(self, u_of_edge, v_of_edge, edge_attr);
	return py::object();
}

py::object copy(py::object self) {
	Graph& self_ = py::extract<Graph&>(self);
	py::object G = self.attr("__class__")();
	Graph &G_ = py::extract<Graph&>(G);
	G_.graph.update(self_.graph);
	G_.id_to_node.update(self_.id_to_node);
	G_.node_to_id.update(self_.node_to_id);
	G_.node = self_.node;
	G_.adj = self_.adj;
	return py::object(G);
}

py::dict degree(py::object self, py::str weight) {
	py::dict degree;
	std::string weight_key = py::extract<std::string>(weight);
	py::list edges = py::extract<py::list>(self.attr("edges"));
	py::object u, v;
	py::dict d;
	for (int i = 0;i < py::len(edges);i++) {
		py::tuple edge = py::extract<py::tuple>(edges[i]);
		u = edge[0];
		v = edge[1];
		d = py::extract<py::dict>(edge[2]);
		if (degree.contains(u)) {
			degree[u] += d.attr("get")(weight, 1);
		}
		else {
			degree[u] = d.attr("get")(weight, 1);
		}
		if (degree.contains(v)) {
			degree[v] += d.attr("get")(weight, 1);
		}
		else {
			degree[v] = d.attr("get")(weight, 1);
		}
	}
	return degree;
}

py::object neighbors(py::object self, py::object node) {
	Graph& self_ = py::extract<Graph&>(self);
	if (!self_.node_to_id.contains(node)) {
		return self.attr("adj")[node].attr("__iter__")();
	}
	else {
		PyErr_Format(PyExc_KeyError, "No node %R", node.ptr());
		return py::object();
	}
}

py::object nodes_subgraph(py::object self, py::list from_nodes) {
	py::object G = self.attr("__class__")();
	Graph& self_ = py::extract<Graph&>(self);
	Graph& G_ = py::extract<Graph&>(G);
	G_.graph.update(self_.graph);
	py::dict nodes = py::extract<py::dict>(self.attr("nodes"));
	py::dict adj = py::extract<py::dict>(self.attr("adj"));
	for (int i = 0;i < py::len(from_nodes);i++) {
		py::object node = from_nodes[i];
		if (self_.node_to_id.contains(node)) {
			py::dict node_attr = py::extract<py::dict>(nodes[node]);
			_add_one_node(G_, node, node_attr);
		}
		py::dict out_edges = py::extract<py::dict>(adj[node]);
		py::list edge_items = py::extract<py::list>(out_edges.items());
		for (int j = 0;j < py::len(edge_items);i++) {
			py::tuple item = py::extract<py::tuple>(edge_items[j]);
			py::object v = item[0];
			py::dict edge_attr = py::extract<py::dict>(item[1]);
			if (from_nodes.contains(v)) {
				_add_one_edge(G_, node, v, edge_attr);
			}
		}
	}
	return G;
}

py::object ego_subgraph(py::object self, py::object center) {
	py::list neighbors_of_center = py::list(self.attr("all_neighbors")(center));
	neighbors_of_center.append(center);
	return self.attr("nodes_subgraph")(neighbors_of_center);
}