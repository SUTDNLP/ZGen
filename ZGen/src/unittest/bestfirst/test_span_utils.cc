#define private public
#include "settings.h"
#include "types/instance.h"
#include "utils/ioutils.h"
#include "bestfirst/search/span_utils.h"
#include "bestfirst/search/span_allocator.h"
#include "unittest/bestfirst/fixture1.h"
#include <boost/test/unit_test.hpp>

static bool in(int head, int modifier,
    const std::vector<std::pair<int, int> >& relations) {
  for (int i = 0; i < relations.size(); ++ i) {
    if (head == relations[i].first && modifier == relations[i].second) {
      return true;
    }
  }
  return false;
}

BOOST_AUTO_TEST_CASE( span_utils_1 ) {
  namespace bf = ZGen::BestFirst;
  Fixture1 f;
  std::vector<bf::SpanUtils::relation_t> relations;

  bf::SpanUtils::get_all_relations(f.root, relations);

  BOOST_CHECK_EQUAL(in(4, 0, relations), true);
  BOOST_CHECK_EQUAL(in(2, 3, relations), true);
  BOOST_CHECK_EQUAL(in(1, 3, relations), false);
}

BOOST_AUTO_TEST_CASE( span_utils_2 ) {
  namespace z = ZGen;
  namespace bf = ZGen::BestFirst;
  namespace eg = ZGen::Engine;
  Fixture1 f;

  std::vector<z::word_t> words;
  bf::SpanUtils::get_all_linearized_words(f.root, words);

#define ENGINE_ENCODE(x) eg::WordEngine::get_const_instance().encode(x)
  BOOST_CHECK_EQUAL(words.size(), 10);
  BOOST_CHECK_EQUAL(words[0],     ENGINE_ENCODE("00"));
  BOOST_CHECK_EQUAL(words[1],     ENGINE_ENCODE("01"));
  BOOST_CHECK_EQUAL(words.back(), ENGINE_ENCODE("8"));
#undef ENGINE_ENCODE
}

BOOST_AUTO_TEST_CASE( span_utils_3 ) {
  namespace z = ZGen;
  namespace bf = ZGen::BestFirst;
  namespace eg = ZGen::Engine;

  Fixture1 f;
  std::vector<z::postag_t> postags;
  bf::SpanUtils::get_all_linearized_postags(f.root, postags);

#define ENGINE_ENCODE(x) eg::PostagEngine::get_const_instance().encode(x)
  BOOST_CHECK_EQUAL(postags.size(), 9);
  BOOST_CHECK_EQUAL(postags[0], ENGINE_ENCODE("NP"));
  BOOST_CHECK_EQUAL(postags[1], ENGINE_ENCODE("NP"));
  BOOST_CHECK_EQUAL(postags[2], ENGINE_ENCODE("VBZ"));
#undef ENGINE_ENCODE
}

