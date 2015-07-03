#include "bestfirst/search/span.h"
#include "bestfirst/search/span_allocator.h"

namespace ZGen {

namespace BestFirst {

SpanAllocator::SpanAllocator(int _reserved_num_spans,
    int _bucket_size)
  : num_spans(0),
  bucket_size(_bucket_size) {
  // The capcity of buckets.
  cap_buckets = (_reserved_num_spans / bucket_size + 1);

  // create cap_buckets buckets.
  buckets = new Span*[cap_buckets];
  for (int i = 0; i < cap_buckets; ++ i) {
    buckets[i] = new Span[_bucket_size];
  }

  // set the next valid span to the very begining
  // of the allocator.
  num_buckets = 0;
  next_valid_span = buckets[0];
  // The number of spans that vailable in bucket size.
  num_spans_left_in_bucket = bucket_size;
}

Span*
SpanAllocator::malloc() {
  Span* current_valid_span = next_valid_span;

  -- num_spans_left_in_bucket;

  if (num_spans_left_in_bucket == 0) {
    // need to move to next buckets.
    ++ num_buckets;
    if (num_buckets >= cap_buckets) {
      cap_buckets = num_buckets * 2;
      Span** new_buckets = new Span*[cap_buckets];
      std::copy(buckets, buckets + num_buckets, new_buckets);
      for (int i = num_buckets; i < cap_buckets; ++ i) {
        new_buckets[i] = new Span[bucket_size];
      }
      delete[] buckets;
      buckets = new_buckets;
    }

    next_valid_span = buckets[num_buckets];
    num_spans_left_in_bucket = bucket_size;
  } else {
    ++ next_valid_span;
  }
  ++ num_spans;
  return current_valid_span;
}

void
SpanAllocator::clear() {
  for (int i = 0; i < cap_buckets; ++ i) {
    for (int j = 0; j < bucket_size; ++ j) {
      buckets[i][j].clear();
    }
  }

  num_buckets= 0;
  num_spans = 0;
  num_spans_left_in_bucket = bucket_size;
  next_valid_span = &buckets[0][0];
}

void
SpanAllocator::pruge() {
  for (int i = 0; i < cap_buckets; ++ i) {
    delete[] buckets[i];
  }
  delete[] (buckets);

  bucket_size = 0;
  num_buckets = 0;
  cap_buckets = 0;
  num_spans = 0;
  next_valid_span  =NULL;
}


} //  end for BestFirst

} //  end for namespace ZGen 
