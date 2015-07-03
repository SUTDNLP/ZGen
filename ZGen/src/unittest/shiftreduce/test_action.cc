#define private public
#include "shiftreduce/types/action.h"
#include <boost/test/unit_test.hpp>
#include <boost/functional/hash.hpp>

BOOST_AUTO_TEST_CASE( action_equal ) {
  namespace sr = ZGen::ShiftReduce;
  BOOST_CHECK_EQUAL(
      sr::Action(sr::Action::kShift, 0, 0, 1),
      sr::Action(sr::Action::kShift, 0, 0, 2)
      );
}

BOOST_AUTO_TEST_CASE( action_not_equal ) {
  namespace sr = ZGen::ShiftReduce;
  BOOST_CHECK_EQUAL(
      sr::Action(sr::Action::kRightArc, 0, 0, 1),
      sr::Action(sr::Action::kShift, 0, 0, 2)
      );
}

BOOST_AUTO_TEST_CASE( action_hash_equal ) {
  namespace sr = ZGen::ShiftReduce;
  BOOST_CHECK_EQUAL(
      boost::hash<sr::Action>()(sr::Action(sr::Action::kShift, 0, 0, 1)),
      boost::hash<sr::Action>()(sr::Action(sr::Action::kShift, 0, 0, 2))
      );
}

BOOST_AUTO_TEST_CASE( action_hash_combine_equal ) {
  namespace sr = ZGen::ShiftReduce;
  size_t seed1 = 0;
  boost::hash_combine(seed1, 10);
  boost::hash_combine(seed1, sr::Action(sr::Action::kShift, 0, 0, 1));

  size_t seed2 = 0;
  boost::hash_combine(seed2, 10);
  boost::hash_combine(seed2, sr::Action(sr::Action::kShift, 0, 0, 2));

  BOOST_CHECK_EQUAL(seed1, seed2);
}


