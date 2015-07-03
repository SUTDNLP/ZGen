#include "utils/math_utils.h"

namespace ZGen {
namespace Utility {

int bin(int x) {
  int sign = (x < 0 ? -1 : 1);

  x = x * sign;
  if (x > 40) {
    return sign * 40;
  } else if (x > 20) {
    return sign * 20;
  } else if (x > 10) {
    return sign * 10;
  } else if (x > 5) {
    return sign * 5;
  }
  return x * sign;
}

} //  end for namespace Utility
} //  end for namespace ZGen
