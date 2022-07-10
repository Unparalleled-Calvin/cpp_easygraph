#include "Utils.h"

py::object attr_to_dict(const Graph::node_attr_dict_factory& attr) {
	py::dict attr_dict = py::dict();
	for (const auto& kv : attr) {
		attr_dict[kv.first] = kv.second;
	}
	return attr_dict;
}
