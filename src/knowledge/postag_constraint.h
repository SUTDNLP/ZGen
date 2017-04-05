#ifndef __ZGEN_KNOWLEDGE_POSTAG_CONSTRAIN_H__
#define __ZGEN_KNOWLEDGE_POSTAG_CONSTRAIN_H__

#include <iostream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include "settings.h"
#include "types/instance.h"
#include "engine/token_alphabet.h"

namespace ZGen {
namespace Knowledge {

class PostagConstraint {
public:
  PostagConstraint(const std::string& postag_dict_path, const std::string& most_frequent_postag_dict_path,
      Engine::TokenAlphabet& _forms_alphabet,
      Engine::TokenAlphabet& _postags_alphabet,
      const std::string& tagset = "PTB");

  /**
   * Given a word in word_t, get it's possible tags.
   *
   *  @param[in]  word          The word string.
   *  @param[out] possible_tags The possible tags.
   */
  void get_possible_tags( const dependency_t* ref, int j, const word_t word,
      std::vector<postag_t>& possible_tags) const;

  bool is_verb(const dependency_t* ref, int j, word_t word) const;

  bool is_verb_present_tense(const dependency_t* ref, int j, word_t word) const;

  int get_most_frequent_tag(const dependency_t* ref, int j, const word_t word) const;

  postag_t pos_VB;
  postag_t pos_MD;

private:
  /**
   * Read the postag dict file, initialize the constrain
   * from dict entries.
   *
   *  @param[in]  ifs   The input file stream, which is opened by the
   *                    constructor.
   */
  void init_postag_constraints_from_file(std::istream& ifs);

  /**
   * Read the dict file for mapping between word and most frequent pos, initialize the feature from dict entries
   *
   * @param[in] ifs The input file stream, which is opened by the constructor
   */
  void init_most_frequent_pos_from_file(std::istream& ifs);

  /**
   * Initialize the default constrain. Default constrain includes the
   * punctations.
   */
  void init_default_postag_constraints(const std::string& tagset);

  typedef std::bitset<kMaxNumberOfPostags> postag_mask_t;
  typedef std::unordered_map<word_t, postag_mask_t> map_t;
  typedef std::unordered_map<word_t, postag_t> map_u;

  map_t rep;  //! The payload.
  map_u rep2;	//! the mapping between word and most frequent tag

  Engine::TokenAlphabet& forms_alphabet;
  Engine::TokenAlphabet& postags_alphabet;
  postag_t pos_NN;
  postag_t pos_NNP;
};

} //  namespace knowledge
} //  namespace zgen

#endif  //  end for __ZGEN_KNOWLEDGE_POSTAG_CONSTRAIN_H__
