#include "shiftreduce/decode/partial_decoder.h"

namespace ZGen {
namespace ShiftReduce {

namespace kn = ZGen::Knowledge;
namespace eg = ZGen::Engine;

PartialDecoder::PartialDecoder(const option_t& _opts,
    DecodeContext* _ctx,
    Model* _model,
    const kn::PostagConstraint* _constraint,
    const Engine::TokenAlphabet& _forms_alphabet,
	int _tag, int _nr_relations)
  : Decoder(_opts, _ctx, _model,      
	   		_constraint, _forms_alphabet, _tag, _nr_relations) {
}

void
PartialDecoder::config_input(const dependency_t* input) {
  //
  forest.set_ref(input);
}

void
PartialDecoder::get_possible_actions(const StateItem& item) {
  int N = item.ref->size();
  possible_actions.clear();

  if (item.stack.size() == 1) {
    // The initial state, every word is OK to be shifted onto the stack.
    for (int j = 0; j < N; ++ j) {
      if (item.buffer.test(j)) {
        word_t word = item.ref->forms[j];
        postag_t tag = item.ref->postags[j];
        get_possible_shift_actions(item, j, word, tag);
      }
    }
  } else {
    if (legal_left_arc(item)) {
      if (!opts.output_label) {
        possible_actions.push_back(ActionFactory::make_left_arc());
      } else {
        for (deprel_t deprel = 3; deprel < nr_relations; ++ deprel) {
          possible_actions.push_back(ActionFactory::make_left_arc(deprel));
        }
      }
    }
    if (legal_right_arc(item)) {
      if (!opts.output_label) {
        deprel_t deprel = eg::TokenAlphabet::NONE;
        possible_actions.push_back(ActionFactory::make_right_arc());
      } else {
        for (deprel_t deprel = 3; deprel < nr_relations; ++ deprel) {
          possible_actions.push_back(ActionFactory::make_right_arc(deprel));
        }
      }
    }
    for (int j = 0; j < N; ++ j) {
      word_t word = item.ref->forms[j];
      postag_t tag = item.ref->postags[j];
      if (legal_shift(item, j)) {
        get_possible_shift_actions(item, j, word, tag);
      }
    }
  }
}

bool
PartialDecoder::all_children_found(const int m,
    const mask_t& sigma, const mask_t& buffer) {
  const DependencyTree::edgeset_t& cc = forest.children(m);
  for (const int c: cc) { if (sigma[c] || buffer[c]) { return false; } }
  return true;
}

bool
PartialDecoder::legal_left_arc(const StateItem& item) {
  if (item.stack.size() < 3 ||
      !(forest.is_root(item.top1) || forest.arc(item.top0, item.top1))) {
    // 1. If there is less than two elements on the stack, cannot LEFT-ARC
    // 2. If the top1 element has a head that is not top0, cannot LEFT-ARC
    return false;
  }

  const DependencyTree::edgeset_t& children = forest.children(item.top1);
  for (const int d: children) {
    // 3. If there is the following constrain:
    //
    //   .-----------------.
    //   |                 v
    //  top1 top0 |   | .. d
    // -----------    `-----
    //
    // LEFT-ARC cannot be performed.
    if (item.buffer[d]) { return false; }
  }

  for (const int k: item.stack) {
    // 4. If there is the following constrain:
    //
    //  .------.
    //  v      |
    //  k .. top1 top0 |  |
    // ----------------.
    //
    // LEFT-ARC cannot be performed.
    if (k < 0) { continue; }
    if (forest.head(k) == item.top1) { return false; }
  }
  return true;
}

bool
PartialDecoder::legal_right_arc(const StateItem& item) {
  if (item.stack.size() < 3 ||
      !(forest.is_root(item.top0) || forest.arc(item.top1, item.top0))) {
    // 1. If there is less than two elements on the stack, cannot LEFT-ARC
    // 2. If the top0 element has a head that is not top1, cannot LEFT-ARC
    return false;
  }

  const DependencyTree::edgeset_t& children = forest.children(item.top0);
  for (const int d: children) {
    // 3. If there is the following constrain:
    //
    //         .-----------.
    //         |           v
    //  top1 top0 |   | .. d
    // -----------    `-----
    //
    // RIGHT-ARC cannot be performed.
    if (item.buffer[d]) { return false; }
  }

  for (const int k: item.stack) {
    // 4. If there is the following constrain:
    //
    //  .-----------.
    //  v           |
    //  k .. top1 top0 |  |
    // ---------------- 
    //
    // RIGHT-ARC cannot be performed.
    if (k < 0) { continue; }
    if (forest.head(k) == item.top0) { return false; }
  }
  return true;
}

bool
PartialDecoder::legal_shift(const StateItem& item, int target) {
  if (!item.buffer[target]) {
    // The word has already be shifted onto the stack, ignore it.
    return false;
  }

  int L = item.stack.size();

  std::bitset<kMaxNumberOfWords> sigma;
  for (const int k: item.stack) {
    if (k < 0) { continue; }
    sigma[k] = true;
  }

  for (int i = 1; i < L; ++ i) {  // i start from 1 becase a dummy symble is at 0.
    int k = item.stack[i];

    if (!forest.same_ancestor(k, target)) { continue; }

    int lca = forest.lca(k, target);

    if (forest.head(k) == target) {
      // The word to be SHIFTED should has a left modifier on the stack.
      //
      //  .-----------------.
      //  v                 |
      //  k .. top0 |   | target
      // -----------     ------
      //
      // Then the following actions should be a serial of LEFT-ARC from top0 to k,
      // which means the (1) Head-Check: element on stack from k to top0 should either
      // without tree constrain or its head should be target, like
      //
      //  .-----------------------.
      //  v    v                  |
      //  k .. k' .. top0 |   | target
      // -----------------     ------
      //
      // Such condition also (2) Children-Finished-Check: enforce the elements from k to
      // top0 should not have unfinished children.
      //
      for (int lmi = i; lmi < L; ++ lmi) {
        int lm = item.stack[lmi];
        // Head check
        if (!forest.is_root(lm) && forest.head(lm) != target) { return false; }

        // Children Finished Check
        if (!all_children_found(lm, sigma, item.buffer)) { return false; }
      }
    } else if (lca == target) {
      // The word to be SHIFTED is a grand parent of the word, which mean there
      // is such constrain that
      //
      //  .----- -----------------.
      //  v    |/                 |
      //  k .. k' .. top0 |   | target
      // -----------------     ------
      //
      // At such condition, target should not be shifted onto the stack.
      return false;
    } else if (forest.head(target) == k) {
      // The word to be SHIFTED is a child of a word on stack. This also enforce
      // a serial of LEFT-ARC in future, so the condition falls into the same with
      // that of LEFT-ARC
      //
      for (int lmi = i+ 1; lmi < L; ++ lmi) {
        int lm = item.stack[lmi];
        if (!forest.is_root(lm) && forest.head(lm) != target) { return false; }
        if (!all_children_found(lm, sigma, item.buffer)) { return false; }
      }
    } else if (lca == k) {
      // The word to be SHIFTED is a descendants of a word on stack. (1) Different-Tree
      // which means a stack word belongs to a different tree component. Condition like
      //
      //  .----- -----------------.
      //  |    v/                 v
      //  k .. k' .. top0 |   | target
      // -----------------     ------
      //
      // is allowed. Because after this word is shifted, only the tree when descendants
      // linked to k will be generated. Thus the following condition is not allow.
      //
      //  .------------------------------.
      //  |      ------.             ----|
      //  |    v/      |           v/    v
      //  k .. k' .. top0 |   | target  k''
      // -----------------     ------
      //
      const DependencyTree::edgeset_t& desc = forest.descendants(lca);
      for (int lmi = i+ 1; lmi < L; ++ lmi) {
        int lm = item.stack[lmi];
        if (!forest.is_root(lm) && std::find(desc.begin(), desc.end(), lm) == desc.end()) {
          return false;
        }
        if (forest.is_root(lm)) {
          if (!all_children_found(lm, sigma, item.buffer)) { return false; }
        }
      }
    } else {
      // k and target are siblings, which urge the k should be the direct child of LCA and k's
      // children are all founded.
      if (forest.head(k) != lca || !all_children_found(k, sigma, item.buffer)) { return false; }

      // If LCA is at the left of k, after target shifted, there is not possibility for RIGHT-ARC
      // between LCA and k.
      for (int lmi = 1; lmi < i; ++ lmi) {
        int lm = item.stack[lmi];
        if (lm == lca) { return false; }
      }

      // If LCA is at the right of target, the condition is same with that of IV.
      //  .------------------------------.
      //  |      ------.             ----|
      //  v    v/      |           v/    |
      //  k .. k' .. top0 |   | target  LCA
      // -----------------     ------
      //
      if (item.buffer[lca]) {
        const DependencyTree::edgeset_t& desc = forest.descendants(lca);
        for (int lmi = i+ 1; lmi < L; ++ lmi) {
          int lm = item.stack[lmi];
          if (!forest.is_root(lm) && std::find(desc.begin(), desc.end(), lm) == desc.end()) {
            return false;
          }
          if (forest.is_root(lm)) {
            if (!all_children_found(lm, sigma, item.buffer)) { return false; }
          }
        }
      }
    }
  }
  return true;
}

} //  end for namespace ShiftReduce
} //  end for namespace ZGen
