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
  PostagConstraint(const std::string& postag_dict_path,
      Engine::TokenAlphabet& _forms_alphabet,
      Engine::TokenAlphabet& _postags_alphabet,
      const std::string& tagset = "PTB");

  /**
   * Given a word in word_t, get it's possible tags.
   *
   *  @param[in]  word          The word string.
   *  @param[out] possible_tags The possible tags.
   */
  void get_possible_tags(const word_t word,
      std::vector<postag_t>& possible_tags) const;

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
   * Initialize the default constrain. Default constrain includes the
   * punctations.
   */
  void init_default_postag_constraints(const std::string& tagset);

  typedef std::bitset<kMaxNumberOfPostags> postag_mask_t;
  typedef std::unordered_map<word_t, postag_mask_t> map_t;

  map_t rep;  //! The payload.

  Engine::TokenAlphabet& forms_alphabet;
  Engine::TokenAlphabet& postags_alphabet;
};

} //  namespace knowledge
} //  namespace zgen

#endif  //  end for __ZGEN_KNOWLEDGE_POSTAG_CONSTRAIN_H__
