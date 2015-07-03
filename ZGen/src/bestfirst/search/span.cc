#include "bestfirst/search/span.h"
#include "settings.h"

namespace ZGen {

namespace BestFirst {

Span::Span()
  : initialized(false) {
}


void
Span::init(const dependency_t* _ref,
    int _root_id,
    postag_t _tag) {
  initialized = true;
  ref         = _ref;
  root_id     = _root_id;
  tag         = _tag;
  used_words.set(root_id);
}

void
Span::copy(const Span* other) {
  initialized= true;
  ref        = other->ref;
  root_id    = other->root_id;
  tag        = other->tag;
  used_words = other->used_words;
  left_spans = other->left_spans;
  right_spans= other->right_spans;
}

void
Span::clear() {
  initialized = false;
  ref         = NULL;
  root_id     = -1;
  tag         = 0;
  score       = -inf;
  left_spans.clear();
  right_spans.clear();
  used_words.reset();
}

bool
Span::complete() {
  int len = ref->size();
  for (int i = 0; i < len; ++ i) {
    if (!used_words[i]) {
      return false;
    }
  }
  return true;
}

bool
Span::link_left(const Span* modifier) {
  if (!((used_words & modifier->used_words).none())) {
    return false;
  }

  left_spans.push_back(modifier);
  used_words |= modifier->used_words;
  return true;
}

bool
Span::link_right(const Span* modifier) {
  // If some word is used by this span and the candidate modifier.
  if (!((used_words & modifier->used_words).none())) {
    return false;
  }

  right_spans.push_back(modifier);
  used_words |= modifier->used_words;
  return true;
}

void
Span::repr(std::string& s) const {
  namespace eg = ZGen::Engine;

  s.append("(");
  for (int i = left_spans.size()- 1; i >= 0; -- i) {
    left_spans[i]->repr(s);
  }
  int id = root_id;
  word_t word= ref->form(id);

  const char *b = eg::WordEngine::get_const_instance().decode(word);
  if (!b) { s.append("-NONE-"); } else { s.append(b); }
  s.append("|");
  s.append(eg::PostagEngine::get_const_instance().decode(tag));
  for (int i = 0; i < right_spans.size(); ++ i) {
    right_spans[i]->repr(s);
  }
  s.append(")");
}

std::ostream& operator << (std::ostream& os, const Span& span) {
  std::string payload;
  payload.reserve(512);
  span.repr(payload);
  os << payload;
  return os;
}

} //  end for namespace Bestfirst

} //  end for namespace ZGen

