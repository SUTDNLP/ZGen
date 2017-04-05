#ifndef __ZGEN_SHIFTREDUCE_GRAPH_DECODER_H__
#define __ZGEN_SHIFTREDUCE_GRAPH_DECODER_H__

#include "decoder.h"
#include "types/tree.h"

namespace ZGen {
namespace ShiftReduce {

class GraphDecoder : public Decoder {
public:
  GraphDecoder(const option_t& opts,
      DecodeContext* ctx,
      Model* model,
      const Knowledge::PostagConstraint* constraint,
      const Engine::TokenAlphabet& forms_alphabet,
	  int tag, int nr_relations);

private:
  /**
   * Config the input onto the decoder.
   *
   *  @param[in]  input       The pointer to the input instance.
   */
  void config_input(const dependency_t* input);

  /**
   * Get the possible actions from the current state. Store the possible
   * actions in the possible_actions (action_collection_t)
   *
   *  @param[in]  item        The input item
   */
  void get_possible_actions(const StateItem& item);

  int get_distance(const graph_t * graph, word_t top0, word_t word);

  Action::ACTION_TYPE reduce(const StateItem& item, const graph_t* graph);

  void all_shift_reduce(int N, const StateItem& item);

  bool is_parent_ancestor_of_next_element(int node, const StateItem& item, const graph_t* graph);

  void all_shift(int N, const StateItem& item);
  void shift_nodes(int top0, const graph::edgeset_t& descendants,
			const StateItem& item);
	bool is_left_arc(int stack_top0, int stack_top1, const graph_t* graph, const StateItem& item);
	bool is_right_arc(int stack_top0, int stack_top1, const graph_t* graph,
			const StateItem& item);
	void make_split_arc(const graph_t* graph, int top0, const StateItem& item);
	std::set<int> get_direct_children(const graph_t* graph, const StateItem& item, int top0);
	bool is_any_direct_children_not_shifted(
			const std::set<int>& direct_children, const StateItem& item);
	std::set<int> get_inflection_nodes(const graph_t* graph, const StateItem& item);
	void shift_parent_and_siblings(graph::edgeset_t& shift_candidates, std::set<int>& valid_parents, int parent, const graph_t* graph, const StateItem& item);
	void shift_parent_siblings_in_sibling_relationship(const graph_t* graph,
			int top0, const StateItem& item, const std::set<int>& inflections,
			graph::edgeset_t& shift_candidates, std::set<int>& valid_parents);
	void shift_parents_siblings_single_stack_item(
			const graph::edgeset_t& parents, const StateItem& item,
			const graph_t* graph, graph::edgeset_t& shift_candidates,
			std::set<int>& valid_parents);
	void shift_parents_siblings_descendant_relationship(
			const StateItem& item,
			const graph_t* graph, int top1, graph::edgeset_t& shift_candidates,
			std::set<int>& valid_parents);
	void insert_comma(const std::set<int>& valid_parents, bool shift_descendants,
			const StateItem& item);
	void split_arc_descendants(const StateItem& item,
			graph::edgeset_t shift_candidates, const graph_t* graph, int top0);

  //!
  DependencyTree tree;
};

} //  end for namespace ShiftReduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_FULL_DECODER_H__
