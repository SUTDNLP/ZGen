#ifndef __ZGEN_SHIFTREDUCE_NGRAM_NGRAM_H__
#define __ZGEN_SHIFTREDUCE_NGRAM_NGRAM_H__

#include "engine/token_alphabet.h"
#include "shiftreduce/option.h"
#include "shiftreduce/types/state.h"
#include "settings.h"
#include "shiftreduce/ngram/lm/ngram_query.hh"
#include <vector>
namespace ZGen {
namespace ShiftReduce {

class Ngram {
public:
 
  Ngram(const std::string& ngram_path);
  /**
   * Get score for the state.
   *
   *  @param[in]  state   The state
   *  @param[in]  act     The action
   *  @param[in]  avg     The average parameter.
   *  @return     int  	  The index of bin for the score
   */
  int score(const StateItem& state, const Action& act, lm::ngram::ProbingModel::State& out, const Engine::TokenAlphabet& forms_alphat);

  /**
   */

  int bin(floatval_t result);
  /**
   */
  void flush(int timestamp);

  /**
   * Load the model fron input stream.
   *
   *  @param[in]  is    The input stream.
   *  @return     bool  If successfully loaded, return true; otherwise return
   *                    false.
   */
  bool load(const std::string& filename);

  /**
   */
  lm::ngram::ProbingModel::State BeginSentenceState();
private:
  lm::ngram::ProbingModel* model;
};

}
}

#endif  //  end for __ZGEN_SHIFTREDUCE_NGRAM_NGRAM_H__
