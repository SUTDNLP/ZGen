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
PostagConstraint::PostagConstraint(const std::string& postag_dict_path, const std::string& most_frequent_postag_dict_path,
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
  std::ifstream ifs2(most_frequent_postag_dict_path);
  if(ifs2.good()){
	  _INFO << "knowledge: Loading most frequent postag dict from: " << most_frequent_postag_dict_path;
	  init_most_frequent_pos_from_file(ifs2);
  }else{
	  _INFO << "knowledge: Opening most frequent postag dict file failed.";
  }

  pos_VB = postags_alphabet.insert("VB");
  pos_NN = postags_alphabet.insert("NN");
  pos_NNP = postags_alphabet.insert("NNP");
  pos_MD = postags_alphabet.insert("MD");
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
PostagConstraint::init_most_frequent_pos_from_file(std::istream& ifs){
	char buffer[kMaxLengthOfBuffer];
	std::vector<std::string> tokens;

	while (ifs.getline(buffer, kMaxLengthOfBuffer)) {
	algo::split(tokens, buffer, boost::is_any_of("\t "),
		boost::token_compress_on);

	if (tokens.size() != 2) {
	  _WARN << "knowledge: Postag dict is not well-formed, should be \"[form] [postag]\"";
	  continue;
	}
	word_t word = forms_alphabet.encode(tokens[0]);
	postag_t tag = postags_alphabet.encode(tokens[1]);
	rep2[word] = tag;
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
PostagConstraint::get_possible_tags( const dependency_t* ref, int j, word_t word,
    std::vector<postag_t>& possible_tags) const {
  possible_tags.clear();
  if(ref->tense_participles[j] == TenseParticiple::kPastParticiple ||ref->tense_participles[j] == TenseParticiple::kPresentParticiple
		  ||ref->tense_participles[j] == TenseParticiple::kSimplePresent ||ref->tense_participles[j] == TenseParticiple::kSimplePast){
	  possible_tags.push_back(pos_VB);
	  return;
  }else if(ref->numbers[j] == Number::kSingular || ref->numbers[j] == Number::kPlural){
	  map_t::const_iterator itx = rep.find(word);
	  if (itx != rep.end()) {
		const postag_mask_t & mask = itx->second;
		if(mask.test(pos_NN)){
			possible_tags.push_back(pos_NN);
		}
		if(mask.test(pos_NNP)){
			possible_tags.push_back(pos_NNP);
		}
	  }
	  if(possible_tags.size()==0){		//could be because no entry exists in tag dictionary
		  possible_tags.push_back(pos_NN);
		  possible_tags.push_back(pos_NNP);
	  }
	  return;
  }else{
	  map_t::const_iterator itx = rep.find(word);
	  if (itx != rep.end()) {
		const postag_mask_t & mask = itx->second;
		for (int i = Engine::TokenAlphabet::END+ 1; i < postags_alphabet.size(); ++ i) {
		  if (mask.test(i)) { possible_tags.push_back(i); }
		}
		return;
	  }
  }

  for (int i = Engine::TokenAlphabet::END+ 1; i < postags_alphabet.size(); ++ i) {
    possible_tags.push_back(i);
  }
}

bool
PostagConstraint::is_verb(const dependency_t* ref, int j, word_t word) const {
	if(ref->tense_participles[j] == TenseParticiple::kPastParticiple ||ref->tense_participles[j] == TenseParticiple::kPresentParticiple
			  ||ref->tense_participles[j] == TenseParticiple::kSimplePresent ||ref->tense_participles[j] == TenseParticiple::kSimplePast){
		  return true;
	  }else{
		  map_t::const_iterator itx = rep.find(word);
		  if (itx != rep.end()) {
			const postag_mask_t & mask = itx->second;
			return mask.test(pos_VB) || mask.test(pos_MD);
		  }
	  }
	return false;
}

/**
 * exclude past participle, present participle and simple past
 */
bool
PostagConstraint::is_verb_present_tense(const dependency_t* ref, int j, word_t word) const {
	if(ref->tense_participles[j] == TenseParticiple::kPastParticiple ||ref->tense_participles[j] == TenseParticiple::kPresentParticiple
			  ||ref->tense_participles[j] == TenseParticiple::kSimplePast){
		return false;
	}else if(ref->tense_participles[j] == TenseParticiple::kSimplePresent){
		return true;
	}else{
		map_t::const_iterator itx = rep.find(word);
		if (itx != rep.end()) {
			const postag_mask_t & mask = itx->second;
			return mask.test(pos_VB);
		}
	}
	return false;
}


int
PostagConstraint::get_most_frequent_tag(const dependency_t* ref, int j, word_t word) const {
	postag_t output;
//	_INFO<<"word index "<<word<<" word "<<forms_alphabet.decode(word);
	if(ref->tense_participles[j] == TenseParticiple::kPastParticiple ||ref->tense_participles[j] == TenseParticiple::kPresentParticiple
			  ||ref->tense_participles[j] == TenseParticiple::kSimplePresent ||ref->tense_participles[j] == TenseParticiple::kSimplePast){
		output= pos_VB;
	}else if(ref->numbers[j] == Number::kSingular || ref->numbers[j] == Number::kPlural){
		output = pos_NN;
	}else{
		output = rep2.at(word);
	}
//	_INFO<<"pos "<<postags_alphabet.decode(output);
	return output;
}

}   //  end for namespace Knowledge
}   //  end for namespace ZGen

