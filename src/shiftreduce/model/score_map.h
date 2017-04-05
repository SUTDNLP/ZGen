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

extern float lambda;

template<class MetaFeatureType>
class ScoreMap {
private:
  typedef MachineLearning::Perceptron::Parameter param_t;
  typedef std::unordered_map< Action, param_t, boost::hash<Action> > entry_t;
  typedef std::unordered_map< MetaFeatureType, entry_t, boost::hash<MetaFeatureType> > map_t;
  typedef std::vector<MetaFeatureType> cache_t;
  typedef std::function<void(const ScoreContext&, const Action&, cache_t&)> extractor_t;
  Action pos_act = action_t(Action::kShift, ZGen::Engine::TokenAlphabet::BEGIN, ZGen::Engine::TokenAlphabet::BEGIN, -1);
public:
  ScoreMap(extractor_t _extractor): extractor(_extractor) {
      cache.reserve(128);
  }

  ScoreMap(extractor_t _extractor, bool _track): extractor(_extractor) {
    cache.reserve(128);
    track = _track;
  }

  floatval_t score(const ScoreContext& ctx, const Action& act, bool avg) {
    cache.clear();
    extractor(ctx, act, cache);
    floatval_t ret = 0.;
    for (const MetaFeatureType& c: cache) {
      typename map_t::const_iterator result1 = rep.find(c);
      if (result1 == rep.end()) { continue; }

      const entry_t& entry = result1->second;
      typename entry_t::const_iterator result2;
      if(act.name() == Action::kShift){
    	  result2 = entry.find(pos_act);
      }else{
    	  result2 = entry.find(act);
      }

      if (result2 == entry.end()) { continue; }
    	  ret += result2->second.dot(avg);
    }
    return ret;
  }

  void update(const ScoreContext& ctx, const Action& act,
       int now, const floatval_t& scale = 1.) {
     // cache_t cache;
     cache.clear();
     extractor(ctx, act, cache);
     for (const MetaFeatureType& c: cache) {
       typename map_t::iterator result1 = rep.find(c);

       if(act.name() == Action::kShift){
       	if (result1 != rep.end()) {
 			entry_t& entry = result1->second;
 			typename entry_t::iterator result2 = entry.find(pos_act);
 			if (result2 != entry.end()) {
 			  param_t& param = result2->second;
 			  param.add(now, scale);
 			} else {
 			  _TRACE << c << " found, but " << pos_act << " not found.";
 			  entry[pos_act] = param_t(scale, scale, now);
 			}
 		 } else {
 			rep[c][pos_act] = param_t(scale, scale, now);
 			_TRACE << c << " not found.";
 		 }
       }else{
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
//<<<<<<< HEAD
    if(track){
    	_INFO<<"feat value";
		for(auto it:rep){
			_INFO<<"it "<<it.first;
			for(auto it2:it.second){
				_INFO<<"action "<<it2.first<<" value "<<it2.second;
/*=======
    if(log){
		for(auto elem: rep){
			_INFO<<"feature "<<elem.first;
			for(auto elem2: elem.second){
				_INFO<<" key "<<elem2.first<<" value "<<elem2.second;

>>>>>>> morph_a_ref*/
			}
		}
    }
  }

private:
  map_t rep;
  cache_t cache;
  extractor_t extractor;
//<<<<<<< HEAD
  bool track = false;
//=======
//  bool log = false;
//>>>>>>> morph_a_ref
};

}
}

#endif  //  end for __ZGEN_SHIFTREDUCE_MODEL_SCORE_MAP_H__
