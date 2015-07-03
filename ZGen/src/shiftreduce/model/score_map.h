#ifndef __ZGEN_SHIFTREDUCE_MODEL_SCORE_MAP_H__
#define __ZGEN_SHIFTREDUCE_MODEL_SCORE_MAP_H__

#include "utils/logging.h"
#include "shiftreduce/types/action.h"
#include "shiftreduce/model/score_context.h"
#include "ml/perceptron/param.h"
#include <boost/functional/hash.hpp>
#include <boost/serialization/unordered_map.hpp>

namespace ZGen {
namespace ShiftReduce {

template<class MetaFeatureType>
class ScoreMap {
private:
  typedef MachineLearning::Perceptron::Parameter param_t;
  typedef std::unordered_map< Action, param_t, boost::hash<Action> > entry_t;
  typedef std::unordered_map< MetaFeatureType, entry_t, boost::hash<MetaFeatureType> > map_t;
  typedef std::vector<MetaFeatureType> cache_t;
  typedef std::function<void(const ScoreContext&, const Action&, cache_t&)> extractor_t;
  //typedef std::function<void(const ScoreContext&, const Action&)> extractor_t;
public:
  ScoreMap(extractor_t _extractor): extractor(_extractor) {
    cache.reserve(128);
  }

  /**
   * Get the score for the (context, action) pair
   *
   *  @param[in]  ctx   The scoring context.
   *  @param[in]  act   The action.
   *  @param[in]  avg   If avg is true return the averaged parameter, else
   *                    return the non-averaged parameter.
   *  @param[in]  default_return_value  The default return value.
   *  @return     floatval_t  The score.
   */
  floatval_t score(const ScoreContext& ctx, const Action& act, bool avg) {
    // cache_t cache;
    cache.clear();
    extractor(ctx, act, cache);
    floatval_t ret = 0.;
    for (const MetaFeatureType& c: cache) {
      typename map_t::const_iterator result1 = rep.find(c);
      if (result1 == rep.end()) { continue; }

      const entry_t& entry = result1->second;
      typename entry_t::const_iterator result2 = entry.find(act);
      if (result2 == entry.end()) { continue; }
      ret += result2->second.dot(avg);
    }
    return ret;
  }

  /**
   * Update the parameter for the (context, action) pair
   *
   *  @param[in]  ctx   The scoring context.
   *  @param[in]  act   The action
   *  @param[in]  now   The current timestamp.
   *  @param[in]  scale The updated scale.
   */
  void update(const ScoreContext& ctx, const Action& act,
      int now, const floatval_t& scale = 1.) {
    // cache_t cache;
    cache.clear();
    extractor(ctx, act, cache);
    for (const MetaFeatureType& c: cache) {
      typename map_t::iterator result1 = rep.find(c);
      if (result1 != rep.end()) {
        entry_t& entry = result1->second;
        typename entry_t::iterator result2 = entry.find(act);
        if (result2 != entry.end()) {
          param_t& param = result2->second;
          param.add(now, scale);
        } else {
          _TRACE << c << " found, but " << act << " not found.";
          entry[act] = param_t(scale, scale, now);
        }
      } else {
        rep[c][act] = param_t(scale, scale, now);
        _TRACE << c << " not found.";
      }

      if (rep.find(c) == rep.end()) {
        _TRACE << c << " still not found!";
      }
    }
  }

  /**
   *
   *
   *
   */
  void flush(int now) {
    for (typename map_t::iterator result1 = rep.begin();
        result1 != rep.end(); ++ result1) {
      entry_t& entry = result1->second;
      for (typename entry_t::iterator result2 = entry.begin();
          result2 != entry.end(); ++ result2) {
        param_t& param = result2->second;
        param.flush(now);
      }
    }
  }

  /**
   * Saving the score map to the archive.
   *
   *
   *
   */
  void save(boost::archive::text_oarchive& oa) {
    oa << rep;
  }

  /**
   * Loading the score map from the archive.
   *
   *  @param[in] ia   The input archive.
   */
  void load(boost::archive::text_iarchive& ia) {
    ia >> rep;
  }

private:
  map_t rep;
  cache_t cache;
  extractor_t extractor;
};

}
}

#endif  //  end for __ZGEN_SHIFTREDUCE_MODEL_SCORE_MAP_H__
