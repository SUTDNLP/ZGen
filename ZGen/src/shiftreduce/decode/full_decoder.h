#ifndef __ZGEN_SHIFTREDUCE_FULL_DECODER_H__
#define __ZGEN_SHIFTREDUCE_FULL_DECODER_H__

#include "decoder.h"
#include "types/tree.h"

namespace ZGen {
namespace ShiftReduce {

class FullDecoder : public Decoder {
public:
  FullDecoder(const option_t& opts,
      DecodeContext* ctx,
      Model* model,
      const Knowledge::PostagConstraint* constraint,
      const Engine::TokenAlphabet& forms_alphabet,
	  int tag, int nr_relations);

private:
  /**
   * Config the input onto the decoder.
   *
   *  @param[in]  input       The pointer to the input instance.
   */
  void config_input(const dependency_t* input);

  /**
   * Get the possible actions from the current state. Store the possible
   * actions in the possible_actions (action_collection_t)
   *
   *  @param[in]  item        The input item
   */
  void get_possible_actions(const StateItem& item);

  //!
  DependencyTree tree;
};

} //  end for namespace ShiftReduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_FULL_DECODER_H__
