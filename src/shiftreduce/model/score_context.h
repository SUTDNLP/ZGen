#ifndef __ZGEN_SHIFTREDUCE_MODEL_SCORE_CONTEXT_H__
#define __ZGEN_SHIFTREDUCE_MODEL_SCORE_CONTEXT_H__

#include <iterator>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include "engine/token_alphabet.h"
#include "shiftreduce/types/state.h"
#include "types/instance.h"
#include "shiftreduce/types/internal_types.h"
#include "utils/math_utils.h"   //  for bin

#define _LEGEAL_RANGE_(x) (((x) >= 0) && ((x) < N))

#define __SET_CTX(prefix) do { \
  prefix##w = item.words_shifted_map.at(prefix);\
  prefix##p = item.postags[prefix]; \
  prefix##l = item.deprels[prefix]; \
} while (0);

#define __SET_CTX_2(prefix, parent) do { \
  prefix##w = item.words_shifted_map.at(prefix);\
  prefix##p = item.postags[prefix]; \
  if(item.graph->path_map_exists(parent, prefix) && item.graph->has_children(parent) && item.graph->arc(parent,prefix)){\
	prefix##l = item.graph->path_map.at(std::make_pair(parent, prefix)).get<2>();\
  }else if(item.graph->is_function_word(parent) || item.graph->is_function_word(prefix)){\
	prefix##l = 400;\
  }else{\
	prefix##l = 500;\
  }  \
} while (0);

#define __CLEAR_CTX(prefix) do { \
  prefix##w = kNoneWord; \
  prefix##p = kNonePostag; \
  prefix##l = kNoneDeprel; \
} while (0);

#define __SET_CNT(prefix) do { \
  prefix##la = item.nr_left_children[prefix]; \
  prefix##ra = item.nr_right_children[prefix]; \
  prefix##ls = utils::bin(item.nr_left_descendant[prefix]); \
  prefix##rs = utils::bin(item.nr_right_descendant[prefix]); \
} while (0);

#define __CLEAR_CNT(prefix) do { \
  prefix##la = 0; \
  prefix##ra = 0; \
  prefix##ls = 0; \
  prefix##rs = 0; \
} while (0);

#define __SET_LABEL_POS_SHIFTED(prefix, position) do {\
if(position>=0 && item.graph->prefix##_map.count(position) >0){	\
	prefix##_##position##_shifted = 0; \
	prefix##_##position##_not_shifted = 0; \
	prefix##_##position##_pos_shifted = 0; \
	prefix##_##position##_pos_not_shifted = 0; \
	int prime = 31; \
	std::vector<std::pair<int,int>> prefix##s = item.graph->prefix##_map.at(position); \
	std::vector<int> prefix##_shifted_pos_list;\
	std::vector<int> prefix##_not_shifted_pos_list;\
	for(auto elem:prefix##s){	\
		if(elem.first==-1 || item.buffer.test(elem.first)){\
			prefix##_##position##_not_shifted = prime* prefix##_##position##_not_shifted + elem.second;	\
			if(elem.first!=-1){ \
				prefix##_not_shifted_pos_list.push_back(item.constraint->get_most_frequent_tag(item.ref, elem.first, item.ref->form(elem.first))); \
			}\
		}else{\
			prefix##_##position##_shifted = prime* prefix##_##position##_shifted + elem.second;\
			prefix##_shifted_pos_list.push_back(item.constraint->get_most_frequent_tag(item.ref, elem.first, item.ref->form(elem.first))); \
		}	\
	}\
	prefix##_##position##_pos_shifted = add_word_pos_feature(&prefix##_shifted_pos_list);\
	prefix##_##position##_pos_not_shifted = add_word_pos_feature(&prefix##_not_shifted_pos_list);\
	}\
} while(0);

