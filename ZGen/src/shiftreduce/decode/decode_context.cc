#include "shiftreduce/settings.h"
#include "shiftreduce/decode/decode_context.h"

namespace ZGen {
namespace ShiftReduce {

DecodeContext::DecodeContext() {
  lattice= new StateItem[kMaxSteps* kMaxBeamSize];
}

DecodeContext::~DecodeContext() {
  delete [](lattice);
}

} //  end for namespace ShiftReduce
} //  end for namespace ZGen
