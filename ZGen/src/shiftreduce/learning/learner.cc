#include "utils/logging.h"
#include "shiftreduce/learning/learner.h"
#include <boost/assert.hpp>

namespace ZGen {
namespace ShiftReduce {

Learner::Learner(const option_t& _opts,
    Model* _model,
    const DecodeContext* _ctx)
  : opts(_opts),
  model(_model),
  timestamp(-1),
  ctx(_ctx) {
}

void
Learner::set_timestamp(int _timestamp) {
  timestamp = _timestamp;
}

void
Learner::learn(const StateItem* predict_state,
    const StateItem* correct_state, const Engine::TokenAlphabet& forms_alphabet ) {
  if (predict_state == correct_state) {
    return;
  }

  int nr_predict_states = 0;
  int nr_correct_states = 0;

  for (const StateItem* p = predict_state; p; p = p->previous) {
    predict_state_chain[nr_predict_states] = p;
    ++ nr_predict_states;
  }

  for (const StateItem* p = correct_state; p; p = p->previous) {
    correct_state_chain[nr_correct_states] = p;
    ++ nr_correct_states;
  }

  BOOST_ASSERT(nr_predict_states== nr_correct_states);

  int i;
  for (i = nr_correct_states- 1; i >= 0; -- i) {
    if (predict_state_chain[i]->last_action != correct_state_chain[i]->last_action) {
      break;
    }
  }

  for (i = i+ 1; i > 0; -- i) {
    const action_t& predict_action = predict_state_chain[i- 1]->last_action;
    const action_t& correct_action = correct_state_chain[i- 1]->last_action;
    _TRACE << "learn: update " << (void *)predict_state_chain[i] << ", "
      << predict_action << " by -1";
    _TRACE << "learn: update " << (void *)correct_state_chain[i] << ", "
      << correct_action << " by +1";
    model->update((*predict_state_chain[i]), predict_action, timestamp, -1., forms_alphabet);
    model->update((*correct_state_chain[i]), correct_action, timestamp, 1., forms_alphabet);
  }
}

} //  end for namespace ShiftReduce
} //  end for namespace ZGen
