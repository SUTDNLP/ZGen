#ifndef __ZGEN_SHIFTREDUCE_MODEL_SCORE_H__
#define __ZGEN_SHIFTREDUCE_MODEL_SCORE_H__

#include "settings.h"
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/access.hpp>

namespace ZGen {
namespace ShiftReduce {

class AbstractMetaFeature {
public:
  AbstractMetaFeature(): seed(0) {}
  friend std::size_t hash_value(const AbstractMetaFeature& s) { return s.seed; }
protected:
  friend class boost::serialization::access;
  std::size_t seed;
};

// Unigram
struct UnigramMetaFeature: public AbstractMetaFeature {
public:
  UnigramMetaFeature(): rep(0) {}
  UnigramMetaFeature(int f): rep(f) { seed = f; }

  bool operator == (const UnigramMetaFeature& a) const { return rep == a.rep; }
  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & rep;
  }

  friend std::ostream & operator << (std::ostream& os, const UnigramMetaFeature& s) {
    os << s.rep << "-" << s.seed; return os;
  }
protected:
  int rep;
};

// Bigram
struct BigramMetaFeature: public AbstractMetaFeature {
public:
  BigramMetaFeature(): rep(0, 0) {}
  BigramMetaFeature(int f1, int f2): rep(f1, f2) {
    boost::hash_combine(seed, f1);
    boost::hash_combine(seed, f2);
  }

  bool operator == (const BigramMetaFeature& a) const { return rep == a.rep; }
  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & rep.get<0>() & rep.get<1>();
  }

  friend std::ostream & operator << (std::ostream& os, const BigramMetaFeature& s) {
    os << s.rep << "-" << s.seed;  return os;
  }
protected:
  boost::tuples::tuple<int, int> rep;
};

// Trigram
struct TrigramMetaFeature: public AbstractMetaFeature {
public:
  TrigramMetaFeature(): rep(0, 0, 0) {}
  TrigramMetaFeature(int f0, int f1, int f2): rep(f0, f1, f2) {
    boost::hash_combine(seed, f0);
    boost::hash_combine(seed, f1);
    boost::hash_combine(seed, f2);
  }

  bool operator == (const TrigramMetaFeature& a) const { return rep == a.rep; }

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & seed & rep.get<0>() & rep.get<1>() & rep.get<2>();
  }

  friend std::ostream & operator << (std::ostream& os, const TrigramMetaFeature& s) {
    os << s.rep << "-" << s.seed;  return os;
  }
protected:
  boost::tuples::tuple<int, int, int> rep;
};

typedef UnigramMetaFeature  us_t;
typedef BigramMetaFeature   bs_t;
typedef TrigramMetaFeature  ts_t;

} //  namespace shiftreduce
} //  namespace zgen

#endif  //  end for __ZGEN_SHIFTREDUCE_MODEL_SCORE_H__
