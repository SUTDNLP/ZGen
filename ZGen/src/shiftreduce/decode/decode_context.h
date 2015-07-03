#ifndef __ZGEN_SHIFTREDUCE_DECODE_CONTEXT_H__
#define __ZGEN_SHIFTREDUCE_DECODE_CONTEXT_H__
#include "shiftreduce/types/state.h"

namespace ZGen {
namespace ShiftReduce {

//! Used to maintain the memory cache used by the decoder.
struct DecodeContext {
  DecodeContext();
  ~DecodeContext();

  StateItem* lattice; //! The lattice for decoding.
  StateItem* lattice_index[kMaxSteps];  //! The starting pointer for each round.
};

} //  end for namespace ShiftReduce
} //  end for namespace ZGen


#endif  //  end for __ZGEN_SHIFTREDUCE_DECODE_CONTEXT_H__
