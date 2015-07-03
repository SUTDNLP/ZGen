#define private public
#include "engine/engine.h"
#include "utils/ioutils.h"
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_allocator.h"
#include "unittest/bestfirst/fixture2.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( span_construct_test ) {
  namespace bf = ZGen::BestFirst;
  namespace eg = ZGen::Engine;

  Fixture2 f;
  BOOST_CHECK_EQUAL(f.span_0->tag, eg::PennTreebankPostagAlphabet::NP);
}

BOOST_AUTO_TEST_CASE( span_link_test ) {
  namespace bf = ZGen::BestFirst;

  Fixture2 f;
  bf::Span* span_0 = f.span_0;
  bf::Span* span_2 = f.span_2;
  bf::Span* span_3 = f.span_3;

  bf::Span* span_0_1 = f.span_0_1;
  BOOST_CHECK_EQUAL(span_0_1->used_words[0], true);
  BOOST_CHECK_EQUAL(span_0_1->used_words[1], true);
  BOOST_CHECK_EQUAL(span_0_1->used_words[2], false);
  BOOST_CHECK_EQUAL(span_0_1->left_spans[0], span_0);

  bf::Span* span_0_1_2 = f.span_0_1_2;
  BOOST_CHECK_EQUAL(span_0_1_2->used_words[0], true);
  BOOST_CHECK_EQUAL(span_0_1_2->used_words[1], true);
  BOOST_CHECK_EQUAL(span_0_1_2->used_words[2], true);
  BOOST_CHECK_EQUAL(span_0_1_2->used_words[3], false);
  BOOST_CHECK_EQUAL(span_0_1_2->left_spans[0], span_0);
  BOOST_CHECK_EQUAL(span_0_1_2->right_spans[0], span_2);

  bf::Span* span_0_1_2_3 = f.span_0_1_2_3;
  BOOST_CHECK_EQUAL(span_0_1_2_3->used_words[0], true);
  BOOST_CHECK_EQUAL(span_0_1_2_3->used_words[1], true);
  BOOST_CHECK_EQUAL(span_0_1_2_3->used_words[2], true);
  BOOST_CHECK_EQUAL(span_0_1_2_3->used_words[3], true);
  BOOST_CHECK_EQUAL(span_0_1_2_3->left_spans[0], span_0);
  BOOST_CHECK_EQUAL(span_0_1_2_3->right_spans[0], span_2);
  BOOST_CHECK_EQUAL(span_0_1_2_3->right_spans[1], span_3);
}

