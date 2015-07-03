#ifndef __ZGEN_SHIFTREDUCE_MODEL_SCORE_CONTEXT_H__
#define __ZGEN_SHIFTREDUCE_MODEL_SCORE_CONTEXT_H__

#include "engine/token_alphabet.h"
#include "shiftreduce/types/state.h"
#include "utils/math_utils.h"   //  for bin

#define _LEGEAL_RANGE_(x) (((x) >= 0) && ((x) < N))

#define __SET_CTX(prefix) do { \
  prefix##w = (item.ref->forms).at(prefix); \
  prefix##p = item.postags[prefix]; \
  prefix##l = item.deprels[prefix]; \
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


namespace ZGen {

namespace ShiftReduce {

namespace eg = ZGen::Engine;
namespace utils= ZGen::Utility;

const int kNoneWord = eg::TokenAlphabet::NONE;
const int kNonePostag = eg::TokenAlphabet::NONE;
const int kNoneDeprel = eg::TokenAlphabet::NONE;

struct ScoreContext {
  ScoreContext(const StateItem& item)
    :_is_begin_state(false),
    _has_S1(false), /*_has_S2(false), _has_S3(false),*/  _has_W1(false), _has_W2(false), _has_W3(false), _has_W4(false),
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

/*	S2w(0), S2ldw(0), S2rdw(0), S2lddw(0), S2rddw(0), S2l2dw(0), S2r2dw(0),
    S2p(0), S2ldp(0), S2rdp(0), S2lddp(0), S2rddp(0), S2l2dp(0), S2r2dp(0),
    S2ldl(0), S2rdl(0), S2lddl(0), S2rddl(0), S2l2dl(0), S2r2dl(0),
    S2la(0), S2ra(0),
    S2ls(0), S2rs(0),

	S3w(0), S3ldw(0), S3rdw(0), S3lddw(0), S3rddw(0), S3l2dw(0), S3r2dw(0),
    S3p(0), S3ldp(0), S3rdp(0), S3lddp(0), S3rddp(0), S3l2dp(0), S3r2dp(0),
    S3ldl(0), S3rdl(0), S3lddl(0), S3rddl(0), S3l2dl(0), S3r2dl(0),
    S3la(0), S3ra(0),
    S3ls(0), S3rs(0),
*/
    W0(0), W1(0), W2(0), W3(0), W4(0),
    P0(0), P1(0), P2(0), P3(0), P4(0),
    S0S1Dist(0), /*S1S2Dist(0), S2S3Dist(0),*/
    S0S0ldDist(0), S0S0rdDist(0), S1S1ldDist(0), S1S1rdDist(0),/* S2S2ldDist(0), S2S2rdDist(0), S3S3ldDist(0), S3S3rdDist(0),*/ LM1(0), LM2(0), LM3(0), LM4(0){
    int N = item.ref->size();
    int S0 = item.top2;

    const sentence_t& forms = item.ref->forms;

    if (S0 >= 0) {
      _is_begin_state = false;

      S0w = forms.at(S0);
      S0p = item.postags[S0];
      __SET_CNT(S0);

      if ( _LEGEAL_RANGE_(item.left_most_child[S0]) ) {
        int S0ld = item.left_most_child[S0];
        __SET_CTX(S0ld);
        S0S0ldDist = utils::bin(item.rank[S0ld]- item.rank[S0]);

        if ( _LEGEAL_RANGE_(item.left_2nd_most_child[S0]) ) {
          int S0l2d = item.left_2nd_most_child[S0];
          __SET_CTX(S0l2d);
        } else {
          __CLEAR_CTX(S0l2d);
        }

        if ( _LEGEAL_RANGE_(item.left_most_child[S0ld]) ) {
          int S0ldd = item.left_most_child[S0ld];
          __SET_CTX(S0ldd);
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
        __SET_CTX(S0rd);
        S0S0rdDist = utils::bin(item.rank[S0rd] - item.rank[S0]);

        if ( _LEGEAL_RANGE_(item.right_2nd_most_child[S0]) ) {
          int S0r2d = item.right_2nd_most_child[S0];
          __SET_CTX(S0r2d);
        } else {
          __CLEAR_CTX(S0l2d);
        }

        if ( _LEGEAL_RANGE_(item.right_most_child[S0rd]) ) {
          int S0rdd = item.right_most_child[S0rd];
          __SET_CTX(S0rdd);
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

	int S1 = item.top3;
    if (item.stack.size() > 4 && S1 >= 0) {
      _has_S1 = true;

      S1w = forms.at(S1);
      S1p = item.postags[S1];
      __SET_CNT(S1);
      S0S1Dist = utils::bin(item.rank[S1]- item.rank[S0]);

      if ( _LEGEAL_RANGE_(item.left_most_child[S1]) ) {
        int S1ld = item.left_most_child[S1];
        __SET_CTX(S1ld);
        S1S1ldDist = utils::bin(item.rank[S1ld]- item.rank[S1]);
        
        if ( _LEGEAL_RANGE_(item.left_2nd_most_child[S1]) ) {
          int S1l2d = item.left_2nd_most_child[S1];
          __SET_CTX(S1l2d);
        } else {
          __CLEAR_CTX(S1l2d);
        }

        if ( _LEGEAL_RANGE_(item.left_most_child[S1ld]) ) {
          int S1ldd = item.left_most_child[S1ld];
          __SET_CTX(S1ldd);
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
        __SET_CTX(S1rd);
        S1S1rdDist = utils::bin(item.rank[S1rd]- item.rank[S1]);
        if ( _LEGEAL_RANGE_(item.right_2nd_most_child[S1]) ) {
          int S1r2d = item.right_2nd_most_child[S1];
          __SET_CTX(S1r2d);
        } else {
          __CLEAR_CTX(S1r2d);
        }

        if ( _LEGEAL_RANGE_(item.right_most_child[S1rd]) ) {
          int S1rdd = item.right_most_child[S1rd];
          __SET_CTX(S1rdd);
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
	/*int S2 = item.top2;
    if (item.stack.size() > 3 && S2 >=0) {
      _has_S2 = true;

      S2w = forms.at(S2);
      S2p = item.postags[S2];
      __SET_CNT(S2);
      S1S2Dist = utils::bin(item.rank[S2]- item.rank[S1]);

      if ( _LEGEAL_RANGE_(item.left_most_child[S2]) ) {
        int S2ld = item.left_most_child[S2];
        __SET_CTX(S2ld);
        S2S2ldDist = utils::bin(item.rank[S2ld]- item.rank[S2]);
        
        if ( _LEGEAL_RANGE_(item.left_2nd_most_child[S2]) ) {
          int S2l2d = item.left_2nd_most_child[S2];
          __SET_CTX(S2l2d);
        } else {
          __CLEAR_CTX(S2l2d);
        }

        if ( _LEGEAL_RANGE_(item.left_most_child[S2ld]) ) {
          int S2ldd = item.left_most_child[S2ld];
          __SET_CTX(S2ldd);
        } else {
          __CLEAR_CTX(S2ldd);
        }
      } else {
        __CLEAR_CTX(S2ld);
        __CLEAR_CTX(S2l2d);
        __CLEAR_CTX(S2ldd);
        S2S2ldDist = 0;
      }

      if ( _LEGEAL_RANGE_(item.right_most_child[S2]) ) {
        int S2rd = item.right_most_child[S2];
        __SET_CTX(S2rd);
        S2S2rdDist = utils::bin(item.rank[S2rd]- item.rank[S2]);
        if ( _LEGEAL_RANGE_(item.right_2nd_most_child[S2]) ) {
          int S2r2d = item.right_2nd_most_child[S2];
          __SET_CTX(S2r2d);
        } else {
          __CLEAR_CTX(S2r2d);
        }

        if ( _LEGEAL_RANGE_(item.right_most_child[S2rd]) ) {
          int S2rdd = item.right_most_child[S2rd];
          __SET_CTX(S2rdd);
        } else {
          __CLEAR_CTX(S2rdd);
        }
      } else {
        __CLEAR_CTX(S2rd);
        __CLEAR_CTX(S2r2d);
        __CLEAR_CTX(S2rdd);
        S2S2rdDist = 0;
      }
    } else {
      _has_S2 = false;
      S2w = kNoneWord;
      S2p = kNonePostag;
      __CLEAR_CNT(S2);
      __CLEAR_CTX(S2ld);
      __CLEAR_CTX(S2l2d);
      __CLEAR_CTX(S2ldd);
      __CLEAR_CTX(S2rd);
      __CLEAR_CTX(S2r2d);
      __CLEAR_CTX(S2rdd);
      S1S2Dist = 0;
	}

	int S3 = item.top3;
    if (item.stack.size() > 4 && S3 >= 0) {
      _has_S3 = true;

      S3w = forms.at(S3);
      S3p = item.postags[S3];
      __SET_CNT(S3);
      S2S3Dist = utils::bin(item.rank[S3]- item.rank[S2]);

      if ( _LEGEAL_RANGE_(item.left_most_child[S3]) ) {
        int S3ld = item.left_most_child[S3];
        __SET_CTX(S3ld);
        S3S3ldDist = utils::bin(item.rank[S3ld]- item.rank[S3]);
        
        if ( _LEGEAL_RANGE_(item.left_2nd_most_child[S3]) ) {
          int S3l2d = item.left_2nd_most_child[S3];
          __SET_CTX(S3l2d);
        } else {
          __CLEAR_CTX(S3l2d);
        }

        if ( _LEGEAL_RANGE_(item.left_most_child[S3ld]) ) {
          int S3ldd = item.left_most_child[S3ld];
          __SET_CTX(S3ldd);
        } else {
          __CLEAR_CTX(S3ldd);
        }
      } else {
        __CLEAR_CTX(S3ld);
        __CLEAR_CTX(S3l2d);
        __CLEAR_CTX(S3ldd);
        S3S3ldDist = 0;
      }

      if ( _LEGEAL_RANGE_(item.right_most_child[S3]) ) {
        int S3rd = item.right_most_child[S3];
        __SET_CTX(S3rd);
        S3S3rdDist = utils::bin(item.rank[S3rd]- item.rank[S3]);
        if ( _LEGEAL_RANGE_(item.right_2nd_most_child[S3]) ) {
          int S3r2d = item.right_2nd_most_child[S3];
          __SET_CTX(S3r2d);
        } else {
          __CLEAR_CTX(S3r2d);
        }

        if ( _LEGEAL_RANGE_(item.right_most_child[S3rd]) ) {
          int S3rdd = item.right_most_child[S3rd];
          __SET_CTX(S3rdd);
        } else {
          __CLEAR_CTX(S3rdd);
        }
      } else {
        __CLEAR_CTX(S3rd);
        __CLEAR_CTX(S3r2d);
        __CLEAR_CTX(S3rdd);
        S3S3rdDist = 0;
      }
    } else {
      _has_S3 = false;
      S3w = kNoneWord;
      S3p = kNonePostag;
      __CLEAR_CNT(S3);
      __CLEAR_CTX(S3ld);
      __CLEAR_CTX(S3l2d);
      __CLEAR_CTX(S3ldd);
      __CLEAR_CTX(S3rd);
      __CLEAR_CTX(S3r2d);
      __CLEAR_CTX(S3rdd);
      S2S3Dist = 0;
    }
*/


    // SHIFTED words
    // item.word_sequence.size() should be equal to item.postag_sequence.size()
    int C = item.word_sequence.size();

	_has_W1 = false;
	_has_W2 = false;

    W0 = item.word_sequence[C - 1];
    P0 = item.postag_sequence[C - 1];

	_has_W3 = false;
	_has_W4 = false;
    if (C > 2) {
      _has_W1 = true;
      W1 = item.word_sequence[C - 2];
      P1 = item.postag_sequence[C - 2];	  
      if (C > 3) {
		_has_W2 = true;
        W2 = item.word_sequence[C - 3];
        P2 = item.postag_sequence[C - 3];

		if (C > 4) {
		  _has_W3 = true;
		  W3 = item.word_sequence[C - 4];
		  P3 = item.postag_sequence[C - 4];

		  if (C > 5) {
			_has_W4 = true;
			W4 = item.word_sequence[C - 5];
			P4 = item.postag_sequence[C - 5];
		  }
		  else {
			W4 = kNoneWord;
			P4 = kNonePostag;
		  }
		}
		else {
		  W3 = (W4 = kNoneWord);
		  P3 = (P4 = kNonePostag);
		}
      } else {
        W2 = (W3 = (W4 = kNoneWord));
        P2 = (P3 = (P4 = kNonePostag));
//        W2 = kNoneWord;
//        P2 = kNonePostag;
      }
    } else {
      W1 = (W2 = (W3 = (W4 = kNoneWord)));
      P1 = (P2 = (P3 = (P4 = kNonePostag)));
//    	W1 = (W2 = kNoneWord);
//    	P1 = (P2 = kNonePostag);
    } 
  } 
//  else {
//		W0 = (W1 = (W2 = kNoneWord));
//		P0 = (P1
//  }
// }

  bool is_begin_state() const {
    return _is_begin_state;
  }

  bool has_S1() const {
    return _has_S1;
  }

  /*bool has_S2() const {
	return _has_S2;
  }

  bool has_S3() const {
	return _has_S3;
  }*/

  bool has_W1() const {
    return _has_W1;
  }

  bool has_W2() const {
    return _has_W2;
  }

  bool has_W3() const {
	return _has_W3;
  }

  bool has_W4() const {
	return _has_W4;
  }

  bool      _is_begin_state;
  bool      _has_S1, /*_has_S2, _has_S3,*/ _has_W1, _has_W2, _has_W3, _has_W4;

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
/*
  word_t    S2w, S2ldw, S2rdw, S2lddw, S2rddw, S2l2dw, S2r2dw;
  postag_t  S2p, S2ldp, S2rdp, S2lddp, S2rddp, S2l2dp, S2r2dp;
  deprel_t       S2ldl, S2rdl, S2lddl, S2rddl, S2l2dl, S2r2dl;
  int       S2la, S2ra;
  int       S2ls, S2rs;

  word_t    S3w, S3ldw, S3rdw, S3lddw, S3rddw, S3l2dw, S3r2dw;
  postag_t  S3p, S3ldp, S3rdp, S3lddp, S3rddp, S3l2dp, S3r2dp;
  deprel_t       S3ldl, S3rdl, S3lddl, S3rddl, S3l2dl, S3r2dl;
  int       S3la, S3ra;
  int       S3ls, S3rs;
*/
  word_t    W0, W1, W2, W3, W4;
  postag_t  P0, P1, P2, P3, P4;

  int       S0S1Dist/*, S1S2Dist, S2S3Dist*/;
  int       S0S0ldDist, S0S0rdDist, S1S1ldDist, S1S1rdDist/*, S2S2ldDist, S2S2rdDist, S3S3ldDist, S3S3rdDist*/;

  int 		LM1, LM2, LM3, LM4;
};

typedef ScoreContext ctx_t;

} //  end for 
} //  end for

#undef __SET_CTX
#undef __CLEAR_CTX
#undef __SET_CNT
#undef __CLEAR_CNT

#endif  //  __ZGEN_SHIFTREDUCE_MODEL_SCORE_CONTEXT_H__
