#ifndef __ZGEN_SHITREDUCE_TYPES_STATE_H__
#define __ZGEN_SHITREDUCE_TYPES_STATE_H__

#include <iostream>
#include <bitset>
#include <cstring>
#include "settings.h"
#include "shiftreduce/settings.h"
#include "shiftreduce/types/action.h"
#include "types/graph.h"
#include "engine/token_alphabet.h"
//ljm
//{
#include "shiftreduce/ngram/lm/model.hh"
//}
namespace ZGen {

namespace ShiftReduce {

class StateItem{
public:
  // The default constructor,
  StateItem();

  // The constructor with sentence reference.
  StateItem(const dependency_t* ref);

  /**
   * Set the sentence reference to the current StateItem, it won't
   * check whether the reference is seted.
   *
   *  @param[in] _ref   The pointer to the sentence
   */
  void set_reference(const dependency_t* _ref);

  void set_graph(const graph_t* graph);

  void set_forms_alphabet(const Engine::TokenAlphabet* forms_alphabet);

  void set_deprels_alphabet(const Engine::TokenAlphabet* deprels_alphabet);

  void set_pos_alphabet(const Engine::TokenAlphabet* pos_alphabet);

  /**
   * Clear the state item.
   */
  void clear();

public:
  //! assign operator
  StateItem & operator = (const StateItem& item);

  //! less than operator
  bool operator < (const StateItem& other) const;

  //! greater than operator
  bool operator > (const StateItem& other) const;

  friend std::ostream & operator << (std::ostream& os, const StateItem& item);

public:
  int top0;   //! The top word in the stack of the current state
  int top1;   //! The second top word.
  const dependency_t* ref;    //! The reference to the sentence
  const StateItem* previous;  //! The previous state
  floatval_t score;           //! The score for the current state
  std::vector<int> stack;     //! The stack for the current state
  std::vector< word_t >   word_sequence;      //! The sequence of words leading by the current state.
  std::vector< postag_t > postag_sequence;    //! The sequence of postags leading by the current state.
  std::bitset<kMaxNumberOfWords> buffer;      //! The word indices in buffer for current state.
  int rank[kMaxNumberOfWords];                //! The rank of the form, with word considered.
  int postags[kMaxNumberOfWords];             //! The postags cache.
  int heads[kMaxNumberOfWords];               //! The tree(heads) for the current status
  int deprels[kMaxNumberOfWords];             //! The tree(deprels) for the current state
  int nr_left_children[kMaxNumberOfWords];    //! The number of left children.
  int nr_right_children[kMaxNumberOfWords];   //! The number of right children.
  int nr_left_descendant[kMaxNumberOfWords];  //! The sum of left descendants.
  int nr_right_descendant[kMaxNumberOfWords]; //! The sum of right descendants.
  int left_most_child[kMaxNumberOfWords];     //! CACHE: use to store the left most child for the word.
  int right_most_child[kMaxNumberOfWords];    //! CACHE: use to store the right most child for the word.
  int left_2nd_most_child[kMaxNumberOfWords]; //! CACHE: use to store the 2nd-left-most child.
  int right_2nd_most_child[kMaxNumberOfWords];//! CACHE: use to store the 2nd-right-most child.
  action_t last_action;                       //! The action that result in the current state.
  const graph_t* graph;
  std::vector<int> words_shifted;
  const Engine::TokenAlphabet* forms_alphabet;
  const Engine::TokenAlphabet* deprels_alphabet;
  const Engine::TokenAlphabet* pos_alphabet;
  //ljm
  //{
  lm::ngram::ProbingModel::State ngstate;
  //}
public:

  /**
   * Copy other state into the current state.
   *
   *  @param[in]  other   The other state.
   */
  void copy(const StateItem & other);

  /**
   * Perform shift on current state, result in a new state on itself.
   *
   *  @param[in]  postag    The PoSTag (represented in integer).
   *  @param[in]  word      The word (represented in integer).
   *  @param[in]  word_id   The index of the word in this sentence.
   *  @return     bool      If successfully shift, return true.
   *                        otherwise false.
   */
  bool shift(postag_t postag, word_t word, int word_id);

  /**
   * Perform shift on current state and result in a new state.
   *
   *  @param[in]  postag    The PoSTag (represented in integer).
   *  @param[in]  word      The word (represented in integer).
   *  @param[in]  word_id   The index of the word in this sentence.
   *  @param[out] other     The pointer to the output state.
   *  @return     bool      If successfully shift, return true,
   *                        otherwise false
   */
  bool shift(postag_t postag, word_t word,
      int word_id, StateItem* other) const;

  /**
   * Perform left arc on current state, result in a new state on itself.
   *
   *  @param[in]  deprel    The dependency relation.
   *  @return     bool      If successfully performed action, return true;
   *                        Otherwise false.
   */
  bool left_arc(deprel_t deprel);

  /**
   * An output wrapper for left arc by performing left arc and result in
   * a new state.
   */
  bool left_arc(deprel_t deprel, StateItem* other) const;

  /**
   * Perform right arc on current state, result in a new state on itself.
   *
   *  @param[in]  deprel    The dependency relation.
   *  @return     bool      If successfully performed action, return true;
   *                        Otherwise false.
   */
  bool right_arc(deprel_t deprel);

  /**
   * An output wrapper for right arc by performing left arc and result in
   * a new state.
   */
  bool right_arc(deprel_t deprel, StateItem* other) const;
};

} //  end for namespace ShiftReduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHITREDUCE_TYPES_STATE_H__
