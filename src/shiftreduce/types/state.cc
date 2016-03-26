#include <cstring>
#include "utils/logging.h"
#include "engine/token_alphabet.h"
#include "shiftreduce/types/state.h"
#include "shiftreduce/types/action.h"

namespace ZGen {
namespace ShiftReduce {

StateItem::StateItem(): ref(0) { clear(); }
StateItem::StateItem(const dependency_t* _ref): ref(_ref) { clear();}

std::ostream& operator << (std::ostream& os, const StateItem& item) {
  os << "ADDRESS:   " << (void *)(&item)  << std::endl;
  os << "PREV ADDR: " << (void *)(&item)  << std::endl;
  os << "SCORE:     " << item.score       << std::endl;
  return os;
}

bool StateItem::operator < (const StateItem& other) const { return score < other.score; }
bool StateItem::operator > (const StateItem& other) const { return score > other.score; }
void StateItem::set_reference(const dependency_t* _ref) { ref = _ref; }
void StateItem::set_graph(const graph_t* _graph){
	graph = _graph;
}
void StateItem::set_forms_alphabet(const Engine::TokenAlphabet* forms_alphabet){
	this->forms_alphabet = forms_alphabet;
}
void StateItem::set_deprels_alphabet(const Engine::TokenAlphabet* deprels_alphabet){
	this->deprels_alphabet = deprels_alphabet;
}
void StateItem::set_pos_alphabet(const Engine::TokenAlphabet* pos_alphabet){
	this->pos_alphabet = pos_alphabet;
}
void
StateItem::clear() {
  score = 0;
  previous = 0;

  top0 = -1;
  top1 = -1;
  // left_arced.reset();

  buffer.set();         // fill all words in buffer
  stack.clear();        // clear words in stack
  stack.push_back(-1);  // Push a BEGIN symbol onto the stack.
  words_shifted.clear();

  // Initialize the word sequence and postag sequence.
  word_sequence.clear();
  word_sequence.push_back(Engine::TokenAlphabet::BEGIN);
  postag_sequence.clear();
  postag_sequence.push_back(Engine::TokenAlphabet::BEGIN);
//  graph->

  //
  memset(rank, 0, sizeof(rank));
  memset(heads, -1, sizeof(heads));     // Set up heads to no-head
  memset(deprels, 0, sizeof(deprels));  // Set up deprel to no-relation
  memset(postags, 0, sizeof(postags));  // Set up postags to no-postags
  // Set up number of left- and right-children to zero.
  memset(nr_left_children, 0, sizeof(nr_left_children));
  memset(nr_right_children, 0, sizeof(nr_right_children));
  memset(nr_left_descendant, 0, sizeof(nr_left_descendant));
  memset(nr_right_descendant, 0, sizeof(nr_right_descendant));

  // Set up the left-most and right-most child
  memset(left_most_child, -1, sizeof(left_most_child));
  memset(right_most_child, -1, sizeof(right_most_child));

  // Set up the left-2nd-most and right-2nd-most child
  memset(left_2nd_most_child, -1, sizeof(left_2nd_most_child));
  memset(right_2nd_most_child, -1, sizeof(right_2nd_most_child));
}


void
StateItem::copy(const StateItem & other) {
  ref = other.ref;
  top0 = other.top0; top1 = other.top1; 
  previous = other.previous;
  stack = other.stack;
  word_sequence = other.word_sequence;
  postag_sequence = other.postag_sequence;
  words_shifted = other.words_shifted;

  buffer = other.buffer;
  score = other.score;
  last_action = other.last_action;
  graph = other.graph;
  forms_alphabet = other.forms_alphabet;
  deprels_alphabet = other.deprels_alphabet;
  pos_alphabet = other.pos_alphabet;
  #define _COPY(name) memcpy((name), other.name, sizeof(name));
  _COPY(rank);
  _COPY(postags);
  _COPY(heads);
  _COPY(deprels);
  _COPY(left_most_child);
  _COPY(left_2nd_most_child);
  _COPY(right_most_child);
  _COPY(right_2nd_most_child);
  _COPY(nr_left_children);
  _COPY(nr_right_children);
  _COPY(nr_left_descendant);
  _COPY(nr_right_descendant);
  #undef _COPY
}


bool
StateItem::shift(postag_t label, word_t word, int index) {
  if (index < 0 || index >= ref->size()) { return false; }
  if (false == buffer.test(index))       { return false; }

  // Push the word onto the stack
  stack.push_back(index);
  top1 = top0;
  top0 = index;

  // Append the word and postag to the certain sequence.
  // Mainly handle the phrase.
  for (int i = ref->phrases.at(index).first;
      i < ref->phrases.at(index).second;
      ++ i) {
    word_sequence.push_back( ref->words.at(i) );
    postag_sequence.push_back( label );
  }

  buffer.set(index, 0);               // Erase this word.
  rank[index] = word_sequence.size(); //
  postags[index] = label;             // Set up the postag.
  words_shifted.push_back(index);
  return true;
}


bool
StateItem::shift(postag_t label, word_t word, int index,
    StateItem* other) const {
  other->copy((*this));
  bool ret = other->shift(label, word, index);
  if (ret) {
    other->previous = this;
    other->last_action = action_t(Action::kShift, label, word, index);
  } else {
    other->clear();
    _DEBUG << "state: Failed to perform SH transition.";
  }

  return ret;
}


bool
StateItem::left_arc(deprel_t deprel) {
  if (stack.size() < 3) {
    // less than two valid element and a pseudo element
    return false;
  }

  // Backup the stack top nodes
  int stack_size = stack.size();
  top0 = stack[stack_size - 1];
  top1 = stack[stack_size - 2];

  // Maintain the stack structure
  stack.pop_back();
  stack.back() = top0;

  // Link (top1 <- top0)
  heads[top1] = top0;
  deprels[top1] = deprel;

  // left_arced.set(top0, 1);

  // Main left child.
  if (left_most_child[top0] == -1) {

    // TP0 is left-isolate node
    left_most_child[top0] = top1;
  } else if (top1 < left_most_child[top0]) {

    // (TP1, LM0, TP0)
    left_2nd_most_child[top0] = left_most_child[top0];
    left_most_child[top0] = top1;
  } else if (top1 < left_2nd_most_child[top0]) {

    // (LM0, TP1, TP0)
    left_2nd_most_child[top0] = top1;
  }

  ++ nr_left_children[top0];
  nr_left_descendant[top0] += (nr_left_descendant[top1]
      + nr_right_descendant[top1]
      + (ref->phrases[top1].second - ref->phrases[top1].first));

  if (stack.size() <= 1) {
    top1 = -1;
  } else {
    top1 = stack[stack.size() - 2];
  }

  return true;
}

bool
StateItem::left_arc(deprel_t deprel, StateItem* other) const {
  other->copy((*this));
  bool ret = other->left_arc(deprel);

  if (ret) {
    other->previous = this;
    other->last_action = action_t(Action::kLeftArc, deprel, 0, -1);
  } else {
    other->clear();
  }

  return ret;
}

bool
StateItem::right_arc(deprel_t deprel) {
  if (stack.size() < 3) {
    return false;
  }

  //
  int stack_size = stack.size();

  top0 = stack[stack_size - 1];
  top1 = stack[stack_size - 2];

  // Maintain the stack structure
  stack.pop_back();

  heads[top0] = top1;
  deprels[top0] = deprel;

  // Maintain left child.
  if (right_most_child[top1] == -1) {
    // TP1 is right-isolate node
    right_most_child[top1] = top0;
  } else if (right_most_child[top1] < top0) {
    // TP1, LM1, TP0
    right_2nd_most_child[top1] = right_most_child[top1];
    right_most_child[top1] = top0;
  } else if (right_2nd_most_child[top1] < top0) {
    // TP1, TP0, LM1
    right_2nd_most_child[top1] = top0;
  }

  ++ nr_right_children[top1];
  nr_right_descendant[top1] += (nr_right_descendant[top0] 
      + nr_left_descendant[top0]
      + (ref->phrases[top0].second - ref->phrases[top0].first));

  top0 = top1;
  if (stack.size() <= 1) {
    top1 = -1;
  } else {
    top1 = stack[stack.size() - 2];
  }

  return true;
}

bool
StateItem::right_arc(deprel_t deprel, StateItem* other) const {
  other->copy((*this));
  bool ret = other->right_arc(deprel);

  if (ret) {
    other->previous = this;
    other->last_action = action_t(Action::kRightArc, deprel, 0, -1);
  } else {
    other->clear();
  }
  return ret;
}

} //  end for namespace ShiftReduce
} //  end for namespace zgen
