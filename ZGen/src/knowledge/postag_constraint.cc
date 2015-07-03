#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include "config.h"
#include "settings.h"
#include "utils/logging.h"
#include "knowledge/postag_constraint.h"

namespace ZGen {
namespace Knowledge {

namespace algo = boost::algorithm;

//! Format of the postag dict should be:
//!
//! [form] [postag]
//! ...
PostagConstraint::PostagConstraint(const std::string& postag_dict_path,
    Engine::TokenAlphabet& _forms_alphabet,
    Engine::TokenAlphabet& _postags_alphabet,
    const std::string& tagset):
  forms_alphabet(_forms_alphabet),
  postags_alphabet(_postags_alphabet) {
  std::ifstream ifs(postag_dict_path);

  if (ifs.good()) {
    _INFO << "knowledge: Loading postag dict from: " << postag_dict_path;
    init_postag_constraints_from_file(ifs);
  } else {
    _INFO << "knowledge: Opening postag dict file failed.";
    init_default_postag_constraints(tagset);
  }
}

void
PostagConstraint::init_postag_constraints_from_file(std::istream& ifs) {
  char buffer[kMaxLengthOfBuffer];
  std::vector<std::string> tokens;

  while (ifs.getline(buffer, kMaxLengthOfBuffer)) {
    algo::split(tokens, buffer, boost::is_any_of("\t "),
        boost::token_compress_on);

    if (tokens.size() != 2) {
      _WARN << "knowledge: Postag dict is not well-formed, should be \"[form] [postag]\"";
      continue;
    }

    word_t word = forms_alphabet.insert(tokens[0]);
    postag_t tag = postags_alphabet.insert(tokens[1]);
    rep[word].set(tag);
  }
}

void
PostagConstraint::init_default_postag_constraints(const std::string& tagset) {
  if (tagset == "PTB") {
    word_t word;
    rep[forms_alphabet.insert("$")].set(postags_alphabet.insert("$"));
    rep[forms_alphabet.insert("``")].set(postags_alphabet.insert("``"));
    rep[forms_alphabet.insert("''")].set(postags_alphabet.insert("''"));
    rep[forms_alphabet.insert("-LRB-")].set(postags_alphabet.insert("-LRB-"));
    rep[forms_alphabet.insert("-RRB-")].set(postags_alphabet.insert("-RRB-"));
    rep[forms_alphabet.insert("-LCB-")].set(postags_alphabet.insert("-LRB-"));
    rep[forms_alphabet.insert("-RCB-")].set(postags_alphabet.insert("-RRB-"));
    rep[forms_alphabet.insert("-LSB-")].set(postags_alphabet.insert("-LRB-"));
    rep[forms_alphabet.insert("-RSB-")].set(postags_alphabet.insert("-RRB-"));
    rep[forms_alphabet.insert(",")].set(postags_alphabet.insert(","));
    rep[forms_alphabet.insert(".")].set(postags_alphabet.insert("."));
    rep[forms_alphabet.insert(":")].set(postags_alphabet.insert(":"));
    rep[forms_alphabet.insert("#")].set(postags_alphabet.insert("#"));
    _INFO << "knowledge: Use default postag constrain for PTB.";
  } else {
    _WARN << "knowledge: Unsupported tagset.";
  }
}

void
PostagConstraint::get_possible_tags(word_t word,
    std::vector<postag_t>& possible_tags) const {
  possible_tags.clear();
  map_t::const_iterator itx = rep.find(word);

  if (itx != rep.end()) {
    const postag_mask_t & mask = itx->second;
    for (int i = Engine::TokenAlphabet::END+ 1; i < postags_alphabet.size(); ++ i) {
      if (mask.test(i)) { possible_tags.push_back(i); }
    }
    return;
  }

  for (int i = Engine::TokenAlphabet::END+ 1; i < postags_alphabet.size(); ++ i) {
    possible_tags.push_back(i);
  }
}

}   //  end for namespace Knowledge
}   //  end for namespace ZGen

