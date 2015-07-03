#define private public
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_allocator.h"
#include <boost/test/unit_test.hpp>

// Used to test if the span allocator functions well.
BOOST_AUTO_TEST_CASE( span_allocator ) {
  namespace z = ZGen;
  namespace bf = ZGen::BestFirst;
  bf::SpanAllocator allocator(3, 2);
  bf::Span* span;
  bf::Span* span2;

  BOOST_CHECK_EQUAL( 2, allocator.bucket_size );
  BOOST_CHECK_EQUAL( 0, allocator.num_spans );
  BOOST_CHECK_EQUAL( 0, allocator.num_buckets );
  BOOST_CHECK_EQUAL( 2, allocator.cap_buckets );

  span = allocator.malloc();
  BOOST_CHECK_EQUAL( span, &allocator.buckets[0][0] );

  span2 = span = allocator.malloc();
  BOOST_CHECK_EQUAL( span, &allocator.buckets[0][1] );

  span = allocator.malloc();
  BOOST_CHECK_EQUAL( span, &allocator.buckets[1][0] );
  BOOST_CHECK_EQUAL( 3, allocator.num_spans );
  BOOST_CHECK_EQUAL( 1, allocator.num_buckets );
  BOOST_CHECK_EQUAL( 2, allocator.cap_buckets );

  span = allocator.malloc();
  BOOST_CHECK_EQUAL( span, &allocator.buckets[1][1] );
  BOOST_CHECK_EQUAL( 4, allocator.num_spans );
  BOOST_CHECK_EQUAL( 2, allocator.num_buckets );
  BOOST_CHECK_EQUAL( 4, allocator.cap_buckets );

  span = allocator.malloc();
  BOOST_CHECK_EQUAL( span, &allocator.buckets[2][0] );
  BOOST_CHECK_EQUAL( span2, &allocator.buckets[0][1] );
}


