#ifndef __ZGEN_SHIFTREDUCE_DECODER_H__
#define __ZGEN_SHIFTREDUCE_DECODER_H__

#include <iostream>
#include "knowledge/postag_constraint.h"
#include "shiftreduce/option.h"
#include "shiftreduce/types/state.h"
#include "shiftreduce/types/internal_types.h"
#include "shiftreduce/model/model.h"
#include "shiftreduce/decode/decode_context.h"
//ljm
//{
#include "shiftreduce/ngram/ngram.h"
//}
namespace ZGen {
namespace ShiftReduce {

class Decoder {
public:
  /**
   * The constructor.
   *
   *  @param[in]  opts        The global option.
   *  @param[in]  ctx         The decode context, aka, lattice
   *  @param[in]  model       The model
   *  @param[in]  constraint  The postag constraint
   */
  Decoder(const option_t& opts,
      DecodeContext* ctx,
      Model* model,
      const Knowledge::PostagConstraint* constraint,
      const Engine::TokenAlphabet& forms_alphabet,
	  int tag,
      int nr_relations = 3);

  ~Decoder();

  typedef std::pair<const StateItem*, const StateItem*> decode_result_t;

  /**
   * Perform the beam-search decoding.
   *
   *  @param[in]  input           The input dependency instance.
   *  @param[in]  gold_actions    The gold transition action (only actived in
   *                              the learning process).
   *  @return     decode_result_t The decode result.
   */
  decode_result_t decode(const dependency_t* input,
      const action_sequence_t& gold_actions);

protected:
  /**
   * Get the possible actions from the current state. Store the possible
   * actions in the possible_actions (action_collection_t)
   *
   *  @param[in]  item        The input item
   */
  virtual void get_possible_actions(const StateItem& item) = 0;

  /**
   * Config the input onto the decoder.
   *
   *  @param[in]  input       The pointer to the input instance.
   */
  virtual void config_input(const dependency_t* input) = 0;

  /**
   * Get the possible shift action from the current state, by refering the
   * postag constrain. The possible actions is stored in possible_actions
   *
   *  @param[in]  item        The input state item.
   *  @param[in]  index       The index.
   *  @param[in]  word        The word.
   *  @param[in]  postag      The postag.
   */
  void get_possible_shift_actions(const StateItem& item,
      int index, word_t word, postag_t postag);

  const option_t& opts;   //! The options.
  Model* model;           //! The model.
  DecodeContext* ctx;     //! The decode context.
  packed_ngstate_t packed_ngstate;
  const Engine::TokenAlphabet& forms_alphabet;
  //! The cached possible actions.
  action_sequence_t possible_actions;
  //! The cached packed score for transition.
  packed_score_t packed_score;
  //! The candidate state tuple: (from state, action, score)
  scored_transition_t candidate_transitions[kMaxBeamSize];
  //! The postag constraints.
  const Knowledge::PostagConstraint* constraint;
  int NP;           //! The index of the Noun Phrash, special tag.
  int nr_relations; //! The number of dependency relation.
private:
  /**
   * Scoring all the possible actions stored in possible_actions given the
   * input state.
   *
   *  @param[in]  item        The input state item.
   */
  void score_possible_actions(const StateItem& item);

  /**
   * Insert the scored transition into the beam.
   *
   *  @param[in]  trans             The scored transition.
   *  @param[in]  current_beam_size The current beam size.
   *  @return     int               If a new transition is added into the beam,
   *                                return 1; otherwise return 0.
   */
  int extend_candidate_transition(const scored_transition_t& trans,
      int current_beam_size);

  /**
   * Clear the array of candidate transitions.
   */
  void clear_candidate_transition();

  /**
   * Perform transition action act on the state stored in from and result in
   * the state to.
   *
   *  @param[in]  from  The fron state.
   *  @param[in]  act   The action.
   *  @param[in]  score The pre-calculated score of this transition.
   *  @param[out] to    The output state
   */
  void transit(const StateItem& from, const action_t& act,
      floatval_t score, StateItem* to);

  /**
   * Search the correct state in the given range. The first pass, find the
   * state with correct (action, word, index) state. If such state is found
   * return this state. Otherwise perform the second pass, find the state
   * with correct (action, word) state ignoring index, because the input
   * sentence may have duplicated forms, like comma.
   *
   *  @param[in] act                    The action.
   *  @param[in] previous_correct_state The correct state.
   *  @param[in/out]  begin             The starting of the states.
   *  @param[in/out]  end               The ending of the states.
   *  @return         const StateItem*  The pointer to the state.
   */
  const StateItem* search_correct_state(const action_t& act,
      const StateItem* previous_correct_state,
      StateItem* begin, StateItem* end);

  /**
   * Search the best state in the given range.
   *
   *  @param[in]  begin   The begining of this range.
   *  @param[in]  end     The end of this range.
   *  @return     const StateItem*  The pointer to the best state.
   */
  const StateItem* search_best_state(const StateItem* begin,
      const StateItem* end);
};

} //  end for namespace ShiftReduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_DECODER_H__
