#include "bestfirst/search/span_utils.h"

namespace ZGen {

namespace BestFirst {

void
SpanUtils::get_all_relations(const Span* span,
    std::vector<relation_t>& relations) {
  get_all_relations_travel(span, relations);
}

void
SpanUtils::get_all_relations_travel(const Span* span,
    std::vector<relation_t>& relations) {
  int head_id = span->root_id;
  for (int i = 0; i < span->left_spans.size(); ++ i) {
    const Span* left_span = span->left_spans[i];
    int modifier_id = left_span->root_id;
    relations.push_back(relation_t(head_id, modifier_id));

    get_all_relations_travel(left_span, relations);
  }

  for (int i = 0; i < span->right_spans.size(); ++ i) {
    const Span* right_span = span->right_spans[i];
    int modifier_id = right_span->root_id;
    relations.push_back(relation_t(head_id, modifier_id));

    get_all_relations_travel(right_span, relations);
  }
}


void
SpanUtils::get_all_left_relations(const Span* span,
    std::vector<relation_t>& relations) {
  get_all_left_relations_travel(span, relations);
}

void
SpanUtils::get_all_left_relations_travel(const Span* span,
    std::vector<relation_t>& relations) {
  int head_id = span->root_id;
  for (int i = 0; i < span->left_spans.size(); ++ i) {
    const Span* left_span = span->left_spans[i];
    int modifier_id = left_span->root_id;
    relations.push_back(relation_t(head_id, modifier_id));
    get_all_left_relations_travel(left_span, relations);
  }

  for (int i = 0; i < span->right_spans.size(); ++ i) {
    const Span* right_span = span->right_spans[i];
    get_all_left_relations_travel(right_span, relations);
  }
}


void
SpanUtils::get_all_right_relations(const Span* span,
    std::vector<relation_t>& relations) {
  get_all_right_relations_travel(span, relations);
}

void
SpanUtils::get_all_right_relations_travel(const Span* span,
    std::vector<relation_t>& relations) {
  int head_id = span->root_id;
  for (int i = 0; i < span->left_spans.size(); ++ i) {
    const Span* left_span = span->left_spans[i];
    get_all_right_relations_travel(left_span, relations);
  }

  for (int i = 0; i < span->right_spans.size(); ++ i) {
    const Span* right_span = span->right_spans[i];
    int modifier_id = right_span->root_id;
    relations.push_back(relation_t(head_id, modifier_id));
    get_all_right_relations_travel(right_span, relations);
  }
}


void
SpanUtils::get_all_modifier_words(const Span* span,
    std::vector<word_t>& modifying_words) {
  modifying_words.clear();
  int head_id = span->root_id;
  for (int i = 0; i < span->left_spans.size(); ++ i) {
    const Span* left_span = span->left_spans[i];
    int modifier_id = left_span->root_id;
    modifying_words.push_back(span->ref->forms.at(modifier_id));
  }

  for (int i = 0; i < span->right_spans.size(); ++ i) {
    const Span* right_span = span->right_spans[i];
    int modifier_id = right_span->root_id;
    modifying_words.push_back(span->ref->forms.at(modifier_id));
  }
}

void
SpanUtils::get_all_modifier_postags(const Span* span,
    std::vector<postag_t>& modifying_postags) {
  modifying_postags.clear();
  int head_id = span->root_id;
  for (int i = 0; i < span->left_spans.size(); ++ i) {
    const Span* left_span = span->left_spans[i];
    modifying_postags.push_back(left_span->tag);
  }

  for (int i = 0; i < span->right_spans.size(); ++ i) {
    const Span* right_span = span->right_spans[i];
    modifying_postags.push_back(right_span->tag);
  }
}

void
SpanUtils::get_all_linearized_indices(const Span* span,
    std::vector<int>& indices) {
  get_all_linearized_indices_travel(span, indices);
}

void
SpanUtils::get_all_linearized_indices_travel(const Span* now,
    std::vector<int>& indices) {
  for (int i = now->left_spans.size() - 1; i >= 0; -- i) {
    const Span* next = now->left_spans[i];
    get_all_linearized_indices_travel(next, indices);
  }

  indices.push_back(now->root_id);

  for (int i = 0; i < now->right_spans.size(); ++ i) {
    const Span* next = now->right_spans[i];
    get_all_linearized_indices_travel(next, indices);
  }
}

void
SpanUtils::get_all_linearized_words(const Span* span,
    std::vector<word_t>& words) {
  std::vector<int> indices;
  get_all_linearized_indices(span, indices);

  for (int j = 0; j < indices.size(); ++ j) {
    int id = indices[j];
    const dependency_t* ref = span->ref;
    for (int i = ref->phrases.at(id).first;
        i < ref->phrases.at(id).second; ++ i) {
      words.push_back( ref->words.at(i) );
    }
  }
}


void
SpanUtils::get_all_linearized_postags(const Span* span,
    std::vector<postag_t>& postags) {
  std::vector<int> indices;
  get_all_linearized_indices_travel(span, indices);

  for (int j = 0; j < indices.size(); ++ j) {
    int id = indices[j];
    const dependency_t* ref = span->ref;
    postags.push_back(ref->postags.at(j));
  }
}

} //  end for namespace BestFirst
} //  end for namespace ZGen
