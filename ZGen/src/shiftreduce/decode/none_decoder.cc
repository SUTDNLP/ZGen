#include "shiftreduce/decode/none_decoder.h"

namespace ZGen {
namespace ShiftReduce {

namespace kn = ZGen::Knowledge;

NoneDecoder::NoneDecoder(const option_t& _opts,
    DecodeContext* _ctx,
    Model* _model,
    const kn::PostagConstraint* _constraint,
	const Engine::TokenAlphabet& _forms_alphabet,
    int _tag, int _nr_relations)
  : Decoder(_opts, _ctx, _model, 
			_constraint,_forms_alphabet, _tag, _nr_relations) {
}

void
NoneDecoder::config_input(const dependency_t* input) {
  // leave it empty.
}

void
NoneDecoder::get_possible_actions(const StateItem& item) {
  possible_actions.clear();
  const dependency_t* ref = item.ref;
  int N= ref->size();
  for (int j = 0; j < N; ++ j) {
    if (item.buffer.test(j)) {
      word_t word = ref->forms[j];
      postag_t tag = ref->postags[j];
      get_possible_shift_actions(item, j, word, tag);
    }
  }

  if (item.stack.size() > 2) {
    if (opts.output_label) {
      for (int i = 3; i< nr_relations; ++ i) {
        possible_actions.push_back(ActionFactory::make_left_arc(i));
        possible_actions.push_back(ActionFactory::make_right_arc(i));
      }
    } else {
      possible_actions.push_back(ActionFactory::make_left_arc());
      possible_actions.push_back(ActionFactory::make_right_arc());
    }
  }
}


} //  end for namespace ShiftReduce
} //  end for namespace ZGen
