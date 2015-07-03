#ifndef __ZGEN_SHIFTREDUCE_PIPE_TYPES_H__
#define __ZGEN_SHIFTREDUCE_PIPE_TYPES_H__

#include "shiftreduce/types/action.h"
#include "shiftreduce/types/state.h"
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <boost/tuple/tuple.hpp>

//ljm
//{
#include "shiftreduce/ngram/lm/model.hh"
//}
namespace ZGen {
namespace ShiftReduce {

typedef std::vector<action_t> action_collection_t;  //! a list of actions;

typedef std::vector<action_t> action_sequence_t;    //! also a list of actions;

typedef std::unordered_map<
  action_t, floatval_t, boost::hash<action_t>
> packed_score_t; //

//ljm
//{
typedef std::unordered_map<
  action_t, lm::ngram::ProbingModel::State, boost::hash<action_t>
> packed_ngstate_t;
//}

typedef boost::tuples::tuple<
  const StateItem*, action_t, floatval_t
> scored_transition_t; //

} //  namespace for Shiftreduce
} //  namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_PIPE_TYPES_H__
