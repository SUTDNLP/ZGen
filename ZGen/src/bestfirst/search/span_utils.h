#ifndef __ZGEN_BESTFIRST_SEARCH_SPAN_UTILS_H__
#define __ZGEN_BESTFIRST_SEARCH_SPAN_UTILS_H__

#include <iostream>
#include "settings.h"
#include "bestfirst/search/span.h"

namespace ZGen {

namespace BestFirst {

class SpanUtils {
public:
  typedef std::pair<int, int> relation_t;

  /**
   * Get all the dependency relations from the span.
   *
   *  @param[in]  span      The input span.
   *  @param[out] relations The output relations.
   */
  static void get_all_relations(const Span* span,
      std::vector<relation_t>& relations);

  /**
   * Get all the (head, left modifier) relations from the spans.
   *
   *  @param[in]  span      The input span.
   *  @param[out] relations The output relations.
   */
  static void get_all_left_relations(const Span* span,
      std::vector<relation_t>& relations);

  /**
   * Get all the (head, right modifier) relations from the spans.
   *
   *  @param[in]  span      The input span.
   *  @param[out] relations The output relations.
   */
  static void get_all_right_relations(const Span* span,
      std::vector<relation_t>& relations);

  /**
   * Get all the modifying words for the span.
   *
   *  @param[in]  span            The input span.
   *  @param[out] modifying_words The output modifying words.
   */
  static void get_all_modifier_words(const Span* span,
      std::vector<word_t>& modifying_words);

  /**
   *
   *
   *
   */
  static void get_all_linearized_indices(const Span* span,
      std::vector<int>& indices);

  /**
   * Get all the modifying postags for the span.
   *
   *  @param[in]  span              The input span.
   *  @param[out] modifying_postags The output modifying postags.
   */
  static void get_all_modifier_postags(const Span* span,
      std::vector<postag_t>& modifying_postags);

  /**
   * Get all the words from the span.
   *
   *  @param[in]  span      The input span.
   *  @param[out] words     The output word sequence.
   */
  static void get_all_linearized_words(const Span* span,
      std::vector<word_t>& words);

  /**
   * Get the linearized postags from the span.
   *
   *  @param[in]  span      The input span.
   *  @param[out] postags   The output postags.
   */
  static void get_all_linearized_postags(const Span* span,
      std::vector<postag_t>& postags);
private:
  /**
   * Perform the pre-order travel to extract the relations
   * from the data.
   *
   *  @param[in]  now       The currently visited span.
   *  @param[out] relations The output relations.
   */
  static void get_all_relations_travel(const Span* now,
      std::vector<relation_t>& relations);

  /**
   * Get all the (head, left modifier) relations from the spans.
   *
   *  @param[in]  span      The input span.
   *  @param[out] relations The output relations.
   */
  static void get_all_left_relations_travel(const Span* span,
      std::vector<relation_t>& relations);

  /**
   * Get all the (head, right modifier) relations from the spans.
   *
   *  @param[in]  span      The input span.
   *  @param[out] relations The output relations.
   */
  static void get_all_right_relations_travel(const Span* span,
      std::vector<relation_t>& relations);

  /**
   * Get all the words from the span.
   *
   *  @param[in]  span      The input span
   *  @param[out] words     The output word sequence.
   */
  static void get_all_linearized_indices_travel(const Span* now,
      std::vector<int>& indices);
};

} //  end for namespace BestFirst
} //  end for namespace ZGen

#endif  //  end for __ZGEN_BESTFIRST_SEARCH_SPAN_UTILS_H__