namespace ZGen {

namespace ShiftReduce {

extern int label_VC;
extern int label_APPO;
extern int word_comma;
extern int pos_comma;
extern int word_to;
extern int word_that;
extern int pos_to;
extern int pos_that;

namespace eg = ZGen::Engine;
namespace utils= ZGen::Utility;

const int kNoneWord = eg::TokenAlphabet::NONE;
const int kNonePostag = eg::TokenAlphabet::NONE;
const int kNoneDeprel = eg::TokenAlphabet::NONE;

struct ScoreContext {
	void load_generic_actions(const StateItem& item, action_t action){
		int S0 = item.top0;
		parent_bw0_shifted = 0;
		parent_bw0_not_shifted = 0;
		children_bw0_shifted = 0;
		children_bw0_not_shifted = 0;
		sibling_bw0_shifted = 0;
		sibling_bw0_not_shifted = 0;

		parent_bw0_pos_shifted = 0;
		parent_bw0_pos_not_shifted = 0;
		children_bw0_pos_shifted = 0;
		children_bw0_pos_not_shifted = 0;
		sibling_bw0_pos_shifted = 0;
		sibling_bw0_pos_not_shifted = 0;

		buffer_word = 0;
		buffer_pos = 0;

		qbw0 = 0;
		bw0_end_quote = 0;
		bw0_not_end_quote = 0;
		qw0_equals_qbw0 = 0;
		qw0_not_equals_qbw0 = 0;
		bw0_bracket = 0;
		bw0_end_bracket = 0;
		bw0_not_end_bracket = 0;
		w0_bracket_equals_bw0_bracket = 0;
		w0_bracket_not_equals_bw0_bracket = 0;

		is_descendant = 0;
		is_sibling_or_parent = 0;

		int S1 = item.top1;


		if(action.name()==Action::kShift){
			int bw0 =  action.index;
//			buffer_word = item.ref->form(bw0);
			buffer_word = action.word;
			buffer_pos = item.ref->postags.at(bw0);

			if(S0 >= 0){
				if(item.graph->descendants_map_.count(S0)){
					if(item.graph->descendants_map_.at(S0).test(bw0)){
						is_descendant = 1; 	//buffer word is a descendant
					}else{
						is_descendant = 2;
					}
				}

				bool is_sibling = false;
				if(item.graph->siblings_map_.count(S0)){
					if(item.graph->siblings_map_.at(S0).test(bw0)){
						is_sibling = true;
					}
				}
				bool is_parent = false;
				if(item.graph->parent_map_.count(S0)){
					if(item.graph->parent_map_.at(S0).test(bw0)){
						is_parent = true;
					}
				}
				if(is_sibling || is_parent){
					is_sibling_or_parent = 1;	//checking if sibling or parent or function word like ',',that, to
				}else{
					is_sibling_or_parent = 2;
				}
			}

			if(QuoteBracket::kSingleQuoteEnd == item.ref->quote_brackets.at(bw0)){
				bw0_end_quote = 1;
			}else if(QuoteBracket::kDoubleQuoteEnd == item.ref->quote_brackets.at(bw0)){
				bw0_end_quote = 2;
			}
			if(bw0_end_quote == 0){
				bw0_not_end_quote = 1;
			}
			Quoted quote = item.ref->quotes.at(bw0);
			qbw0 = static_cast<int>(quote);

			if(qbw0 == qw0){
				qw0_equals_qbw0 = 1;
			}else{
				qw0_not_equals_qbw0 = 1;
			}

			if(QuoteBracket::kRoundBracketEnd == item.ref->quote_brackets.at(bw0)){
				bw0_end_bracket = 1;
			}else if(QuoteBracket::kCurlyBracketEnd == item.ref->quote_brackets.at(bw0)){
				bw0_end_bracket = 2;
			}
			if(bw0_end_bracket == 0){
				bw0_not_end_bracket = 1;
			}
			Bracket bracket = item.ref->brackets.at(bw0);
			bw0_bracket = static_cast<int>(bracket);
			if(w0_bracket == bw0_bracket){
				w0_bracket_equals_bw0_bracket = 1;
			}else{
				w0_bracket_not_equals_bw0_bracket = 1;
			}

			__SET_LABEL_POS_SHIFTED(parent, bw0);

			__SET_LABEL_POS_SHIFTED(children, bw0);

			__SET_LABEL_POS_SHIFTED(sibling, bw0);

		}else if(action.name()==Action::kInsert){
			buffer_word = word_comma;
			buffer_pos = pos_comma;
		}else if(action.name() == Action::kSplitArc && S0>=0){
			buffer_word = action.word;
			if(buffer_word == word_to){
				buffer_pos = pos_to;
			}else if(buffer_word == word_that){
				buffer_pos = pos_that;
			}else{
				_INFO<<"invalid word for Split Arc";
			}
			is_descendant = 1;
			is_sibling_or_parent = 2;
			parent_bw0_pos_shifted = item.constraint->get_most_frequent_tag(item.ref, S0, item.ref->form(S0));
		}
	}
	int add_word_pos_feature(std::vector<int>* entry_list){
		int prime = 31;
		int feature_value = 0;
		std::sort(entry_list->begin(),entry_list->end());
		for(auto elem:*entry_list){
			feature_value = prime* feature_value + elem;
		}
		return feature_value;
	}

