#ifndef __ZGEN_BESTFIRST_MODEL_SCORE_CONTEXT_H__
#define __ZGEN_BESTFIRST_MODEL_SCORE_CONTEXT_H__

#include <vector>
#include "settings.h"
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_utils.h"

namespace ZGen {

namespace BestFirst {

struct ScoreContext {
  ScoreContext(const Span& span) {
    int id = span.root_id;

    ref = span.ref;
    word = span.ref->forms.at(id);
    tag = span.tag;

    SpanUtils::get_all_linearized_words(&span, words);
    SpanUtils::get_all_linearized_postags(&span, postags);
    SpanUtils::get_all_relations(&span, relations);
  }

  const dependency_t* ref;
  word_t  word;                   /*< The form of the current span */
  postag_t  tag;                  /*< The postag of the current span */
  std::vector<word_t> words;      /*< The linearized words */
  std::vector<postag_t> postags;  /*< The linearized postags */
  std::vector<word_t> head_words; /*< The head word of the span */
  //!
  std::vector<SpanUtils::relation_t> relations;
};

typedef ScoreContext ctx_t;

} //  end for namespace BestFirst

} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_MODEL_SCORE_CONTEXT_H__
