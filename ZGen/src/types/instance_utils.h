#ifndef __ZGEN_TYPES_INSTANCE_UTILS_H__
#define __ZGEN_TYPES_INSTANCE_UTILS_H__

#include "types/instance.h"

namespace ZGen {

class InstanceUtils {
public:
  static void shuffle_instance(const dependency_t& instance,
      dependency_t& shuffled_instance,
      std::vector<int>& order);
};

}

#endif  //  end for __ZGEN_TYPES_INSTANCE_UTILS_H__
