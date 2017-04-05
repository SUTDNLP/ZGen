#include "shiftreduce/types/action.h"

namespace ZGen {
namespace ShiftReduce {

Action::Action(): rep(0), word(0), index(-1), seed(0), prefix_type(PrefixType::kNone){}

Action::Action(int _name, int _tag, int _word, int _index)
  : rep(_tag << 3 | _name), word(_word), index(_index), seed(0), prefix_type(PrefixType::kNone){
  boost::hash_combine(seed, rep);
  boost::hash_combine(seed, word);
}

Action::Action(int _name, int _tag, int _word, int _index, PrefixType _prefix_type)
  : rep(_tag << 3 | _name), word(_word), index(_index), seed(0), prefix_type(_prefix_type) {
  boost::hash_combine(seed, rep);
  boost::hash_combine(seed, word);
}

std::ostream & operator << (std::ostream& os, const Action& act) {
  if (act.name() == Action::kNone) {
    os << "NO";
  } else if (act.name() == Action::kShift) {
    os << "SH-" << act.tag() << "-" << act.word << "(" << act.index << ")";
  } else if (act.name() == Action::kLeftArc) {
    os << "LA-" << act.tag();
  } else if (act.name() == Action::kRightArc) {
    os << "RA-" << act.tag();
  } else if (act.name() == Action::kInsert) {
	os << "IS";
  } else if (act.name() == Action::kSplitArc) {
	os << "SA-" << act.tag() << "-" << act.word;
  } else if (act.name() == Action::kIdle) {
		os << "Idle";
  } else {
    os << "Unrecognized action.";
  }
  return os;
}

Action ActionFactory::make_shift(int tag, int word, int index, PrefixType prefix_type) {
  return Action(Action::kShift, tag, word, index, prefix_type);
}

Action ActionFactory::make_left_arc(int label) {
  return Action(Action::kLeftArc, label, 0, -1);
}

Action ActionFactory::make_left_arc() {
  return Action(Action::kLeftArc, 0, 0, -1);
}

Action ActionFactory::make_right_arc(int label) {
  return Action(Action::kRightArc, label, 0, -1);
}

Action ActionFactory::make_right_arc() {
  return Action(Action::kRightArc, 0, 0, -1);
}

Action ActionFactory::make_insert(){
	return Action(Action::kInsert, 0, 0, -1);
}

Action ActionFactory::make_insert(int index){
	return Action(Action::kInsert, 0, 0, index);
}

Action ActionFactory::make_split_arc(int tag, int word){
	return Action(Action::kSplitArc, tag, word, -1);
}

Action ActionFactory::make_idle(){
	return Action(Action::kIdle, 0, 0, -1);
}

} //  namespace shiftreduce
} //  namespace zge
