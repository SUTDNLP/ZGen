#ifndef __ZGEN_SHIFTREDUCE_LEARNER_H__
#define __ZGEN_SHIFTREDUCE_LEARNER_H__

#include "shiftreduce/option.h"
#include "shiftreduce/types/state.h"
#include "shiftreduce/model/model.h"
#include "shiftreduce/decode/decode_context.h"

namespace ZGen {
namespace ShiftReduce {

class Learner {
public:
  Learner(const option_t& opts,
      Model* model,
      const DecodeContext* ctx);

  /**
   * Set timestamp to the learner.
   *
   *  @param[in]  timestamp   The time stamp.
   */
  void set_timestamp(int timestamp);

  /**
   * Perform learning process.
   *
   *  @param[in]  last_state_of_predict_chain The last state in state chain
   *                                          for postive result.
   *  @param[in]  last_state_of_correct_chain The last state in state chain
   *                                          for negative result.
   */
  void learn(const StateItem* last_state_of_predict_chain,
      const StateItem* last_state_of_correct_chain, const Engine::TokenAlphabet& forms_alphabet);

  /**
   *
   *
   *
   *
   */
  void finish_learning(int now);
private:
  const option_t& opts;
  int timestamp;
  Model* model;
  const DecodeContext* ctx;
  const StateItem* predict_state_chain[kMaxSteps];
  const StateItem* correct_state_chain[kMaxSteps];
};

} //  end for namespace ShiftReduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_LEARNER_H__
