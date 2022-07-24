#include "Cluster.h"
#include "Utils.h"

inline Graph::weight_t wt(Graph::adj_dict_factory& adj, Graph::node_t u, Graph::node_t v, std::string weight) {
	auto& attr = adj[u][v];
	return attr.count(weight) ? attr[weight] : 1;
}

std::set<Graph::node_t> operator &(const std::set<Graph::node_t>& set1, const std::set<Graph::node_t>& set2) {
	std::set<Graph::node_t> intersection;
	std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersection, intersection.begin()));
	return intersection;
}

py::list _weighted_triangles_and_degree(py::object G, py::object nodes, py::object weight = py::str("weight")) {
	std::string weight_key = weight_to_string(weight);
	Graph& G_ = py::extract<Graph&>(G);
	Graph::weight_t max_weight;
	if (weight == py::object() || G.attr("number_of_edges")() == 0) {
		max_weight = 1;
	}
	else {
		int assigned = 0;
		for (auto& u_info : G_.adj) {
			for (auto& v_info : u_info.second) {
				auto& d = v_info.second;
				if (assigned) {
					max_weight = std::max(max_weight, d.count(weight_key) ? d[weight_key] : 1);
				}
				else {
					assigned = 1;
					max_weight = d.count(weight_key) ? d[weight_key] : 1;
				}
			}
		}
	}
	py::list nodes_list = py::list(nodes == py::object() ? G.attr("nodes") : G.attr("nbunch_iter")());
	py::list ret = py::list();
	for (int i = 0;i < py::len(nodes_list);i++) {
		Graph::node_t i_id = py::extract<Graph::node_t>(G_.node_to_id[nodes_list[i]]);
		std::set<Graph::node_t> inbrs, seen;
		auto& adj = G_.adj;
		for (const auto& pair : adj[i_id]) {
			inbrs.insert(pair.first);
		}
		inbrs.erase(i_id);
		Graph::weight_t weighted_triangles = 0;
		for (const auto& j_id : inbrs) {
			seen.insert(j_id);
			std::set<Graph::node_t> jnbrs;
			for (const auto& pair : adj[j_id]) {
				jnbrs.insert(pair.first);
			}
			jnbrs.erase(seen.begin(), seen.end());
			Graph::weight_t weighted_triangles = 0;
			Graph::weight_t wij = wt(adj, i_id, j_id, weight_key);
			for (const auto& k_id : inbrs & jnbrs) {
				Graph::weight_t wjk = wt(adj, j_id, k_id, weight_key);
				Graph::weight_t wki = wt(adj, k_id, i_id, weight_key);
				weighted_triangles += std::cbrt(wij * wjk * wki);
			}
		}
		ret.append(py::make_tuple(G_.id_to_node[i_id], inbrs.size(), 2 * weighted_triangles));
	}
	return ret;
}

py::list _triangles_and_degree(py::object G, py::object nodes = py::object()) {
	Graph& G_ = py::extract<Graph&>(G);
	py::list nodes_list = py::list(nodes == py::object() ? G.attr("nodes") : G.attr("nbunch_iter")());
	py::list ret = py::list();
	for (int i = 0;i < py::len(nodes_list);i++) {
		Graph::node_t v = py::extract<Graph::node_t>(G_.node_to_id[nodes_list[i]]);
		std::set<Graph::node_t> vs;
		for (const auto& pair : G_.adj[v]) {
			vs.insert(pair.first);
		}
		vs.erase(v);
		py::dict gen_degree = py::dict();
		Graph::weight_t ntriangles = 0;
		for (const auto& w : vs) {
			std::set<Graph::node_t> wnbrs;
			for (const auto& pair : G_.adj[w]) {
				wnbrs.insert(pair.first);
			}
			wnbrs.erase(w);
			ntriangles += (vs & wnbrs).size();
		}
		ret.append(py::make_tuple(nodes_list[i], vs.size(), ntriangles));
	}
	return ret;
}

py::object clustering(py::object G, py::object nodes, py::object weight) {
	py::dict clusterc = py::dict();
	if (G.attr("is_directed")()) {
		PyErr_Format(PyExc_RuntimeError, "Not implemented yet");
		return py::object();
	}
	else {
		py::list td_list = weight != py::object() ? _weighted_triangles_and_degree(G, nodes, weight) : _triangles_and_degree(G, nodes);
		for (int i = 0;i < py::len(td_list);i++) {
			py::tuple tuple = py::extract<py::tuple>(td_list[i]);
			py::object v = tuple[0], d = tuple[1], t = tuple[2];
			clusterc[v] = t == 0 ? 0 : t / (d * (d - 1));
		}
	}
	if (G.contains(nodes)) {
		return clusterc[nodes];
	}
	return clusterc;
}