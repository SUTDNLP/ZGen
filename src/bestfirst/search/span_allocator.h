#ifndef __ZGEN_BESTFIRST_SPAN_POOL_H__
#define __ZGEN_BESTFIRST_SPAN_POOL_H__

#include "bestfirst/search/span.h"

namespace ZGen {

namespace BestFirst {

//! Span allocator is a stacked-list for allocating memory for spans.
//! The following graph illustrate the general structure for SpanAllocator.
//!
//!  buckets
//!    |
//!    v
//!  [ 0 ] -> [ 1 ] -> [ 2 ] ... [B-1]
//!    |
//!    v
//!  [ B ] -> [B+1] -> [B+2] ... [2*B-1]
//!    |
//!    v
//!   ...
//! Each time there is not enough memory for spans in buckets, the buckets
//! array, which is the header array, is reallocated, but the stacked-list
//! is not reallocated. It guarentee that the allocated span's address is
//! not modified.
class SpanAllocator {
public:
  /**
   * Initialize the span allocator with the total number of
   * spans, the size of each buckets.
   *
   *  @param[in]  num_spans   The total number of spans in the allocator.
   *  @param[in]  bucket_size The number of elements in each buckets.
   */
  SpanAllocator(int reserved_num_spans= 1,
      int bucket_size=65535);

  /**
   * Malloc a Span instance.
   *
   *  @return Span*   The Span instance.
   */
  Span* malloc();

  /**
   * Batchly clear all the spans in the Pool. The
   */
  void clear();

  /**
   * Clear all the instance in pool.
   */
  void pruge();

  /**
   * Print the allocator information onto the stream, [Not Impl].
   *
   *  @param[in/out]  os
   *  @param[in]      allocator
   */
  friend std::ostream& operator << (std::ostream& os, const SpanAllocator& allocator);
private:
  Span** buckets;   /*< The real memory for storing the spans.*/

  Span* next_valid_span;

  int bucket_size;  /*< The size of buckets */
  int num_buckets;  /*< The number of buckets. */
  int cap_buckets;  /*< The capcity of buckets. */
  int num_spans_left_in_bucket; 
  int num_spans;    /*< The total number of spans in buckets */
};

}

}

#endif  //  end for __ZGEN_BESTFIRST_SPAN_POOL_H__
