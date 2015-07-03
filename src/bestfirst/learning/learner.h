#ifndef __ZGEN_BESTFIRST_LEARNING_LEARNER_H__
#define __ZGEN_BESTFIRST_LEARNING_LEARNER_H__

#include "bestfirst/model/model.h"
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_types.h"
#include "bestfirst/search/span_allocator.h"

namespace ZGen {

namespace BestFirst {

class Learner {
public:
   //! The learning result status.
  enum { CONVERGED = 0,
    SAFE_UNCONVERGED,
    UNSAFE_UNCONVERGED,
    NO_GOLD_IN_AGENDA };

  Learner(Model* model, int max_iteration= 1);

  /**
   * Config the model onto the learner.
   *
   *  @param[in]  model   The pointer to the model.
   */
  void config_model(Model* model);

  /**
   * Config the timestamp to learner.
   *
   *  @param[in]  timestamp The timestamp.
   */
  void config_timestamp(int timestamp);

  /**
   * The main function for Perceptron learning. The learning process
   * includes:
   *  - 1: calculate the number of gold spans in the agenda.
   *  - 2: check whether the current agenda is converged.
   *  - 3: while not converged:
   *         update parameter,
   *         check converged
   *
   * PREREQUISITE
   *  - model is configed.
   *  - timestamp is configed.
   *
   *  @param[in]      gold_spans  The set of gold spans.
   *  @param[in/out]  agenda      The agenda.
   *  @return         int         The learning status.
   */
  int learn(const SpanSet& gold_spans,
      SpanMaxHeap& agenda);

private:
  /**
   * Check if the current status in agenda has reach the converged status.
   * [PREREQUISITE]
   *  - collect positive spans / collect negative span is performed.
   *
   *  @param[in]  nr_gold_spans   The number of gold spans in agenda.
   *  @param[in]  gold_spans      The set of gold spans.
   *  @return     bool            If the agenda is converged, return true,
   *                              otherwise return false.
   */
  bool test_convergence(int nr_gold_spans, const SpanSet& gold_spans);

  /**
   * Perform one iteration perceptron learning process.
   * [PREREQUISITE]
   *  - collect positive spans / collect negative spans is performed.
   *
   *  @return         int         The converged status, if converged return 0,
   *                              otherwise, return 1
   */
  void learn_one_iteration();

  /**
   * Dump the agenda to the sorted list.
   *
   *  @param[in]  agenda  The input span agenda.
   */
  void load_sortedlist_from_agenda(SpanMaxHeap& agenda);

  /**
   * Clear the agenda and reload the spans from sorted list.
   *
   *  @param[out] agenda  The output span agenda.
   */
  void load_agenda_from_sortedlist(SpanMaxHeap& agenda);

  /**
   * Clear the agenda and reload the spans from sorted list.
   *
   *  @param[out] agenda  The output span agenda.
   */
  void load_agenda_from_sortedlist(const SpanSet& gold_spans,
      SpanMaxHeap& agenda);

  /**
   * Count the number of gold spans in the sorted list.
   *
   *  @param[in]  gold_spans  The gold spans
   */
  int count_number_of_gold_spans_in_sortedlist(const SpanSet& gold_spans);

  /**
   * Collect positive spans from the sorted list according to the set of gold
   * spans. This process includes iterating from the nth index of the sorted
   * list and find the span in the set of gold spans.
   *
   *  @param[in]  nr_gold_spans   The number of gold spans in agenda.
   *  @param[in]  gold_spans      The set of gold spans.
   *  @return     int             The number of positive spans.
   */
  int collect_positive_spans(int nr_gold_spans,
      const SpanSet& gold_spans);

  /**
   * Collect negative spans from the sorted list according to the set of gold
   * spans. This process includes iterating from the 0 index of the sorted
   * list and find the span in the set of gold spans.
   *
   *  @param[in]  nr_gold_spans   The number of gold spans in agenda.
   *  @param[in]  gold_spans      The set of gold spans.
   *  @return     int             The number of negative spans.
   */
  int collect_negative_spans(int nr_gold_spans,
      const SpanSet& gold_spans);

  /**
   * Increase the weight for the positive spans.
   *
   *  @param[in]  positive_spans  The positive spans.
   */
  void increase_positive_spans(const std::vector<Span*>& positive_spans);

  /**
   *
   *
   */
  void decrease_negative_spans(const std::vector<Span*>& negative_spans);

  /**
   *
   *
   *
   */
  void score_spans_in_sortedlist();
private:
  //! The pointer to the model.
  Model* model;
  int max_iteration;
  int timestamp;
  std::vector<Span *> positive_spans;
  std::vector<Span *> negative_spans;
  std::vector<Span *> sorted_span_list;
};

} //  end for namespace BestFirst

} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_LEARNING_PERCEPTRON__
