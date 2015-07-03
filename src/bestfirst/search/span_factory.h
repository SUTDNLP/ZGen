#ifndef __ZGEN_BESTFIRST_SEARCH_SPAN_FACTORY_H__
#define __ZGEN_BESTFIRST_SEARCH_SPAN_FACTORY_H__

#include <vector>
#include "types/instance.h"
#include "knowledge/postag_constrain.h"
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_types.h"
#include "bestfirst/search/span_allocator.h"

namespace ZGen {

namespace BestFirst {

class SpanFactory {
public:
  SpanFactory(const Knowledge::PostagConstrain* constrain,
      SpanAllocator* allocator);

  /**
   * Make initial spans from the input data. These spans is used as initial
   * spans to the searching process.
   *
   *  @param[in]  input_ref   The input data's reference.
   *  @param[out] spans       The output spans.
   */
  void create_initial_spans_from_input(const dependency_t* input_ref,
      std::vector<Span *>& spans);

  /**
   * Make spans (namely, the transition in the search process) from the charts.
   *
   *  @param[in]  span        The from spans.
   *  @param[in]  charts      The current chart.
   *  @param[out] spans       The output spans.
   */
  void create_spans_from_chart(const Span* span,
      const SpanKMaxHeap* charts,
      std::vector<Span *>& spans);

  /**
   * Enumerate all the gold spans from a reference dependency tree.
   *
   *  @param[in]  input       The input dependency.
   *  @param[in]  reference   The reference dependency.
   *  @param[out] spans       The output spans.
   */
  void create_gold_spans_from_reference(const dependency_t* input,
      const dependency_t* reference,
      SpanSet& spans);
protected:

  //! legal_to_left_combine and legal_to_right_combine are the functions
  //! which needed to be override for different input constrains
  //! implementation.
  /**
   * Check if it is legal to construct a span by combining the head and the 
   * modifier as a left modifier.
   *
   *  @param[in]  head      The pointer to the head span.
   *  @param[in]  modifier  The pointer to the modifier span.
   *  @return     bool      Return true when it is legal, otherwise return
   *                        false.
   */
  virtual bool legal_to_left_combine(const Span* head,
      const Span* modifier) const;

  /**
   * Check if it is legal to construct a span by combining the head and the 
   * modifier as a right modifier.
   *
   *  @param[in]  head      The pointer to the head span.
   *  @param[in]  modifier  The pointer to the modifier span.
   *  @return     bool      Return true when it is legal, otherwise return
   *                        false.
   */
  virtual bool legal_to_right_combine(const Span* head,
      const Span* modifier) const;
private:
  typedef std::vector<int> edges_t;

private:
  /**
   * Perform DFS on the given input reference.
   *
   *  @param[in]  now             The currently visited node.
   *  @param[in]  input           The input dependency reference,
   *                              for span creation.
   *  @param[in]  left_modifiers  The left modifiers.
   *  @param[in]  right_modifiers The right modifiers.
   *  @param[out] spans           The output spans.
   */
  Span* create_gold_spans_from_reference_travel(int now,
      const dependency_t* input,
      const dependency_t* reference,
      const std::vector< edges_t>& left_modifiers_tree,
      const std::vector< edges_t>& right_modifiers_tree,
      SpanSet& spans);

private:
  SpanAllocator* allocator;
  const Knowledge::PostagConstrain* constrain;
};

} //  end for namespace Bestfirst

} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_SEARCH_SPAN_FACTORY_H__
