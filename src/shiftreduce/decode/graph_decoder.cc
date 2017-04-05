#include "shiftreduce/decode/graph_decoder.h"
#include "shiftreduce/types/action_utils.h"
#include <queue>
#include <deque>
#include <algorithm>
#include <vector>
namespace ZGen {
namespace ShiftReduce {

namespace kn = ZGen::Knowledge;
//namespace split = ZGen::ShiftReduce::Action::SPLIT_ARC_TAG;
extern int sentence_no;
extern bool learn_mode;
extern int word_comma;
extern int word_that;
extern int word_to;
extern action_sequence_t decoder_gold_actions;
extern int step_no;
extern int const_sentence_no;
std::set<int> coverage_vector;
extern std::set<int> subtree_root_node_that;
extern std::set<int> gold_previous_words;

GraphDecoder::GraphDecoder(const option_t& _opts,
    DecodeContext* _ctx,
    Model* _model,
    const kn::PostagConstraint* _constraint,
    const Engine::TokenAlphabet& _forms_alphabet,
	int _tag, int _nr_relations)
  : Decoder(_opts, _ctx, _model, 
			_constraint, _forms_alphabet, _tag, _nr_relations) {
}

void
GraphDecoder::config_input(const dependency_t* input) {
  // leave it empty.
//	if(learn_mode){
//		tree.set_ref(input);
//	}
}

int GraphDecoder::get_distance(const graph_t * graph, int top0, int word){
	if(graph->path_map_exists(top0,word)){
	  boost::tuples::tuple<int,Direction,int> path_record = graph->get_path_map_entry(top0, word);
	  int distance = path_record.get<0>();
	  if(distance < 6){
		  return distance;
	  }
	}
	return -1;
}

bool GraphDecoder::is_left_arc(int stack_top0, int stack_top1,
		const graph_t* graph, const StateItem& item) {
	if(stack_top1 >= 0 && graph->has_children(stack_top0) && graph->arc(stack_top0, stack_top1)){
		return true;
	}
	return false;
}

bool GraphDecoder::is_right_arc(int stack_top0, int stack_top1,
		const graph_t* graph, const StateItem& item) {
	/**
	 * Case IX stack_top1 has a left child so all descendants of stack_top1 are shifted
	 * so we cannot do right arc now
	 */
	if(item.nr_left_children[stack_top1] > 0){
		return false;
	}
	if(stack_top1 >= 0 && graph->has_children(stack_top1)
			&& graph->arc(stack_top1, stack_top0)){
		return true;
	}
	return false;
}

Action::ACTION_TYPE GraphDecoder::reduce(const StateItem& item, const graph_t* graph) {
	int reduce = -1;
	Action rightArc = ActionFactory::make_right_arc();
	Action leftArc = ActionFactory::make_left_arc();
	int stack_top1 = item.top1;
	int stack_top0 = item.top0;
	if (is_left_arc(stack_top0, stack_top1, graph, item)) {
		possible_actions.push_back(leftArc); //left arc between tree nodes
		return Action::kLeftArc;
	} else if (is_right_arc(stack_top0, stack_top1, graph, item)) {
		possible_actions.push_back(rightArc); //right arc between tree nodes
		return Action::kRightArc;
	}
	return Action::kNone;
}

void GraphDecoder::all_shift_reduce(int N, const StateItem& item) {
	for (int j = 0; j < N; ++j) {
		if (item.buffer.test(j)) {
			word_t word = item.ref->forms[j];
			postag_t tag = item.ref->postags[j];
			get_possible_shift_actions(item, j, word, tag);
		}
	}
	if (item.stack.size() > 2) {
		possible_actions.push_back(ActionFactory::make_right_arc());
		possible_actions.push_back(ActionFactory::make_left_arc());
	}
}

bool GraphDecoder::is_parent_ancestor_of_next_element(int parent, const StateItem& item, const graph_t* graph){
	auto result = std::find(item.stack.begin(), item.stack.end(), parent);
    int nPosition = std::distance(item.stack.begin(), result);
	bool ancestor = false;
	for(int index=nPosition; index< item.stack.size()-1; index++){
		int next_element = item.stack.at(index+1);
		if(graph->is_descendant(parent, next_element)){
			ancestor = true;
		}else{
			ancestor = false;
			break;
		}
	}
	return ancestor;
}

void GraphDecoder::all_shift(int N, const StateItem& item) {
	for (int j = 0; j < N; ++j) {
		if (item.buffer.test(j)) {
			word_t word = item.ref->forms[j];
			postag_t tag = item.ref->postags[j];
			get_possible_shift_actions(item, j, word, tag);
		}
	}
}

void GraphDecoder::shift_nodes(int top0, const graph::edgeset_t& descendants,
		const StateItem& item) {
	for (const int d : descendants) {
		if (item.buffer.test(d) && get_distance(item.graph, top0, d)!=-1) {
			word_t word = item.ref->forms[d];
			postag_t tag = item.ref->postags[d];
			get_possible_shift_actions(item, d, word, tag);
		}
	}
}




void GraphDecoder::make_split_arc(const graph_t* graph, int top0,
		const StateItem& item) {
	//if there are left child nodes then return
	if(item.nr_left_children[top0] > 0){
		return;
	}
	const graph::edgeset_t& children = graph->children_.at(top0);
	for (auto child : children) {
		if (item.buffer.test(child)) {
			if(gold_previous_words.count(item.words_shifted_map.at(top0)) &&
					(item.constraint->is_verb(item.ref, child, item.ref->form(child)) || subtree_root_node_that.count(item.ref->form(child)))){
				possible_actions.push_back(
							ActionFactory::make_split_arc(
									ZGen::ShiftReduce::Action::SPLIT_ARC_TAG::kParentShiftedChildNotShifted, word_that));
				break;
			}
		}
	}
	for (auto child : children) {
		if (item.buffer.test(child)) {
			if(item.constraint->is_verb_present_tense(item.ref, child, item.ref->form(child))){
				possible_actions.push_back(
							ActionFactory::make_split_arc(
									ZGen::ShiftReduce::Action::SPLIT_ARC_TAG::kParentShiftedChildNotShifted, word_to));
				break;
			}
		}
	}
//	if(learn_mode){
		//shift Split arc if parent in shallow tree of child is a function word
		/*const graph::edgeset_t& children = graph->children_.at(top0);
		for (auto child : children) {
			if (item.buffer.test(child)) {
				int parent = tree.head(
						item.gold_ref->input_gold_node_mapping.at(child));
				if (parent >= 0) {
					std::vector<int>::const_iterator it;
					int gold_function_word = item.gold_ref->form(parent);
					it = std::find(item.gold_ref->function_words.begin(),
							item.gold_ref->function_words.end(), parent);
					if (it != item.gold_ref->function_words.end() && gold_function_word == word_that) {
						possible_actions.push_back(
								ActionFactory::make_split_arc(
										ZGen::ShiftReduce::Action::SPLIT_ARC_TAG::kParentShiftedChildNotShifted,
										gold_function_word));
						_INFO<<"is verb "<<item.constraint->is_verb(item.ref, child, item.ref->form(child))<< " "<<item.form(child);
					}
				}
			}
		}*/
//	}else{
//		if(item.constraint->get_most_frequent_tag(item.ref, top0, item.ref->form(top0)) == item.constraint->pos_VB){
		/*if(item.constraint->is_verb(item.ref, top0, item.ref->form(top0))){
			const graph::edgeset_t& children = graph->children_.at(top0);
			for (auto child : children) {
//				if (item.buffer.test(child) && item.constraint->get_most_frequent_tag(item.ref, child, item.ref->form(child)) == item.constraint->pos_VB) {
				if (item.buffer.test(child) && item.constraint->is_verb_present_tense(item.ref, child, item.ref->form(child))){
					possible_actions.push_back(
										ActionFactory::make_split_arc(
												ZGen::ShiftReduce::Action::SPLIT_ARC_TAG::kParentShiftedChildNotShifted,
												word_to));
					break;
				}
			}
		}*/
//	}
}

std::set<int> GraphDecoder::get_direct_children(const graph_t* graph, const StateItem& item, int input_node) {
	std::set<int> direct_children;
	if (graph->has_children(input_node)) {
		std::unordered_set<int> children = graph->children_.at(input_node);
		for (auto child : children) {
			std::unordered_set<int> parents = graph->parent_.at(child);
			if (parents.size() == 1) {
				direct_children.insert(child);
			} else {
				for (auto parent : parents) {
					if(item.nr_left_children[parent] > 0 || parent == input_node){
						continue;
					}
						if (item.buffer.test(parent)) {
							//non direct child
							goto outside_loop;
						}
						if (item.shifted.test(parent)
								&& is_parent_ancestor_of_next_element(parent, item,
										graph)) {
							//non direct child
							goto outside_loop;
						}
				}
				direct_children.insert(child);
			}
			outside_loop: ;
		}
	}
	return direct_children;
}

bool GraphDecoder::is_any_direct_children_not_shifted(
		const std::set<int>& direct_children, const StateItem& item) {
	bool any_direct_children_not_shifted = false;
	if (direct_children.size() > 0) {
		for (auto direct_child : direct_children) {
			if (item.buffer.test(direct_child)) {
				any_direct_children_not_shifted = true;
				break;
			}
		}
	}
	return any_direct_children_not_shifted;
}

std::set<int> GraphDecoder::get_inflection_nodes(const graph_t* graph, const StateItem& item) {
	int top1 = item.top1;
	int top0 = item.top0;
	std::vector<std::set<int> > ancestors;
	std::set<int> inflection_points = graph->get_inflection_points(top1, top0);
	std::vector<std::set<std::pair<int, int> > > inflection_hops;
	for (int loop_index = item.stack.size() - 2; loop_index >= 2;
			loop_index--) {
		int stack_top_n = item.stack.at(loop_index);
		int stack_top_n_plus_1 = item.stack.at(loop_index - 1);
		std::set<int> ancestor;
		std::set<int> upd_inflection_points;
		std::set<std::pair<int, int> > inflection_hop;
//		_INFO<<"stack_top_n_plus_1 "<<item.form(stack_top_n_plus_1);
		for (auto inflection_point : inflection_points) {
			//if inflection point is shifted do not continue
			if(!item.buffer.test(inflection_point)){
				continue;
			}
			if (graph->is_descendant(stack_top_n_plus_1, inflection_point)) {
				ancestor.insert(inflection_point);
			}
//			_INFO<<"inflection_point "<<item.form(inflection_point);
			std::set<int> direct_children = get_direct_children(graph, item, stack_top_n_plus_1);
			if (graph->is_sibling(stack_top_n_plus_1, inflection_point) && !is_any_direct_children_not_shifted(direct_children, item)) {
				std::set<int> tmp_inflection_points =
						graph->get_inflection_points(stack_top_n_plus_1,
								inflection_point);
				upd_inflection_points.insert(tmp_inflection_points.begin(),
						tmp_inflection_points.end());
				for (auto tmp_inflection_point : tmp_inflection_points) {
					inflection_hop.insert(
							std::make_pair(inflection_point,
									tmp_inflection_point));
//					_INFO<<"tmp_inflection_point "<<item.form(tmp_inflection_point);
				}
			}
			if (graph->is_parent(inflection_point, stack_top_n_plus_1) && !is_any_direct_children_not_shifted(direct_children, item)) {
				inflection_hop.insert(
						std::make_pair(inflection_point, inflection_point));
				upd_inflection_points.insert(inflection_point);
			}
		}
		inflection_hops.push_back(inflection_hop);
		inflection_points = upd_inflection_points;
		ancestors.push_back(ancestor);
	}
	std::set<int> inflections;
	if(inflection_hops.size()>0){
		std::set<std::pair<int, int> > inflection_hop = inflection_hops.at(
				inflection_hops.size() - 1);
		for (auto inflection_hop_entry : inflection_hop) {
			inflections.insert(inflection_hop_entry.first);
		}
		std::set<int> ancestor = ancestors.at(inflection_hops.size() - 1);
		if (ancestor.size() > 0) {
			inflections.insert(ancestor.begin(), ancestor.end());
		}
		for (int loop_index = inflection_hops.size() - 2; loop_index >= 0;
				loop_index--) {
			std::set<std::pair<int, int> > inflection_hop = inflection_hops.at(
					loop_index);
			std::set<int> upd_inflections;
			for (auto inflection_hop_entry : inflection_hop) {
//				_INFO<<"inflection_hop "<<item.form(inflection_hop_entry.first)<<" "<<item.form(inflection_hop_entry.second);
				if (inflections.count(inflection_hop_entry.second)) {
					upd_inflections.insert(inflection_hop_entry.first);
				}
			}
			std::set<int> ancestor = ancestors.at(loop_index);
			if (ancestor.size() > 0) {
				upd_inflections.insert(ancestor.begin(), ancestor.end());
			}
			/*for(auto ancestor_entry: ancestor){
				_INFO<<"ancestor entry "<<item.form(ancestor_entry);
			}*/
			inflections = upd_inflections;
		}
	}else if(item.stack.size() == 3){
		inflections = inflection_points;
	}
	return inflections;
}

void GraphDecoder::shift_parent_siblings_in_sibling_relationship(
		const graph_t* graph, int top0, const StateItem& item,
		const std::set<int>& inflections, graph::edgeset_t& shift_candidates,
		std::set<int>& valid_parents) {
	/*for(int index=item.stack.size()-1; index>0; index --){
	 _INFO<<"inflection stack element "<<item.forms_alphabet->decode(item.ref->form(item.stack.at(index)));
	 }
	 for(auto valid_inflection: inflections){
	 _INFO<<"valid inflection "<<valid_inflection;
	 }*/
	const graph::edgeset_t& parents = graph->parent_.at(top0);
	for (auto parent : parents) {
		if (!item.buffer.test(parent)) {
			continue;
		}
		for (auto valid_inflection : inflections) {
			if (graph->is_descendant(valid_inflection, parent)
					|| valid_inflection == parent) {
				shift_parent_and_siblings(shift_candidates, valid_parents,
						parent, graph, item);
				break;
			}
		}
	}
}

void GraphDecoder::shift_parents_siblings_single_stack_item(
		const graph::edgeset_t& parents, const StateItem& item,
		const graph_t* graph, graph::edgeset_t& shift_candidates,
		std::set<int>& valid_parents) {
	for (auto parent : parents) {
		if (!item.buffer.test(parent)) {
			continue;
		}
		shift_candidates.insert(parent);
		ZGen::graph::edgeset_t siblings = graph->bypass_shifted_nodes(parent,
				item.buffer);
		shift_candidates.insert(siblings.begin(), siblings.end());
		valid_parents.insert(parent);
	}
}

void GraphDecoder::shift_parents_siblings_descendant_relationship(
		const StateItem& item,
		const graph_t* graph, int top1, graph::edgeset_t& shift_candidates,
		std::set<int>& valid_parents) {
	std::unordered_set<int> parents = graph->parent_.at(item.top0);
	for (auto parent : parents) {
		if (item.buffer.test(parent) && graph->is_descendant(top1, parent)) {
			shift_candidates.insert(parent);
			ZGen::graph::edgeset_t siblings = graph->bypass_shifted_nodes(
					parent, item.buffer);
			shift_candidates.insert(siblings.begin(), siblings.end());
			valid_parents.insert(parent);
		}
	}
}

void GraphDecoder::insert_comma(const std::set<int>& valid_parents, bool shift_descendants,
		const StateItem& item) {
	//shift comma through parents and siblings
	bool comma_shifted = false;
	int num_commas = 3;
	if (valid_parents.size() > 0) {
		for (auto valid_parent : valid_parents) {
			if ((!item.child_comma.count(valid_parent)
							|| item.child_comma.at(valid_parent) < num_commas)
					&& item.last_action.name() != Action::kInsert) {
				possible_actions.push_back(
						ActionFactory::make_insert(valid_parent));
				comma_shifted = true;
				break;
			}
		}
	}
	//shift comma in descendant
	if(shift_descendants && !comma_shifted){
		if((!item.child_comma.count(item.top0) || item.child_comma.at(item.top0) < num_commas)
				&& item.last_action.name()!=Action::kLeftArc && item.last_action.name()!=Action::kInsert){
			possible_actions.push_back(ActionFactory::make_insert());
		}
	}
}

void GraphDecoder::split_arc_descendants(const StateItem& item,
		graph::edgeset_t shift_candidates, const graph_t* graph, int top0) {
	//if last action was split arc only shift the descendants of stack_top0 through child nodes which have parent as function word in shallow tree
	if (ActionUtils::is_split_arc(item.last_action)) {
		possible_actions.clear();
		shift_candidates.clear();
		const graph::edgeset_t& children = graph->children_.at(top0);
//		if(learn_mode){
			/*for (auto child : children) {
				if (item.buffer.test(child)) {
					int parent = tree.head(
							item.gold_ref->input_gold_node_mapping.at(child));
					if (parent >= 0) {
						std::vector<int>::const_iterator it;
						int gold_function_word = item.gold_ref->form(parent);
						it = std::find(item.gold_ref->function_words.begin(),
								item.gold_ref->function_words.end(), parent);
						if (it != item.gold_ref->function_words.end()
								&& item.last_action.word == gold_function_word) {
							ZGen::graph::edgeset_t desc =
									graph->bypass_shifted_nodes(child, item.buffer);
							shift_candidates.insert(child);
							if(item.last_action.word==word_that){
								shift_candidates.insert(desc.begin(), desc.end());
							}
						}
					}
				}
			}*/
//		}else{
		/*	if(item.last_action.word == word_to){
				for (auto child : children) {
					if(item.constraint->get_most_frequent_tag(item.ref, child, item.ref->form(child)) == item.constraint->pos_VB){
						shift_candidates.insert(child);
					}
				}
			}
		}*/
			if(item.last_action.word == word_that){
				for (auto child : children) {
					if(item.constraint->is_verb(item.ref, child, item.ref->form(child))
							|| subtree_root_node_that.count(item.ref->form(child))){
						shift_candidates.insert(child);
						ZGen::graph::edgeset_t desc =
								graph->bypass_shifted_nodes(child, item.buffer);
						shift_candidates.insert(desc.begin(), desc.end());
					}
				}
			}else if(item.last_action.word == word_to){
				for (auto child : children) {
					if(item.constraint->is_verb_present_tense(item.ref, child, item.ref->form(child))){
						shift_candidates.insert(child);
					}
				}
			}
		shift_nodes(top0, shift_candidates, item);
	}
}

void
GraphDecoder::get_possible_actions(const StateItem& item) {
	  int N = item.ref->size();
	  possible_actions.clear();
//	if(learn_mode && item.top0>-1 && item.last_action.name() != Action::kIdle){
	/*if(item.last_action.name() != Action::kIdle && item.last_action.name() != Action::kInsert){
		possible_actions.push_back(ActionFactory::make_insert());
	}*/
	graph::edgeset_t shift_candidates;
	if (item.stack.size() == 1) {
	  all_shift(N, item);
	} else {
		  bool all_graph_descendants_shifted = true;
		  int top0 = item.top0;
		  int top0_index = item.stack.size()-1;
		  const graph_t * graph = item.graph;
		  /**
		   * if direct child is not shifted
		   * then
		   * 	shift graph descendants
		   * else
		   * 	shift graph descendants
		   * 	reduce
		   * 	shift parent siblings
		   */

		  //get direct child nodes

		  std::set<int> direct_children = get_direct_children(graph, item, top0);
		  ZGen::graph::edgeset_t descendants_through_non_shifted_nodes =
		  					graph->bypass_shifted_nodes(top0, item.buffer);
		  bool any_direct_children_not_shifted = is_any_direct_children_not_shifted(direct_children, item);
		  bool shift_descendants = false;
		  std::set<int> valid_parents;
		  if(any_direct_children_not_shifted){
				shift_nodes(top0, descendants_through_non_shifted_nodes, item);
				shift_descendants = true;
		  }else{
			  graph::edgeset_t shift_candidates;
			  if(!item.nr_left_children[top0] > 0){
				  shift_candidates.insert(descendants_through_non_shifted_nodes.begin(), descendants_through_non_shifted_nodes.end());
				  shift_descendants = true;
			  }
			  Action::ACTION_TYPE reduced = reduce(item, graph);
//			  if(!reduced){
			  if(reduced == Action::kNone){
				  //shift parent and siblings
				  /**
				   * top1 top0 are in either or both of the following possibilities
				   * top1 is ancestor to top0
				   * top0 is sibling/common parent to top1
				   */
				  //top1 is ancestor to top0
				  int top1 = item.top1;
				  if (top1 >= 0){
					  if(graph->is_descendant(top1, top0)){
						  shift_parents_siblings_descendant_relationship(item, graph, top1, shift_candidates, valid_parents);
					  }
					  direct_children = get_direct_children(graph, item, top1);
					  if(graph->is_sibling(top1, top0)  && !is_any_direct_children_not_shifted(direct_children, item)){
						std::set<int> inflections = get_inflection_nodes(graph, item);
						/*for(int index=item.stack.size()-1; index>0; index --){
						  _INFO<<"inflection stack element "<<item.forms_alphabet->decode(item.ref->form(item.stack.at(index)));
						}
						  for(auto valid_inflection: inflections){
							  _INFO<<"valid inflection "<<item.form(valid_inflection);
						  }*/
						shift_parent_siblings_in_sibling_relationship(graph, top0, item, inflections, shift_candidates, valid_parents);
					  }
				  }else{
					  const graph::edgeset_t& parents = graph->parent_.at(top0);
					  shift_parents_siblings_single_stack_item(parents, item, graph, shift_candidates, valid_parents);
				  }
			  }else if(reduced == Action::kLeftArc){
				  int top1 = item.top1;
				  if (top1 >= 0){
					  if(graph->is_sibling(top1, top0)){
						  std::set<int> inflections = get_inflection_nodes(graph, item);
						  shift_parent_siblings_in_sibling_relationship(graph, top0, item, inflections, shift_candidates, valid_parents);
					  }
				  }
			  }else if(reduced == Action::kRightArc){
				  int top1 = item.top1;
				  if(graph->is_descendant(top1, top0)){
					shift_parents_siblings_descendant_relationship(item, graph, top1, shift_candidates, valid_parents);
				  }
				  direct_children = get_direct_children(graph, item, top1);
				  if(graph->is_sibling(top1, top0) && !is_any_direct_children_not_shifted(direct_children, item)){	//all direct children of top0 should be shifted
					std::set<int> inflections = get_inflection_nodes(graph, item);
					shift_parent_siblings_in_sibling_relationship(graph, top0, item, inflections, shift_candidates, valid_parents);
				  }
			  }
			  shift_nodes(top0, shift_candidates, item);
		  }
		  //if last action was split arc only shift the descendants of stack_top0 through child nodes which have parent as function word in shallow tree
		  if(shift_descendants){
			  make_split_arc(graph, top0, item);
		  }
		  split_arc_descendants(item, shift_candidates, graph, top0);
		  insert_comma(valid_parents, shift_descendants, item);
	}
	if(item.words_shifted.size() == item.ref->size() && item.stack.size()==2){
		possible_actions.push_back(ActionFactory::make_idle());
	}
	if(possible_actions.size()==0){
		  /*_INFO<<"top0 "<<item.forms_alphabet->decode(item.ref->form(item.top0));
		  _INFO<<"item "<<item;
		  for(int index=item.stack.size()-1; index>0; index --){
			  _INFO<<"stack element "<<item.forms_alphabet->decode(item.ref->form(item.stack.at(index)));
		  }*/
		  if(coverage_vector.find(sentence_no) == coverage_vector.end()){
			  coverage_vector.insert(sentence_no);
			  _INFO<<"sentence_no "<<sentence_no;
			  const_sentence_no = sentence_no;
		  }
			  all_shift_reduce(N, item);
	  }
}

void GraphDecoder::shift_parent_and_siblings(graph::edgeset_t& shift_candidates, std::set<int>& valid_parents, int parent, const graph_t* graph, const StateItem& item){
	shift_candidates.insert(parent);
	ZGen::graph::edgeset_t siblings = graph->bypass_shifted_nodes(parent, item.buffer);
	shift_candidates.insert(siblings.begin(), siblings.end());
	valid_parents.insert(parent);
}
} //  end for namespace ShiftReduce 
} //  end for namespace ZGen
