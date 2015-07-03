#include <algorithm>
#include <boost/assert.hpp>
#include <boost/log/trivial.hpp>
#include "bestfirst/search/span.h"
#include "bestfirst/learning/learner.h"

namespace ZGen {

namespace BestFirst {

Learner::Learner(Model* _model,
    int _max_iteration)
  : model(_model),
  timestamp(0),
  max_iteration(_max_iteration) {
}


void
Learner::config_timestamp(int _timestamp) {
  //timestamp = _timestamp;
  //timestamp += 1;
}

int
Learner::learn(const SpanSet& gold_spans,
    SpanMaxHeap& agenda) {
  ++ timestamp;
  BOOST_ASSERT(model != NULL);
  //
  load_sortedlist_from_agenda(agenda);

  int nr_gold_spans_in_sortedlist = 
    count_number_of_gold_spans_in_sortedlist(gold_spans);

  if (nr_gold_spans_in_sortedlist == 0) {
    return NO_GOLD_IN_AGENDA;
  }

  BOOST_LOG_TRIVIAL(trace) << "Learner::learn #(gold spans)="
    << nr_gold_spans_in_sortedlist
    << ", #(sorted spans)="
    << sorted_span_list.size();

  bool converged = test_convergence(nr_gold_spans_in_sortedlist,
      gold_spans);

  for (int i = 0; !converged && i < max_iteration; ++ i) {
    BOOST_LOG_TRIVIAL(trace) << "Learner::iteration #" << i;
    learn_one_iteration();
    converged = test_convergence(nr_gold_spans_in_sortedlist,
        gold_spans);
  }

  int ret = CONVERGED;
  if (converged) {
    BOOST_LOG_TRIVIAL(trace) << "Learner::learn converged";
    ret = CONVERGED;
  } else {
    if (gold_spans.find(sorted_span_list[0]) != gold_spans.end()) {
      ret = SAFE_UNCONVERGED;
      BOOST_LOG_TRIVIAL(trace) << "Learner::learn "
        << "unconverged, but the best span is gold span.";
    } else {
      ret = UNSAFE_UNCONVERGED;
       BOOST_LOG_TRIVIAL(trace) << "Learner::learn "
        << "unconverged, and the best span is not gold span.";
    }
  }
  load_agenda_from_sortedlist(gold_spans, agenda);
  return ret;
}

void
Learner::load_sortedlist_from_agenda(SpanMaxHeap& agenda) {
  sorted_span_list.clear();
  for (SpanMaxHeap::iterator i = agenda.begin(); i != agenda.end(); ++ i) {
    sorted_span_list.push_back((*i));
  }
  // Sort the span list.
  std::sort(sorted_span_list.begin(), sorted_span_list.end(), SpanPtrGreater());
}

void
Learner::load_agenda_from_sortedlist(SpanMaxHeap& agenda) {
  agenda.clear();
  for (int i = 0; i < sorted_span_list.size(); ++ i) {
    agenda.push(sorted_span_list[i]);
  }
}

void
Learner::load_agenda_from_sortedlist(const SpanSet& gold_spans, 
    SpanMaxHeap& agenda) {
  agenda.clear();
  bool flag = false;
  for (int i = 0; i < sorted_span_list.size(); ++ i) {
    if (gold_spans.find(sorted_span_list[i]) != gold_spans.end()) {
      flag = true;
    }
    if (flag) {
      agenda.push(sorted_span_list[i]);
    }
  }
}

int
Learner::count_number_of_gold_spans_in_sortedlist(
    const SpanSet& gold_spans) {
  int ret = 0;
  for (int i = 0; i < sorted_span_list.size(); ++ i) {
    Span* span = sorted_span_list[i];
    if (gold_spans.find(span) != gold_spans.end()) {
      ++ ret;
    }
  }
  return ret;
}

bool
Learner::test_convergence(int nr_gold_spans_in_sortedlist,
    const SpanSet& gold_spans) {
  collect_positive_spans(nr_gold_spans_in_sortedlist, gold_spans);
  collect_negative_spans(nr_gold_spans_in_sortedlist, gold_spans);
  return (negative_spans.size() == 0);
}

int
Learner::collect_positive_spans(int nr_gold_spans,
    const SpanSet& gold_spans) {
  positive_spans.clear();
  for (int i = nr_gold_spans; i < sorted_span_list.size(); ++ i) {
    Span* span = sorted_span_list[i];
    if (gold_spans.find(span) != gold_spans.end()) {
      positive_spans.push_back(span);
    }
  }
  return positive_spans.size();
}

int
Learner::collect_negative_spans(int nr_gold_spans,
    const SpanSet& gold_spans) {
  negative_spans.clear();
  for (int i = 0; i < nr_gold_spans; ++ i) {
    Span* span = sorted_span_list[i];
    if (gold_spans.find(span) == gold_spans.end()) {
      negative_spans.push_back(span);
    }
  }
  return negative_spans.size();
}

void
Learner::learn_one_iteration() {
  increase_positive_spans(positive_spans);
  decrease_negative_spans(negative_spans);

  // Rescore
  score_spans_in_sortedlist();

  // Sort the span list.
  std::sort(sorted_span_list.begin(), sorted_span_list.end(), SpanPtrGreater());
}

void
Learner::score_spans_in_sortedlist() {
  for (int i = 0; i < sorted_span_list.size(); ++ i) {
    Span* span = sorted_span_list[i];
    // Pay attention to this.
    span->score = model->score(*span, true);
  }
}

void
Learner::increase_positive_spans(
    const std::vector<Span *>& positive_spans) {
  for (int i = 0; i < positive_spans.size(); ++ i) {
    model->update(*positive_spans[i], timestamp, 1);
  }
}

void
Learner::decrease_negative_spans(
    const std::vector<Span *>& negative_spans) {
  BOOST_ASSERT(timestamp!= -1);
  for (int i = 0; i < negative_spans.size(); ++ i) {
    model->update(*negative_spans[i], timestamp, -1);
  }
}

} //  end for namespace BestFirst

} //  end for namespace ZGen
