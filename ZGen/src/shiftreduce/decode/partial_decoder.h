#ifndef __ZGEN_SHIFTREDUCE_PARTIAL_DECODER_H__
#define __ZGEN_SHIFTREDUCE_PARTIAL_DECODER_H__

#include "decoder.h"
#include "types/tree.h"
#include <bitset>

namespace ZGen {
namespace ShiftReduce {

class PartialDecoder: public Decoder {
public:
  PartialDecoder(const option_t& opts,
      DecodeContext* ctx,
      Model* model,
      const Knowledge::PostagConstraint* constraint,
      const Engine::TokenAlphabet& forms_alphabet,
	  int NP, int nr_relations);

private:
  /**
   * Config the input onto the decoder.
   *
   *  @param[in]  input       The pointer to the input instance.
   */
  void config_input(const dependency_t* input);

  //! Implement the get possible actions
  void get_possible_actions(const StateItem& item);

  /**
   * Return whether it is legal to shift the ith word onto the stack. This
   * function corresponding to the algorithm shows the ``partial-constrain''
   * part in the NAACL2015 paper.
   *
   *  @param[in]  item    The given state
   *  @param[in]  i       The index of the to be shifted word.
   *  @return     bool    true on it's legal to shift the ith word, otherwise
   *                      false.
   */
  bool legal_shift(const StateItem& item, int i);

  /**
   *
   *
   *
   */
  bool legal_left_arc(const StateItem& item);

  /**
   *
   *
   *
   *
   */
  bool legal_right_arc(const StateItem& item);


  typedef std::bitset<kMaxNumberOfWords> mask_t;
  bool all_children_found(const int modifier, const mask_t& sigma, const mask_t& buffer);

  //!
  DependencyForest forest;
};

} //  end for namespace Shiftreduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_PARTIAL_DECODER_H__
