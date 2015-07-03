#ifndef __ZGEN_ML_PERCEPTRON_PARAM_H__
#define __ZGEN_ML_PERCEPTRON_PARAM_H__

#include "settings.h"

namespace ZGen {

namespace MachineLearning {

namespace Perceptron {

//! It's a parameter class that support recording the updated time
//! and averaged parameter.
struct Parameter {
  Parameter(): w(0), w_sum(0), w_time(0) {}

  Parameter(floatval_t _w, floatval_t _w_sum, floatval_t _w_time)
    : w(_w), w_sum(_w_sum), w_time(_w_time) {}

  floatval_t w;
  floatval_t w_sum;
  int w_time;

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & w & w_sum;
  }

  floatval_t dot(bool avg) const {
    return (avg? w_sum: w);
  }

  void add(int now, floatval_t scale) {
    int elapsed = now - w_time;
    floatval_t cur_val = w;
    w = cur_val + scale;
    w_sum += elapsed * cur_val + scale;
    w_time = now;
  }

  void flush(int now) {
    w_sum += (now - w_time) * w;
    w_time = now;
  }

  friend std::ostream& operator << (std::ostream& os, const Parameter& p) {
    os << "(" << p.w << ", " << p.w_sum << ", " << p.w_time << ")";
    return os;
  }
};

} //  end for namespace Perceptron

} //  end for namespace Machinelearning

} //  end for namespace ZGen

#endif  //  end for __ZGEN_ML_PERCEPTRON_PARAM_H__