  ScoreContext(const StateItem& item)
    :_is_begin_state(false),
    _has_S1(false),  _has_W1(false), _has_W2(false), 
    S0w(0), S0ldw(0), S0rdw(0), S0lddw(0), S0rddw(0), S0l2dw(0), S0r2dw(0),
    S0p(0), S0ldp(0), S0rdp(0), S0lddp(0), S0rddp(0), S0l2dp(0), S0r2dp(0),
    S0ldl(0), S0rdl(0), S0lddl(0), S0rddl(0), S0l2dl(0), S0r2dl(0),
    S0la(0), S0ra(0),
    S0ls(0), S0rs(0),
    S1w(0), S1ldw(0), S1rdw(0), S1lddw(0), S1rddw(0), S1l2dw(0), S1r2dw(0),
    S1p(0), S1ldp(0), S1rdp(0), S1lddp(0), S1rddp(0), S1l2dp(0), S1r2dp(0),
    S1ldl(0), S1rdl(0), S1lddl(0), S1rddl(0), S1l2dl(0), S1r2dl(0),
    S1la(0), S1ra(0),
    S1ls(0), S1rs(0),

    W0(0), W1(0), W2(0),
    P0(0), P1(0), P2(0), 
    S0S1Dist(0), 
    S0S0ldDist(0), S0S0rdDist(0), S1S1ldDist(0), S1S1rdDist(0), LM1(0), LM2(0), LM3(0), LM4(0), qw0(0), w0_begin_quote(0), w0_not_begin_quote(0), w0_begin_bracket(0), w0_not_begin_bracket(0), w0_bracket(0),
  	  left_arc(0), right_arc(0), all_descendants_shifted(0), parent_S0_shifted(0), parent_S0_not_shifted(0), children_S0_shifted(0), children_S0_not_shifted(0),
	  sibling_S0_shifted(0), sibling_S0_not_shifted(0),
	  children_S0_pos_not_shifted(0), children_S0_pos_shifted(0), sibling_S0_pos_not_shifted(0), sibling_S0_pos_shifted(0),
	  parent_S0_pos_not_shifted(0), parent_S0_pos_shifted(0){
    int N = item.ref->size();
    int S0 = item.top0;

//    const sentence_t& forms = item.ref->forms;

    if (S0 >= 0) {
      _is_begin_state = false;

      S0w = item.words_shifted_map.at(S0);
      S0p = item.postags[S0];
      __SET_CNT(S0);

      if ( _LEGEAL_RANGE_(item.left_most_child[S0]) ) {
        int S0ld = item.left_most_child[S0];
        __SET_CTX_2(S0ld, S0);
        S0S0ldDist = utils::bin(item.rank[S0ld]- item.rank[S0]);

        if ( _LEGEAL_RANGE_(item.left_2nd_most_child[S0]) ) {
          int S0l2d = item.left_2nd_most_child[S0];
          __SET_CTX_2(S0l2d, S0);
        } else {
          __CLEAR_CTX(S0l2d);
        }

        if ( _LEGEAL_RANGE_(item.left_most_child[S0ld]) ) {
          int S0ldd = item.left_most_child[S0ld];
          __SET_CTX_2(S0ldd, S0);
        } else {
          __CLEAR_CTX(S0ldd);
        }
      } else {
        __CLEAR_CTX(S0ld);
        __CLEAR_CTX(S0l2d);
        __CLEAR_CTX(S0ldd);
        S0S0ldDist = 0;
      }

      if ( _LEGEAL_RANGE_(item.right_most_child[S0]) ) {
        int S0rd = item.right_most_child[S0];
        __SET_CTX_2(S0rd, S0);
        S0S0rdDist = utils::bin(item.rank[S0rd] - item.rank[S0]);

        if ( _LEGEAL_RANGE_(item.right_2nd_most_child[S0]) ) {
          int S0r2d = item.right_2nd_most_child[S0];
          __SET_CTX_2(S0r2d, S0);
        } else {
          __CLEAR_CTX(S0l2d);
        }

        if ( _LEGEAL_RANGE_(item.right_most_child[S0rd]) ) {
          int S0rdd = item.right_most_child[S0rd];
          __SET_CTX_2(S0rdd, S0);
        } else {
          __CLEAR_CTX(S0rdd);
        }
      } else {
        __CLEAR_CTX(S0rd);
        __CLEAR_CTX(S0r2d);
        __CLEAR_CTX(S0rdd);
        S0S0rdDist = 0;
      }
    } else {
      // The given state is begin state.
      _is_begin_state = true;
      //
      S0w = eg::TokenAlphabet::BEGIN;
      S0p = eg::TokenAlphabet::BEGIN;
      __CLEAR_CNT(S0);
      __CLEAR_CTX(S0ld);
      __CLEAR_CTX(S0l2d);
      __CLEAR_CTX(S0ldd);
      __CLEAR_CTX(S0rd);
      __CLEAR_CTX(S0r2d);
      __CLEAR_CTX(S0rdd);
    }

    int S1 = item.top1;
    if (item.stack.size() > 2 && S1 >= 0) {
      _has_S1 = true;

//      S1w = forms.at(S1);
      S1w = item.words_shifted_map.at(S1);
      S1p = item.postags[S1];
      __SET_CNT(S1);
      S0S1Dist = utils::bin(item.rank[S1]- item.rank[S0]);

      if ( _LEGEAL_RANGE_(item.left_most_child[S1]) ) {
        int S1ld = item.left_most_child[S1];
        __SET_CTX_2(S1ld, S1);
        S1S1ldDist = utils::bin(item.rank[S1ld]- item.rank[S1]);
        
        if ( _LEGEAL_RANGE_(item.left_2nd_most_child[S1]) ) {
          int S1l2d = item.left_2nd_most_child[S1];
          __SET_CTX_2(S1l2d, S1);
        } else {
          __CLEAR_CTX(S1l2d);
        }

        if ( _LEGEAL_RANGE_(item.left_most_child[S1ld]) ) {
          int S1ldd = item.left_most_child[S1ld];
          __SET_CTX_2(S1ldd, S1);
        } else {
          __CLEAR_CTX(S1ldd);
        }
      } else {
        __CLEAR_CTX(S1ld);
        __CLEAR_CTX(S1l2d);
        __CLEAR_CTX(S1ldd);
        S1S1ldDist = 0;
      }

      if ( _LEGEAL_RANGE_(item.right_most_child[S1]) ) {
        int S1rd = item.right_most_child[S1];
        __SET_CTX_2(S1rd, S1);
        S1S1rdDist = utils::bin(item.rank[S1rd]- item.rank[S1]);
        if ( _LEGEAL_RANGE_(item.right_2nd_most_child[S1]) ) {
          int S1r2d = item.right_2nd_most_child[S1];
          __SET_CTX_2(S1r2d, S1);
        } else {
          __CLEAR_CTX(S1r2d);
        }

        if ( _LEGEAL_RANGE_(item.right_most_child[S1rd]) ) {
          int S1rdd = item.right_most_child[S1rd];
          __SET_CTX_2(S1rdd, S1);
        } else {
          __CLEAR_CTX(S1rdd);
        }
      } else {
        __CLEAR_CTX(S1rd);
        __CLEAR_CTX(S1r2d);
        __CLEAR_CTX(S1rdd);
        S1S1rdDist = 0;
      }
    } else {
      _has_S1 = false;
      S1w = kNoneWord;
      S1p = kNonePostag;
      __CLEAR_CNT(S1);
      __CLEAR_CTX(S1ld);
      __CLEAR_CTX(S1l2d);
      __CLEAR_CTX(S1ldd);
      __CLEAR_CTX(S1rd);
      __CLEAR_CTX(S1r2d);
      __CLEAR_CTX(S1rdd);
      S0S1Dist = 0;
    }

    // SHIFTED words
    // item.word_sequence.size() should be equal to item.postag_sequence.size()
    int C = item.word_sequence.size();

    _has_W1 = false;
    _has_W2 = false;
    W0 = item.word_sequence[C - 1];
    P0 = item.postag_sequence[C - 1];
    if (C > 2) {
      _has_W1 = true;
      W1 = item.word_sequence[C - 2];
      P1 = item.postag_sequence[C - 2];	  
      if (C > 3) {
		_has_W2 = true;
        W2 = item.word_sequence[C - 3];
        P2 = item.postag_sequence[C - 3];
      } else {
        W2 = kNoneWord;
        P2 = kNonePostag;
      }
    } else {
      W1 = (W2 = kNoneWord);
      P1 = (P2 = kNonePostag);
    } 

    int words_shifted_size = item.words_shifted.size();
	if(words_shifted_size>0){
		int ws0 = item.words_shifted[words_shifted_size - 1];
		if(QuoteBracket::kSingleQuoteBegin == item.ref->quote_brackets.at(ws0)){
			w0_begin_quote = 1;
		}else if(QuoteBracket::kDoubleQuoteBegin == item.ref->quote_brackets.at(ws0)){
			w0_begin_quote = 2;
		}

		if(w0_begin_quote ==0){
			w0_not_begin_quote = 1;
		}

		Quoted quote = item.ref->quotes.at(ws0);
		qw0 = static_cast<int>(quote);
		qw0_quote = quote;



		if(QuoteBracket::kRoundBracketBegin == item.ref->quote_brackets.at(ws0)){
			w0_begin_bracket = 1;
		}else if(QuoteBracket::kCurlyBracketBegin == item.ref->quote_brackets.at(ws0)){
			w0_begin_bracket = 2;
		}

		if(w0_begin_bracket ==0){
			w0_not_begin_bracket = 1;
		}
		Bracket bracket = item.ref->brackets.at(ws0);
		w0_bracket_value = bracket;
		w0_bracket = static_cast<int>(bracket);
	}

	if(has_S1()){
		if(item.graph->children_map.count(S0) >0){
			if(item.graph->arc(S0,S1)){
				left_arc = 1;
			}else{
				left_arc = 2;
			}
		}
		if(item.graph->children_map.count(S1) >0){
			if(item.graph->arc(S1, S0)){
				right_arc = 1;
			}else{
				right_arc = 2;
			}
		}
	}

	if (S0 >= 0){
		all_descendants_shifted = 2;		//all descendants are shifted;
		if(item.graph->descendants_map_.count(S0)>0) {
//			std::bitset<kMaxNumberOfWords> and_output = item.graph->descendants_map_.at(S0)& item.buffer;
			if((item.graph->descendants_map_.at(S0)& item.buffer).any()){
				all_descendants_shifted = 1;	//there is atleast one descendant which is not shifted yet
			}
		}
	}

		__SET_LABEL_POS_SHIFTED(parent, S0);

		__SET_LABEL_POS_SHIFTED(children, S0);

		__SET_LABEL_POS_SHIFTED(sibling, S0);
  } 

