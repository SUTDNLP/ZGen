#define private public
#include "engine/engine.h"
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_types.h"
#include "unittest/bestfirst/fixture2.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( span_types1 ) {
  namespace z = ZGen;
  namespace bf = ZGen::BestFirst;

  Fixture2 f;

  f.span_0->score = 0;
  f.span_1->score = 1;
  f.span_2->score = 2;
  f.span_3->score = 3;

  bf::SpanMaxHeap max_heap;
  max_heap.push(f.span_0);
  max_heap.push(f.span_1);
  max_heap.push(f.span_2);
  max_heap.push(f.span_3);

  bf::Span* span = NULL;
  span = max_heap.top(); max_heap.pop(); BOOST_CHECK_EQUAL(span, f.span_3);
  span = max_heap.top(); max_heap.pop(); BOOST_CHECK_EQUAL(span, f.span_2);
  span = max_heap.top(); max_heap.pop(); BOOST_CHECK_EQUAL(span, f.span_1);
  span = max_heap.top(); max_heap.pop(); BOOST_CHECK_EQUAL(span, f.span_0);
  BOOST_CHECK_EQUAL(max_heap.empty(), true);

  bf::SpanMinHeap min_heap;
  min_heap.push(f.span_0);
  min_heap.push(f.span_1);
  min_heap.push(f.span_2);
  min_heap.push(f.span_3);

  span = min_heap.top(); min_heap.pop(); BOOST_CHECK_EQUAL(span, f.span_0);
  span = min_heap.top(); min_heap.pop(); BOOST_CHECK_EQUAL(span, f.span_1);
}

BOOST_AUTO_TEST_CASE( span_types2 ) {
  namespace bf = ZGen::BestFirst;
  Fixture2 f;

  bf::SpanSet st;
  st.insert(f.span_0);
  st.insert(f.span_1);
  st.insert(f.span_2);

  BOOST_CHECK_EQUAL(st.find(f.span_0) != st.end(), true);
  BOOST_CHECK_EQUAL(st.find(f.span_3) == st.end(), true);
}

BOOST_AUTO_TEST_CASE( span_types3 ) {
  namespace bf = ZGen::BestFirst;
  Fixture2 f;

  bf::SpanKMaxHeap kmax_heap;
  kmax_heap.init(3);
  kmax_heap.insert(f.span_0);
  kmax_heap.insert(f.span_1);
  kmax_heap.insert(f.span_2);
  kmax_heap.insert(f.span_3);

  bf::Span* span;
  BOOST_CHECK_EQUAL(kmax_heap.size(), 3);
  span = kmax_heap.top(); kmax_heap.pop(); BOOST_CHECK_EQUAL(span, f.span_1);
}
