#ifndef __ZGEN_BESTFIRST_SPAN_H__
#define __ZGEN_BESTFIRST_SPAN_H__

#include <bitset>
#include "settings.h"
#include "types/instance.h"

namespace ZGen {

namespace BestFirst {

// Span is flexiable, memory statable.
struct Span {
  Span();

  /**
   * Initialize the span with input information, including set the
   * init flag to true.
   *
   *  @param[in]  ref     The reference to the input dependency.
   *  @param[in]  root_id The index to the root.
   *  @param[in]  tag     The postag.
   */
  void init(const dependency_t* ref,
      int root_id,
      postag_t tag);


  /**
   * Copy the context of the other span into this span.
   *
   *  @param[in] other   The other span.
   */
  void copy(const Span *other);


  /**
   * Clear the information stored in the span, including clear the left
   * and right spans, set the initialized flag false.
   */
  void clear();


  /**
   * Return if the span is a complete span. A complete span is 
   *
   *  @return bool  Return true if the span is a complete span, otherwise
   *                return false.
   */
  bool complete();


  /**
   * Link the left modifier span into the current span.
   *
   *  @param[in]  modifier  The left modifier.
   *  @return     bool      Return true if successfully linked, otherwise false.
   */
  bool link_left(const Span *modifier);


  /**
   * Link the right modifier span into the current span.
   *
   *  @param[in]  modifier  The right modifier.
   *  @return     bool      Return true if successfully linked, otherwise false.
   */
  bool link_right(const Span *modifier);

  /**
   * Represent the span in string.
   *
   *  @param[out] s         The output string.
   */
  void repr(std::string& s) const;

  //!
  friend std::ostream& operator << (std::ostream& os, const Span& span);

  bool initialized;                   /*!< The initialized flag */
  const dependency_t* ref;            /*!< The reference to the input */
  int root_id;                        /*!< The index for the root */
  postag_t tag;                       /*!< The postag tag to the root of the span */
  floatval_t  score;                  /*!< The score of the current span */
  std::vector<const Span *> left_spans;
                                      /*!< The left modifiers */
  std::vector<const Span *> right_spans;
                                      /*!< The right modifiers */
  std::bitset<kMaxNumberOfWords> used_words;
                                      /*!< The used words */
};

struct SpanPtrGreater {
  bool operator () (const Span* lhs, const Span* rhs) const {
    return lhs->score > rhs->score;
  }
};

struct SpanPtrLess {
  bool operator () (const Span* lhs, const Span* rhs) const {
    return lhs->score < rhs->score;
  }
};

struct SpanPtrEqual {
  bool operator () (const Span* x, const Span* y) const {
    std::string xrepr; xrepr.reserve(512); x->repr(xrepr);
    std::string yrepr; yrepr.reserve(512); y->repr(yrepr);

    return xrepr == yrepr;
  }
};

struct SpanPtrHash {
  size_t operator () (const Span* x) const {
    std::string xrepr; xrepr.reserve(512); x->repr(xrepr);
    return boost::hash_value<std::string>(xrepr);
  }
};

} //  end for namespace Bestfirst

} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_SPAN_H__
