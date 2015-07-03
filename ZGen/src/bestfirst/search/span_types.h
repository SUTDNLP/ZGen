#ifndef __ZGEN_BESTFIRST_SEARCH_SPAN_TYPES_H__
#define __ZGEN_BESTFIRST_SEARCH_SPAN_TYPES_H__

#include <boost/unordered_set.hpp>
#include <boost/heap/priority_queue.hpp>
#include "bestfirst/search/span.h"

namespace ZGen {

namespace BestFirst {

typedef boost::unordered_set<Span *, SpanPtrHash, SpanPtrEqual> SpanSet;

typedef boost::heap::priority_queue<Span *, boost::heap::compare<SpanPtrLess> > SpanMaxHeap;

typedef boost::heap::priority_queue<Span *, boost::heap::compare<SpanPtrGreater> > SpanMinHeap;

class SpanKMaxHeap: public SpanMinHeap {
public:
  SpanKMaxHeap(){
  }

  void init(int volumn) {
    K = volumn;
    SpanMinHeap::clear();
  }

  bool insert(Span *span) {
    if (SpanMinHeap::size() == K) {
      if (span->score > SpanMinHeap::top()->score) {
        SpanMinHeap::pop();
        SpanMinHeap::push(span);
      }
      return false;
    }
    SpanMinHeap::push(span);
    return true;
  }
private:
  int K;
};

} //  end for namespace Bestfirst

} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_SEARCH_SPAN_TYPES_H__
