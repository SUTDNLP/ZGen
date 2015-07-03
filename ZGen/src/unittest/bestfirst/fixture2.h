#ifndef __ZGEN_UNITTEST_BESTFIRST_FIXTURE_2_H__
#define __ZGEN_UNITTEST_BESTFIRST_FIXTURE_2_H__

#include "engine/engine.h"
#include "types/instance.h"
#include "utils/ioutils.h"
#include "bestfirst/search/span.h"
#include "bestfirst/search/span_allocator.h"

struct Fixture2 {
  Fixture2()
    : allocator(127, 128) {
    namespace z = ZGen;
    namespace io = ZGen::IO;
    namespace bf = ZGen::BestFirst;
    namespace eg = ZGen::Engine;

    std::stringstream S;
    S << "__Ms._Haag__ _ -1 N" << std::endl
      << "plays _ -1 N"        << std::endl
      << "__Elianti__ _ -1 N"  << std::endl
      << ". _ -1 N"            << std::endl;

    io::read_dependency_dataset(S, data);

    const z::dependency_t* inst = &data[0];
    span_0 = allocator.malloc(); span_0->init(inst, 0, eg::PennTreebankPostagAlphabet::NP);
    span_1 = allocator.malloc(); span_1->init(inst, 1, eg::PennTreebankPostagAlphabet::VBZ);
    span_2 = allocator.malloc(); span_2->init(inst, 2, eg::PennTreebankPostagAlphabet::NP);
    span_3 = allocator.malloc(); span_3->init(inst, 3, eg::PennTreebankPostagAlphabet::PERIOD);

    span_0_1 = allocator.malloc();
      span_0_1->copy(span_1);
      span_0_1->link_left(span_0);

    span_0_1_2 = allocator.malloc();
      span_0_1_2->copy(span_0_1);
      span_0_1_2->link_right(span_2);

    span_0_1_2_3 = allocator.malloc();
      span_0_1_2_3->copy(span_0_1_2);
      span_0_1_2_3->link_right(span_3);
  }

  typedef ZGen::BestFirst::Span span_t;

  span_t* span_0;
  span_t* span_1;
  span_t* span_2;
  span_t* span_3;
  span_t* span_0_1;
  span_t* span_0_1_2;
  span_t* span_0_1_2_3;

  ZGen::BestFirst::SpanAllocator allocator;
  std::vector<ZGen::dependency_t> data;
};

#endif  //  end for __ZGEN_UNITTEST_BESTFIRST_FIXTURE_2_H__
