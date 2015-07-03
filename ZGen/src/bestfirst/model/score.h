#ifndef __ZGEN_BESTFIRST_MODEL_SCORE_H___
#define __ZGEN_BESTFIRST_MODEL_SCORE_H___

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZGen {

namespace BestFirst {

typedef int UnigramScore;

struct BigramScore {
  BigramScore() {}

  BigramScore(int feat1, int feat2)
    : payload(feat1, feat2) {
  }

  bool operator == (const BigramScore & a) const {
    return (a.payload.get<0>() == payload.get<0>() &&
        a.payload.get<1>() == payload.get<1>());
  }

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & payload.get<0>() & payload.get<1>();
  }

  friend std::size_t hash_value(const BigramScore & m) {
    std::size_t seed = 0;
    boost::hash_combine(seed, m.payload.get<0>());
    boost::hash_combine(seed, m.payload.get<1>());
    return seed;
  }

  boost::tuples::tuple<int, int> payload;
};

struct TrigramScore {
  TrigramScore() {}

  TrigramScore(int feat0, int feat1, int feat2)
    : payload(feat0, feat1, feat2) {
    }

  bool operator == (const TrigramScore & a) const {
    return (a.payload.get<0>() == payload.get<0>() &&
        a.payload.get<1>() == payload.get<1>() &&
        a.payload.get<2>() == payload.get<2>());
  }

  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned version) {
    ar & payload.get<0>() & payload.get<1>() & payload.get<2>();
  }

  friend std::size_t hash_value(const TrigramScore & m) {
    std::size_t seed = 0;
    boost::hash_combine(seed, m.payload.get<0>());
    boost::hash_combine(seed, m.payload.get<1>());
    boost::hash_combine(seed, m.payload.get<2>());
    return seed;
  }

  boost::tuples::tuple<int, int, int> payload;
};

typedef UnigramScore us_t;
typedef BigramScore  bs_t;
typedef TrigramScore ts_t;

} //  end for namespace BestFirst
} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_MODEL_SCORE_H___
