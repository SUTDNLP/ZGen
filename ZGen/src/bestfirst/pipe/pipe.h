#ifndef __ZGEN_BESTFIRST_PIPE_H__
#define __ZGEN_BESTFIRST_PIPE_H__

#include "types/instance.h"
#include "knowledge/postag_constrain.h"
#include "bestfirst/option.h"
#include "bestfirst/learning/learner.h"
#include "bestfirst/search/span_types.h"
#include "bestfirst/search/span_allocator.h"
#include "bestfirst/search/span_factory.h"

namespace ZGen {

namespace BestFirst {

class Pipe {
public:
  Pipe(const Option& opts);

  ~Pipe();

  /**
   * Perform realization.
   */
  void realize();

  /**
   * Save model according to the options.
   */
  void save_model();

private:
  bool prepare_data();

  //!
  void build_output(const Span* span, dependency_t& output);

private:
  //! The options.
  const Option& opts;
  //! The model.
  Model* model;
  //! The postag constrain.
  Knowledge::PostagConstrain * constrain;
  //! The span allocator.
  SpanAllocator*  allocator;
  //! The span factory
  SpanFactory* factory;
  //! The learner
  Learner* learner;

  SpanSet         gold_spans;   /*< The gold spans. */
  SpanMaxHeap     agenda;       /*< The open table in best-first search. */
  SpanKMaxHeap    chart;        /*< The span chart for storing spans.*/
  SpanSet         chart_mask;   //!

  std::vector<dependency_t> dataset;
  std::vector<dependency_t> referset;
};

} //  end for namespace BestFirst

} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_PIPE_H__