  bool is_begin_state() const {
    return _is_begin_state;
  }

  bool has_S1() const {
    return _has_S1;
  }

  bool has_W1() const {
    return _has_W1;
  }

  bool has_W2() const {
    return _has_W2;
  }


  bool      _is_begin_state;
  bool      _has_S1, _has_W1, _has_W2;

  word_t    S0w, S0ldw, S0rdw, S0lddw, S0rddw, S0l2dw, S0r2dw;
  postag_t  S0p, S0ldp, S0rdp, S0lddp, S0rddp, S0l2dp, S0r2dp;
  deprel_t       S0ldl, S0rdl, S0lddl, S0rddl, S0l2dl, S0r2dl;
  int       S0la, S0ra;
  int       S0ls, S0rs;

  word_t    S1w, S1ldw, S1rdw, S1lddw, S1rddw, S1l2dw, S1r2dw;
  postag_t  S1p, S1ldp, S1rdp, S1lddp, S1rddp, S1l2dp, S1r2dp;
  deprel_t       S1ldl, S1rdl, S1lddl, S1rddl, S1l2dl, S1r2dl;
  int       S1la, S1ra;
  int       S1ls, S1rs;
  
  word_t    W0, W1, W2;
  postag_t  P0, P1, P2;

  int       S0S1Dist;
  int       S0S0ldDist, S0S0rdDist, S1S1ldDist, S1S1rdDist;

