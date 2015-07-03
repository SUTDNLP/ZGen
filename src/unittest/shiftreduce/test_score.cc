#define private public
#include "shiftreduce/model/score.h"
#include <iostream>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( action_score ) {
  namespace sr = ZGen::ShiftReduce;
  typedef std::unordered_map<
    sr::UnigramMetaFeature,
    int,
    boost::hash<sr::UnigramMetaFeature>
  > entry_t;

  entry_t entry;
  entry[sr::UnigramMetaFeature(10)] = 9;

  for (entry_t::const_iterator itx = entry.begin(); itx != entry.end(); ++ itx) {
    std::cout << itx->first << ", " << itx->second << std::endl;
  }
  BOOST_CHECK_EQUAL( entry[sr::UnigramMetaFeature(10)], 9);
}


