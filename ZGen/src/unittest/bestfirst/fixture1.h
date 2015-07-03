#ifndef __ZGEN_UNITTEST_BESTFIRST_FIXTURE_1_H__
#define __ZGEN_UNITTEST_BESTFIRST_FIXTURE_1_H__

#include "engine/engine.h"
#include "types/instance.h"
#include "utils/ioutils.h"
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_allocator.h"

struct Fixture1 {
  Fixture1()
    : allocator(127, 128) {
    namespace z = ZGen;
    namespace io = ZGen::IO;
    namespace bf = ZGen::BestFirst;

    std::stringstream S;
    S << "__00_01__ NP 4 N"   << std::endl
      << "1 NP 2 N"   << std::endl
      << "2 VBZ 4 N"  << std::endl
      << "3 NNS 2 N"  << std::endl
      << "4 VBZ -1 N" << std::endl
      << "5 JJ 6 N"   << std::endl
      << "6 NN 4 N"   << std::endl
      << "7 NP 4 N"   << std::endl
      << "8 CD 7 N"   << std::endl
      ;

    io::read_dependency_dataset(S, data);

    const z::dependency_t* inst = &data[0];
    bf::Span* span_0 = allocator.malloc(); span_0->init(inst, 0, inst->postag(0));
    bf::Span* span_1 = allocator.malloc(); span_1->init(inst, 1, inst->postag(1));
    bf::Span* span_2 = allocator.malloc(); span_2->init(inst, 2, inst->postag(2));
    bf::Span* span_3 = allocator.malloc(); span_3->init(inst, 3, inst->postag(3));
    bf::Span* span_4 = allocator.malloc(); span_4->init(inst, 4, inst->postag(4));
    bf::Span* span_5 = allocator.malloc(); span_5->init(inst, 5, inst->postag(5));
    bf::Span* span_6 = allocator.malloc(); span_6->init(inst, 6, inst->postag(6));
    bf::Span* span_7 = allocator.malloc(); span_7->init(inst, 7, inst->postag(7));
    bf::Span* span_8 = allocator.malloc(); span_8->init(inst, 8, inst->postag(8));

    bf::Span* span_1_2_3 = allocator.malloc();
      span_1_2_3->copy(span_2);
        span_1_2_3->link_left(span_1);
        span_1_2_3->link_right(span_3);

    bf::Span* span_5_6 = allocator.malloc();
      span_5_6->copy(span_6);
        span_5_6->link_left(span_5);

    bf::Span* span_7_8 = allocator.malloc();
      span_7_8->copy(span_7);
        span_7_8->link_right(span_8);

    root = allocator.malloc();
      root->copy(span_4);
        root->link_left(span_1_2_3);
        root->link_left(span_0);
        root->link_right(span_5_6);
        root->link_right(span_7_8);
  }

  ZGen::BestFirst::Span* root;
  ZGen::BestFirst::SpanAllocator allocator;
  std::vector<ZGen::dependency_t> data;
};

#endif  //  end for __ZGEN_UNITTEST_BESTFIRST_FIXTURE_1_H__
