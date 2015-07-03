#include "shiftreduce/decode/full_decoder.h"

namespace ZGen {
namespace ShiftReduce {

namespace kn = ZGen::Knowledge;

FullDecoder::FullDecoder(const option_t& _opts,
    DecodeContext* _ctx,
    Model* _model,
    const kn::PostagConstraint* _constraint,
    const Engine::TokenAlphabet& _forms_alphabet,
	int _tag, int _nr_relations)
  : Decoder(_opts, _ctx, _model, 
			_constraint, _forms_alphabet, _tag, _nr_relations) {
}

void
FullDecoder::config_input(const dependency_t* input) {
  // leave it empty.
  tree.set_ref(input);
}

void
FullDecoder::get_possible_actions(const StateItem& item) {
  int N = item.ref->size();
  possible_actions.clear();

  if (item.stack.size() == 1) {
    for (int j = 0; j < N; ++ j) {
      if (item.buffer.test(j)) {
        word_t word = item.ref->forms[j];
        postag_t tag = item.ref->postags[j];
        get_possible_shift_actions(item, j, word, tag);
      }
    }
  } else {
    bool all_descendants_shifted = true;
    int top0 = item.top0;
    const DependencyTree::edgeset_t& descendants = tree.descendants(top0);

    for (const int d: descendants) {
      if (item.buffer.test(d)) { all_descendants_shifted = false; break; }
    }

    if (!all_descendants_shifted) {
      for (const int d: descendants) {
        if (item.buffer.test(d)) {
          word_t word = item.ref->forms[d];
          postag_t tag = item.ref->postags[d];
          get_possible_shift_actions(item, d, word, tag);
        }
      }
    } else {
      int top1 = item.top1;
      if (top1 >= 0 && tree.arc(top0, top1)) {
        if (opts.output_label) {
          deprel_t deprel = item.ref->deprels[top1];
          possible_actions.push_back(ActionFactory::make_left_arc(deprel));
        } else {
          possible_actions.push_back(ActionFactory::make_left_arc());
        }
      } else if (top1 >= 0 && tree.arc(top1, top0)) {
        if (opts.output_label) {
          deprel_t deprel = item.ref->deprels[top0];
          possible_actions.push_back(ActionFactory::make_right_arc(deprel));
        } else {
          possible_actions.push_back(ActionFactory::make_right_arc());
        }
      } else {
        const DependencyTree::edgeset_t& siblings= tree.siblings(top0);
        for (const int s: siblings) {
          if (item.buffer.test(s)) {
            word_t word = item.ref->forms[s];
            postag_t tag = item.ref->postags[s];
            get_possible_shift_actions(item, s, word, tag);
          }
        }

        int h = tree.head(top0);
        if (item.buffer.test(h)) {
          word_t word = item.ref->forms[h];
          postag_t tag = item.ref->postags[h];
          get_possible_shift_actions(item, h, word, tag);
        }
      }
    }
  }
}

} //  end for namespace ShiftReduce 
} //  end for namespace ZGen
