#ifndef __ZGEN_TYPES_INSTANCE_H__
#define __ZGEN_TYPES_INSTANCE_H__

#include <iostream>
#include <vector>
#include <string>
#include <map>

namespace ZGen {

// Type define
typedef int word_t;
typedef int postag_t;
typedef int deprel_t;
typedef int split_mode_t;
typedef std::vector<word_t> sentence_t;

enum class Number{
	kNone=0,
	kSingular,
	kPlural
};

enum class Tense{
	kNone=0,
	kPresent,
	kPast
};

enum class Bracket{
	kNone=1,
	kRound,
	kRoundRound,
	kCurly
};

enum class Quoted{
	kNone=1,
	kSingle,
	kDouble,
	kDoubleSingle,
	kDoubleDouble,
	kDoubleDoubleDouble
};

enum class QuoteBracket{
	kNone=1,
	kSingleQuoteBegin,
	kSingleQuoteEnd,
	kDoubleQuoteBegin,
	kDoubleQuoteEnd,
	kRoundBracketBegin,
	kRoundBracketEnd,
	kCurlyBracketBegin,
	kCurlyBracketEnd
};

enum class Participle{
	kNone=0,
	kPresent,
	kPast
};

enum class TenseParticiple{
	kNone = 0,
	kSimplePresent,
	kSimplePast,
	kPresentParticiple,
	kPastParticiple,
	kDefault
};

enum class PrefixType{
	kNone=0,
	kBeginsA,
	kBeginsE,
	kBeginsI,
	kBeginsO,
	kBeginsU,
	kBeginsEu,
	kBeginsOn,
	kBeginsH
};
// The struct for dependency parse
struct DependencyParse {
  typedef std::pair<int, int> range_t;

  std::vector<word_t>       forms;      //! The heads. For the pre-recognized NP,
                                        //! just the head is stored in forms.
  std::vector<std::vector<word_t>> lexemes;	//multiple lexeme for each form
  std::vector<std::vector<PrefixType>> prefix_types;	//prefix type for each form
  std::vector<postag_t>     postags;    //! The postags.
  std::vector<int>          heads;      //! The heads
  std::vector<deprel_t>     deprels;    //! The dependency relation.
  std::vector<word_t>       words;      //!
  std::vector<range_t>      phrases;    //!
  std::vector<bool>         is_phrases; //! Record whether ith item is phrase.
  std::vector<std::string>  extras;     //! For extra information.
  std::vector<Number> numbers;
  std::vector<Tense> tenses;
  std::vector<Bracket> brackets;
  std::vector<Quoted> quotes;
  std::vector<Participle> participles;
  std::vector<TenseParticiple> tense_participles;
  std::vector<QuoteBracket> quote_brackets;
  std::vector<int>		function_words;	//! the positions of function words - only used during training
  std::map<int, int>	input_gold_node_mapping;	//! maps input node and gold node - only used during training
  std::map<int, int>	gold_input_node_mapping;	//! maps gold node and input node- only used during training

  void clear();

  void push_back(const word_t & form,
      const postag_t & postag,
      const int head,
      const deprel_t & deprel);

  void push_back(const word_t& form,
      const postag_t& postag,
      const int head,
      const deprel_t& deprel,
      const std::string& extra);

  void push_back(const word_t & form,
      const postag_t & postag,
      const int head,
      const deprel_t & deprel,
      const std::vector<word_t> & extended_words,
      const range_t & phrase,
      bool is_phrase);

  void push_back(const std::vector<word_t> & v_lexemes, const std::vector<PrefixType> & v_prefix_types);

  void push_back(const word_t& form,
      const postag_t& postag,
      const int head,
      const deprel_t& deprel,
      const std::vector<word_t>& extended_words,
      const range_t& phrase,
      bool is_phrase,
      const std::string& extra);

  void push_back_extra_info(TenseParticiple tense_participle, Number number, Quoted quoted, Bracket bracket, QuoteBracket quote_bracket);

  size_t size() const;
  word_t form(int index) const;
  postag_t postag(int index) const;
  friend std::ostream& operator << (std::ostream & os, const DependencyParse & d);
};

typedef DependencyParse dependency_t;
typedef DependencyParse unordered_dependency_t;

} //  namespace ZGen

#endif  //  end for __ZGEN_TYPES_INSTANCE_H__
