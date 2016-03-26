#ifndef __ZGEN_SHIFTREDUCE_MODEL_MODEL_H__
#define __ZGEN_SHIFTREDUCE_MODEL_MODEL_H__

#include "shiftreduce/option.h"
#include "score.h"
#include "score_map.h"
#include "shiftreduce/ngram/ngram.h"
#include <vector>

namespace ZGen {
namespace ShiftReduce {

class Model {
public:
  Model(const option_t& opts);

  /**
   * Get score for the state.
   *
   *  @param[in]  state   The state
   *  @param[in]  act     The action
   *  @param[in]  avg     The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state.
   */
//  floatval_t score(const StateItem& state, const Action& act, bool avg);

  /**
   * Get score for the state context.
   *
   *  @param[in]  ctx   The input state context.
   *  @param[in]  act   The action.
   *  @param[in]  avg   The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state
   *                           context.
   */
  floatval_t score(const ScoreContext& ctx, const Action& act, bool avg);

  floatval_t score(ScoreContext& ctx, const StateItem& state, const Action& act, bool avg, lm::ngram::ProbingModel::State &out, const Engine::TokenAlphabet& forms_alphabet);
  /**
   * Update the model with the state.
   *
   *  @param[in]  state     The input state.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const StateItem& state, const Action& act, int timestamp,
      const floatval_t& scale);

  /**
   * Update the model with the state context.
   *
   *  @param[in]  ctx       The input state context.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const ScoreContext& ctx, const Action& act, int timestamp,
      const floatval_t& scale);

  void update(const StateItem& state, const Action& act, int timestamp,
	  const floatval_t& scale,  const Engine::TokenAlphabet& forms_alphabet);
  /**
   */
  void flush(int timestamp);

  /**
   * Save the model into the output stream.
   *
   *  @param[out] os    The output stream.
   *  @return     bool  If successfully saved, return true; otherwise return
   *                    false.
   */
  bool save(std::ostream& os);

  /**
   * Load the model fron input stream.
   *
   *  @param[in]  is    The input stream.
   *  @return     bool  If successfully loaded, return true; otherwise return
   *                    false.
   */
  bool load(std::istream& is);

  /**
   * Load the language model from input path.
   * 
   *  @param[in] path	The input path
   *  @return	 bool 	If successfully loaded, return true; otherwise return
   *  					false.
   */
  bool load(const option_t& opts);
private:
  std::vector< ScoreMap<UnigramMetaFeature> > uscore_repo;
  std::vector< ScoreMap<BigramMetaFeature>  > bscore_repo;
  std::vector< ScoreMap<TrigramMetaFeature> > tscore_repo;
  std::vector< ScoreMap<FourgramMetaFeature> > foscore_repo;

	void standard_features(const option_t& opts);

public:
  Ngram* ngram;
};

}
}

#endif  //  end for __ZGEN_SHIFTREDUCE_MODEL_MODEL_H__
