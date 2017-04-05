#include "types/instance.h"

namespace ZGen {

void
DependencyParse::clear() {
  forms.clear();
  lexemes.clear();
  prefix_types.clear();
  postags.clear();
  heads.clear();
  deprels.clear();
  words.clear();
  phrases.clear();
  extras.clear();
  tense_participles.clear();
  numbers.clear();
  brackets.clear();
  quotes.clear();
  quote_brackets.clear();

}

void
DependencyParse::push_back(const word_t & form,
    const postag_t & postag,
    const int head,
    const deprel_t & deprel) {
  forms.push_back(form);
  postags.push_back(postag);
  heads.push_back(head);
  deprels.push_back(deprel);
}

void
DependencyParse::push_back(const word_t& form, 
    const postag_t& postag,
    const int head,
    const deprel_t& deprel,
    const std::string& extra) {
  push_back(form, postag, head, deprel);
  extras.push_back(extra);
}

void
DependencyParse::push_back(const word_t & form,
    const postag_t & postag,
    const int head,
    const deprel_t & deprel,
    const std::vector<word_t> & extended_words,
    const range_t & phrase,
    bool is_phrase) {

  push_back(form, postag, head, deprel);

  phrases.push_back(phrase);
  for (int i = 0; i < extended_words.size(); ++ i) {
    word_t word = extended_words[i];
    words.push_back(word);
  }

  is_phrases.push_back(is_phrase);
  return;
}

void
DependencyParse::push_back(const std::vector<word_t> & v_lexemes, const std::vector<PrefixType> & v_prefix_types ){
	lexemes.push_back(v_lexemes);
	prefix_types.push_back(v_prefix_types);
}

void
DependencyParse::push_back(const word_t& form,
    const postag_t& postag,
    const int head,
    const deprel_t& deprel,
    const std::vector<word_t>& extended_words,
    const range_t& phrase,
    bool is_phrase,
    const std::string& extra) {
  push_back(form, postag, head, deprel, extended_words, phrase, is_phrase);
  extras.push_back(extra);
}

word_t DependencyParse::form(int index) const     { return forms.at(index); }

void DependencyParse::push_back_extra_info(TenseParticiple tense_participle , Number number, Quoted quoted, Bracket bracket, QuoteBracket quote_bracket){
	tense_participles.push_back(tense_participle);
	numbers.push_back(number);
	quotes.push_back(quoted);
	brackets.push_back(bracket);
	quote_brackets.push_back(quote_bracket);
}
postag_t
DependencyParse::postag(int index) const { return postags.at(index); }

size_t DependencyParse::size() const              { return forms.size(); }

std::ostream& operator << (std::ostream& os, const DependencyParse& d) {
  int N = d.forms.size();
  for (int i = 0; i < N; ++ i) {
    os << d.forms[i] << "\t";

    if (d.postags.size() == N) {
      os << d.postags[i];
    } else {
      os << "_";
    }
    os << "\t";

    if (d.heads.size() == N) {
      os << d.heads[i];
    } else {
      os << "_";
    }
    os << "\t";

    if (d.deprels.size() == N) {
      os << d.deprels[i];
    } else {
      os << "NONE";
    }

    os << "\t(";
    for (int j = d.phrases.at(i).first; j < d.phrases.at(i).second; ++ j) {
      os << d.words[j];
      if (j < d.phrases[i].second - 1) {
        os << "|";
      }
    }
    os << ")" << std::endl;
  }

  return os;
}

} //  end for namespace ZGen
