#include "Evaluation.h"

struct pair_hash
{
	template<class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2>& p) const
	{
		auto h1 = std::hash<T1>{}(p.first);
		auto h2 = std::hash<T2>{}(p.second);
		return h1 ^ h2;
	}
};

std::unordered_map<std::pair<Graph::node_t, Graph::node_t>, Graph::weight_t, pair_hash> sum_nmw_rec, max_nmw_rec, local_constraint_rec;

enum norm_t {
	sum, max
};


Graph::weight_t mutual_weight(Graph::adj_dict_factory& G, Graph::node_t u, Graph::node_t v, std::string weight) {
	Graph::weight_t a_uv = 0, a_vu = 0;
	if (G.count(u) && G[u].count(v)) {
		Graph::edge_attr_dict_factory& guv = G[u][v];
		a_uv = guv.count(weight) ? guv[weight] : 1;
	}
	if (G.count(v) && G[v].count(u)) {
		Graph::edge_attr_dict_factory& gvu = G[v][u];
		a_uv = gvu.count(weight) ? gvu[weight] : 1;
	}
	return a_uv + a_vu;
}

Graph::weight_t  normalized_mutual_weight(Graph::adj_dict_factory& G, Graph::node_t u, Graph::node_t v, std::string weight, norm_t norm = sum) {
	std::pair<Graph::node_t, Graph::node_t> edge = std::make_pair(u, v);
	auto& nmw_rec = (norm == sum) ? sum_nmw_rec : max_nmw_rec;
	if (nmw_rec.count(edge)) {
		return nmw_rec[edge];
	}
	else {
		Graph::weight_t scale = 0;
		for (auto& w : G[u]) {
			Graph::weight_t temp_weight = mutual_weight(G, u, w.first, weight);
			scale = (norm == sum) ? (scale + temp_weight) : std::max(scale, temp_weight);
		}
		Graph::weight_t nmw = scale ? (mutual_weight(G, u, v, weight) / scale) : 0;
		nmw_rec[edge] = nmw;
		return nmw;
	}
}

Graph::weight_t local_constraint(Graph::adj_dict_factory& G, Graph::node_t u, Graph::node_t v, std::string weight) {
	std::pair<Graph::node_t, Graph::node_t> edge = std::make_pair(u, v);
	if (local_constraint_rec.count(edge)) {
		return local_constraint_rec[edge];
	}
	else {
		Graph::weight_t direct = normalized_mutual_weight(G, u, v, weight);
		Graph::weight_t indirect = 0;
		for (auto& w : G[u]) {
			indirect += normalized_mutual_weight(G, u, w.first, weight) * normalized_mutual_weight(G, w.first, u, weight);
		}
		Graph::weight_t result = pow((direct + indirect), 2);
		local_constraint_rec[edge] = result;
		return result;
	}
}

std::pair<Graph::node_t, Graph::weight_t> compute_constraint_of_v(Graph::adj_dict_factory& G, Graph::node_t v, std::string weight) {
	Graph::weight_t constraint_of_v = 0;
	if (G[v].size() == 0) {
		constraint_of_v = Py_NAN;
	}
	else {
		for (const auto& n : G[v]) {
			constraint_of_v += local_constraint(G, v, n.first, weight);
		}
	}
	return std::make_pair(v, constraint_of_v);
}

py::object constraint(py::object self, py::object G, py::object nodes, py::object weight, py::object n_workers) {
	py::object warnings = py::import("warnings");
	if (weight.attr("__class__") != py::str().attr("__class__")) {
		warnings.attr("warn")(py::str(weight) + py::str(" would be transformed into an instance of str."));
		weight = py::str(weight);
	}
	std::string weight_key = py::extract<std::string>(weight);
	sum_nmw_rec.clear();
	max_nmw_rec.clear();
	local_constraint_rec.clear();
	if (nodes == py::object()) {
		nodes = G.attr("nodes");
	}
	py::list nodes_list = py::list(nodes);
	py::list constraint_results = py::list();
	Graph& G_ = py::extract<Graph&>(G);
	for (int i = 0;i < py::len(nodes_list);i++) {
		py::object v = nodes_list[i];
		Graph::node_t v_id = py::extract<Graph::node_t>(G_.node_to_id[v]);
		std::pair<Graph::node_t, Graph::weight_t> constraint_pair = compute_constraint_of_v(G_.adj, v_id, weight_key);
		py::tuple constraint_of_v = py::make_tuple(G_.id_to_node[constraint_pair.first], constraint_pair.second);
		constraint_results.append(constraint_of_v);
	}
	py::dict constraint = py::dict(constraint_results);
	return constraint;
}