#ifndef __ZGEN_BESTFIRST_MODEL_SCORE_MAP_H__
#define __ZGEN_BESTFIRST_MODEL_SCORE_MAP_H__

#include <vector>
#include <boost/unordered_map.hpp>
#include "utils/serialization/unordered_map.h"
#include "bestfirst/model/score.h"
#include "bestfirst/model/score_context.h"
#include "ml/perceptron/param.h"

namespace ZGen {

namespace BestFirst {

template<class ScoreType>
class ScoreMap {
private:
  typedef MachineLearning::Perceptron::Parameter param_t;
  typedef boost::unordered_map<ScoreType, param_t> map_t;
  typedef std::function<void(const ScoreContext&, std::vector<ScoreType>&)> extractor_t;

public:
  ScoreMap(extractor_t _extractor): extractor(_extractor) {}

  floatval_t score(const ScoreContext& ctx, bool avg,
      floatval_t default_return_value = 0.) const {
    std::vector<ScoreType> features;
    extractor(ctx, features);
    floatval_t retval = 0.;

    for (int i = 0; i < features.size(); ++ i) {
      const ScoreType& entry = features[i];
      typename map_t::const_iterator itx = payload.find(entry);

      if (itx == payload.end()) {
        retval += default_return_value;
      } else {
        if (avg) {
          retval += itx->second.w_sum;
        } else {
          retval += itx->second.w;
        }
      }
    }

    return retval;
  }

  void update(const ScoreContext& ctx, int now, floatval_t scale = 1.) {
    std::vector<ScoreType> features;
    extractor(ctx, features);

    for (int i = 0; i < features.size(); ++ i) {
      const ScoreType& entry = features[i];
      param_t& param = payload[entry];
      param.add(now, scale);
    }
  }

  void flush(int now) {
    for (typename map_t::iterator itx = payload.begin();
        itx != payload.end(); ++ itx) {
      itx->second.flush(now);
    }
  }

  void save(boost::archive::text_oarchive& oa) {
    oa << payload;
  }

  void load(boost::archive::text_iarchive& ia) {
    ia >> payload;
  }

private:
  map_t payload;
  extractor_t extractor;
};

} //  end for namespace BestFirst
} //  end for namespace ZGen


#endif  //  end for __ZGEN_BESTFIRST_MODEL_SCORE_MAP_H__
