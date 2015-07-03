#ifndef __ZGEN_BESTFRIST_MODEL_H__
#define __ZGEN_BESTFRIST_MODEL_H__

#include "settings.h"
#include "bestfirst/search/span.h"
#include "bestfirst/model/score_map.h"

namespace ZGen {

namespace BestFirst {

class Model {
public:
  Model();

  /**
   *
   *
   *
   *
   */
  floatval_t score(const Span& span, bool avg) const;

  /**
   * Scoring the span.
   *
   *  @param[in]  ctx        The span context.
   *  @return     floatval_t  The score for the span.
   */
  floatval_t score(const ScoreContext& ctx, bool avg) const;

  /**
   * Update the model parameters according to the model.
   *
   *  @param[in]  span      The span.
   *  @param[in]  timestamp The timestamp.
   *  @param[in]  scale     The update scale.
   */
  void update(const Span& span, int timestamp, floatval_t scale);

  /**
   * Update the model parameters according to the model.
   *
   *  @param[in]  span      The span.
   *  @param[in]  timestamp The timestamp.
   *  @param[in]  scale     The update scale.
   */
  void update(const ScoreContext& ctx, int timestamp, floatval_t scale);

  void flush(int timestamp);

  /**
   * Load model from the input stream.
   *
   *  @param[in]  ifs   The input stream.
   *  @return     bool  If successfully loaded, return true; otherwise return
   *                    false.
   */
  bool load(std::istream& is);

  /**
   * Save model to the output stream.
   *
   *  @param[out] ofs   The output stream.
   *  @return     bool  If successfully saved, return true; otherwise return
   *                    false.
   */
  bool save(std::ostream& os);
private:
  std::vector< ScoreMap<UnigramScore> > uscore_repo;
  std::vector< ScoreMap<BigramScore> >  bscore_repo;
  std::vector< ScoreMap<TrigramScore> > tscore_repo;
};

} //  end for namespace Bestfirst
} //  end for namespace ZGen


#endif  //  end for __ZGEN_BESTFRIST_MODEL_H__
