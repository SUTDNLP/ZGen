#include <vector>
#include <boost/log/trivial.hpp>
#include "types/instance.h"
#include "engine/postag_alphabet.h"
#include "bestfirst/search/span_types.h"
#include "bestfirst/search/span_factory.h"
#include "bestfirst/search/span_allocator.h"

namespace ZGen {

namespace BestFirst {

namespace kn = ZGen::Knowledge;

SpanFactory::SpanFactory(const kn::PostagConstrain* _constrain,
    SpanAllocator* _allocator)
  : constrain(_constrain), 
  allocator(_allocator) {
}

void
SpanFactory::create_initial_spans_from_input(const dependency_t* instance,
    std::vector<Span *>& spans) {
  namespace eg = ZGen::Engine;

  spans.clear();
  for (int i = 0; i < instance->size(); ++ i) {
    word_t form = instance->form(i);
    std::vector<postag_t> tags;

    if (instance->is_phrases[i]) {
      tags.push_back(eg::PennTreebankPostagAlphabet::NP);
    } else {
      constrain->get_possible_tags(form, tags);
    }

    for (int j = 0; j < tags.size(); ++ j) {
      postag_t tag = tags[j];
      Span* span = allocator->malloc();
      span->init(instance, i, tag);
      spans.push_back(span);
    }
  }
}

void
SpanFactory::create_spans_from_chart(const Span* span,
    const SpanKMaxHeap* charts,
    std::vector<Span *>& spans) {
  spans.clear();
  for (SpanKMaxHeap::iterator i = charts->begin();
      i != charts->end(); ++ i) {
    const Span* span2 = (*i);

    if (legal_to_left_combine(span, span2)) {
      Span* new_span = allocator->malloc();
      new_span->copy(span);
      new_span->link_left(span2);
      spans.push_back(new_span);
    }

    if (legal_to_left_combine(span2, span)) {
      Span* new_span = allocator->malloc();
      new_span->copy(span2);
      new_span->link_left(span);
      spans.push_back(new_span);
    }

    if (legal_to_right_combine(span, span2)) {
      Span* new_span = allocator->malloc();
      new_span->copy(span);
      new_span->link_right(span2);
      spans.push_back(new_span);
    }

    if (legal_to_right_combine(span, span2)) {
      Span* new_span = allocator->malloc();
      new_span->copy(span2);
      new_span->link_right(span);
      spans.push_back(new_span);
    }
  }
}

void
SpanFactory::create_gold_spans_from_reference(const dependency_t* input,
    const dependency_t* reference,
    SpanSet& spans) {
  int N = input->size();
  std::vector<edges_t > left_modifiers;
  std::vector<edges_t > right_modifiers;

  left_modifiers.resize(N);
  // [NOTE] left modifier are stored in the original order.
  right_modifiers.resize(N);

  int root = -1;
  for (int i = 0; i < N; ++ i) {
    int head = reference->heads[i];
    if (head == -1) {
      root = i;
      continue;
    }
    if (head > i) {
      // left modifier
      left_modifiers[head].push_back(i);
    } else if (head < i) {
      // right modifier
      right_modifiers[head].push_back(i);
    }
  }

  create_gold_spans_from_reference_travel(root, input,
      reference, left_modifiers, right_modifiers, spans);
}

Span *
SpanFactory::create_gold_spans_from_reference_travel(int now,
    const dependency_t* input,
    const dependency_t* reference,
    const std::vector<edges_t >& left_modifiers_tree,
    const std::vector<edges_t >& right_modifiers_tree,
    SpanSet& spans) {
  std::vector<Span *> left_complete_subspans;
  std::vector<Span *> right_complete_subspans;

  const edges_t& left_modifiers= left_modifiers_tree[now];
  const edges_t& right_modifiers= right_modifiers_tree[now];

  // Get all the left modifier spans.
  for (int i = left_modifiers.size() - 1; i >= 0; -- i) {
    int next= left_modifiers[i];
    Span* subspan = create_gold_spans_from_reference_travel(next, input,
        reference, left_modifiers_tree, right_modifiers_tree, spans);
    left_complete_subspans.push_back(subspan);
  }

  // Get all the right modifier spans.
  for (int i = 0; i < right_modifiers.size(); ++ i) {
    int next= right_modifiers[i];
    Span* subspan = create_gold_spans_from_reference_travel(next, input,
        reference, left_modifiers_tree, right_modifiers_tree, spans);
    right_complete_subspans.push_back(subspan);
  }

  postag_t tag = reference->postag(now);

  // Build up the root span.
  Span* root_span = allocator->malloc();
  root_span->init(input, now, tag);
  spans.insert(root_span);

  Span* left_incomplete_span  = allocator->malloc();
  Span* right_incomplete_span = allocator->malloc();
  Span* complete_span = NULL;
  Span* new_span = NULL;

  for (int i = -1; i < (int)left_complete_subspans.size(); ++ i) {
    for (int j = -1; j < (int)right_complete_subspans.size(); ++ j) {

      if (i >= 0 || j >= 0) {
        new_span = allocator->malloc();
        new_span->copy(root_span);
        if (i >= 0) {
          for (int ii = 0; ii <= i; ++ ii) {
            new_span->link_left(left_complete_subspans[ii]);
          }
        }
        if (j >= 0) {
          for (int jj = 0; jj <= j; ++ jj) {
            new_span->link_right(right_complete_subspans[jj]);
          }
        }
      } else {
        new_span = root_span;
      }
      spans.insert(new_span);
      BOOST_LOG_TRIVIAL(trace) << "SpanFactory(create gold spans): " << (*new_span);
    }
  }

  return new_span;
}

bool
SpanFactory::legal_to_left_combine(const Span* head,
    const Span* modifier) const {
  if (!((head->used_words & modifier->used_words).none())) {
    return false;
  }
  return true;
}

bool
SpanFactory::legal_to_right_combine(const Span* head,
    const Span* modifier) const {
  if (!((head->used_words & modifier->used_words).none())) {
    return false;
  }
  return true;
}

} //  end for namespace BestFirst

} //  end for namespace ZGen
