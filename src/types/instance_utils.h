#ifndef __ZGEN_TYPES_INSTANCE_UTILS_H__
#define __ZGEN_TYPES_INSTANCE_UTILS_H__

#include "types/instance.h"
#include "types/graph.h"

namespace ZGen {

class InstanceUtils {
public:
  static void shuffle_instance(const dependency_t& instance, dependency_t& shuffled_instance, dependency_t& gold_instance,
      std::vector<int>& order, const graph_t & graph, graph_t & shuffled_graph);
};

}

#endif  //  end for __ZGEN_TYPES_INSTANCE_UTILS_H__
