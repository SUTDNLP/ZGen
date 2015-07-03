#ifndef __ZGEN_SHIFTREDUCE_TYPES_ACTION_H__
#define __ZGEN_SHIFTREDUCE_TYPES_ACTION_H__

#include <iostream>
#include "types/instance.h"
#include "shiftreduce/settings.h"
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZGen {

namespace ShiftReduce {

class Action {
public:
  enum {
    kNone = 0,
    kShift,
    kLeftArc,
    kRightArc
  };

  Action();
  Action(int _name, int _tag, int _word, int _index);

  inline std::size_t hash() const { return seed; }
  inline int name() const { return rep & 0x07; }
  inline int tag()  const { return rep >> 3; }

  bool operator == (const Action& a) const { return (a.rep == rep && a.word == word); }
  bool operator != (const Action& a) const { return !((*this) == a); }

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned) { ar & rep & word & seed; }

  friend std::ostream & operator << (std::ostream& os, const Action& act);

  friend std::size_t hash_value(const Action& a) { return a.seed; }

  int rep;    //! The lowest 3 bits are used to store the action name
              //! (SHIFT, LEFT-ARC, RIGHT-ARC), while the rest bits are
              //! used to store the tag  field. It represent PoSTag
              //! (on SHIFT) and dependency relation (on LEFT-ARC and RIGHT-ARC)
  int word;   //! The word.
  int index;  //! extra field for auxilary in the SHIFT action.

  std::size_t seed;
};

typedef Action action_t;

class ActionFactory {
public:
  static Action make_shift(int tag, int word, int index);
  static Action make_left_arc();
  static Action make_right_arc();
  static Action make_left_arc(int label);
  static Action make_right_arc(int label);
};

} //  namespace shiftreduce
} //  namespace zgen

#endif  //  end for __ZGEN_SHIFTREDUCE_TYPES_ACTION_H__