  int 		LM1, LM2, LM3, LM4;
  int parent_bw0_shifted = 0;
  int parent_bw0_not_shifted = 0;
  int parent_S0_shifted = 0;
  int parent_S0_not_shifted = 0;
  int parent_bw0_pos_shifted = 0;
  int parent_bw0_pos_not_shifted = 0;
  int parent_S0_pos_shifted = 0;
  int parent_S0_pos_not_shifted = 0;
  int children_bw0_not_shifted = 0;
  int children_bw0_shifted = 0;
  int children_bw0_pos_not_shifted = 0;
  int children_bw0_pos_shifted = 0;
  int children_S0_not_shifted = 0;
  int children_S0_shifted = 0;
  int children_S0_pos_not_shifted = 0;
  int children_S0_pos_shifted = 0;
  int sibling_bw0_shifted = 0;
  int sibling_bw0_not_shifted = 0;
  int sibling_S0_shifted = 0;
	int sibling_S0_not_shifted = 0;
  int sibling_bw0_pos_shifted = 0;
  int sibling_bw0_pos_not_shifted = 0;
  int sibling_S0_pos_shifted = 0;
	int sibling_S0_pos_not_shifted = 0;
  int buffer_word = 0;
  int buffer_pos = 0;

  int qw0 = 0;
  int qbw0 = 0;
  int w0_begin_quote = 0;
  int w0_not_begin_quote = 0;
  int bw0_end_quote = 0;
  int bw0_not_end_quote = 0;
  int qw0_equals_qbw0 = 0;
  int qw0_not_equals_qbw0 = 0;
  Quoted qw0_quote = Quoted::kNone;
  Bracket w0_bracket_value = Bracket::kNone;
  int w0_begin_bracket = 0;
  int w0_not_begin_bracket = 0;
  int bw0_end_bracket = 0;
  int bw0_not_end_bracket = 0;
  int w0_bracket = 0;
  int bw0_bracket = 0;
  int w0_bracket_equals_bw0_bracket = 0;
  int w0_bracket_not_equals_bw0_bracket = 0;
  int left_arc = 0;
  int right_arc = 0;
  int all_descendants_shifted = 0;
  int is_descendant = 0;
  int is_sibling_or_parent = 0;
  };

typedef ScoreContext ctx_t;

} //  end for 
} //  end for

#undef __SET_CTX
#undef __SET_CTX_2
#undef __CLEAR_CTX
#undef __SET_CNT
#undef __CLEAR_CNT

#endif  //  __ZGEN_SHIFTREDUCE_MODEL_SCORE_CONTEXT_H__
