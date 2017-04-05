#include <iostream>
#include "shiftreduce/types/action_utils.h"
#include <boost/log/trivial.hpp>
#include "utils/logging.h"
//#include <algorithm>

namespace ZGen {
namespace ShiftReduce {

extern int word_to;
extern int word_that;
extern int word_comma;
extern int sentence_no;
extern int learn_mode;
bool ActionUtils::is_shift(const action_t& act)     { return (act.name() == Action::kShift); }
bool ActionUtils::is_left_arc(const action_t& act)  { return (act.name() == Action::kLeftArc); }
bool ActionUtils::is_right_arc(const action_t& act) { return (act.name() == Action::kRightArc); }
bool ActionUtils::is_insert(const action_t& act) { return (act.name() == Action::kInsert); }
bool ActionUtils::is_split_arc(const action_t& act) { return (act.name() == Action::kSplitArc); }
bool ActionUtils::is_idle(const action_t& act) { return (act.name() == Action::kIdle); }

int
ActionUtils::get_correct_actions(const dependency_t& oracle,
    bool labeled,
    std::vector<action_t>& actions) {
  // Extract ordered-tree structure from the oracle parse.
  int N = oracle.forms.size();
  int root = -1;
  std::vector<std::vector<int> > tree(N);

  actions.clear();

  for (int i = 0; i < N; ++ i) {
    int head = oracle.heads[i];
    if (head == -1) { root = i; }
    else { tree[head].push_back(i); }
  }

  get_correct_actions_travel(root, labeled, oracle, tree, actions);
  if(!learn_mode){
	  for(auto fn_word: oracle.function_words){
		  remove_from_actions(fn_word, actions);
	  }
  }
  for(Action & action:actions){
	  if(action.name()==Action::kShift){
		  action.index = oracle.gold_input_node_mapping.at(action.index);
	  }
  }
	int input_size = oracle.input_gold_node_mapping.size();
	int max_index = 4*input_size-2-actions.size();
	for(int index = 0; index < max_index; index++){
	  actions.push_back(ActionFactory::make_idle());
	}
  return (int)actions.size();
}

bool ActionUtils::is_comma(const dependency_t& parse, int root) {
	bool is_comma = false;
	for (auto elem : parse.function_words) {
		if (elem == root && word_comma == parse.form(root)) {
			is_comma = true;
		}
	}
	return is_comma;
}

bool ActionUtils::is_that_to(const dependency_t& parse, int root) {
	bool is_that_to = false;
	for (auto elem : parse.function_words) {
		if (elem == root && (word_that == parse.form(root) || word_to == parse.form(root))) {
			is_that_to = true;
		}
	}
	return is_that_to;
}

bool ActionUtils::is_function_word(const dependency_t& parse, int child) {
	bool is_function_word = false;
	for (auto elem : parse.function_words) {
		if (elem == child) {
			is_function_word = true;
		}
	}
	return is_function_word;
}

/**
 * Method to remove the shift action of to/that
 */
void ActionUtils::remove_from_actions(int child,
		std::vector<action_t>& actions) {
	for (std::vector<action_t>::iterator it = actions.begin();
			it != actions.end();) {
		if ((*it).index == child) {
			it = actions.erase(it); // Returns the new iterator to continue from.
		} else {
			++it;
		}
	}
}


void
ActionUtils::get_correct_actions_travel(int root,
    bool labeled,
    const dependency_t & parse,
    const std::vector<std::vector<int> > & tree,
    std::vector<action_t>& actions) {
  const std::vector<int> & children = tree[root];

  int i;
  for (i = 0; i < children.size() && children[i] < root; ++ i) {
    get_correct_actions_travel(children[i], labeled, parse, tree, actions);
  }

  if(is_comma(parse, root)){
	  actions.push_back(ActionFactory::make_insert());
  }else if(is_that_to(parse, root)){
	  actions.push_back(ActionFactory::make_split_arc(ZGen::ShiftReduce::Action::SPLIT_ARC_TAG::kParentShiftedChildNotShifted, parse.form(root)));
  }else{
	  actions.push_back( action_t(Action::kShift, parse.postags[root],
			  parse.forms[root], root) );
  }

  for (int j = i; j < children.size(); ++ j) {
    int child = children[j];
    get_correct_actions_travel(child, labeled, parse, tree, actions);
	//if root is function word ignore right arc
	if(!is_function_word(parse, root)){
		if(is_comma(parse,child)){
//			actions.push_back(ActionFactory::make_insert());
		}else if (labeled) {
		  actions.push_back( ActionFactory::make_right_arc(parse.deprels[child]) );
		} else {
		  actions.push_back( ActionFactory::make_right_arc() );
		  /*if(is_function_word(parse, child)){
			  ZGen::ShiftReduce::Action::SPLIT_ARC_TAG split_arc_tag = make_split_arc(root, child, actions, tree);
			  actions.push_back(ActionFactory::make_split_arc(split_arc_tag, parse.form(child)));
		  }*/
		}
	}
  }

  for (int j = i - 1; j >= 0; -- j) {
    int child = children[j];
	if(!is_function_word(parse, root)){
		if(is_comma(parse,child)){
			//actions.push_back(ActionFactory::make_insert());
//			_INFO<<"comma left arc sent number "<<sentence_no;
		}else if (labeled) {
			actions.push_back( ActionFactory::make_left_arc(parse.deprels[child]) );
		} else {
			actions.push_back( ActionFactory::make_left_arc() );
			/*if(is_function_word(parse, child)){
				ZGen::ShiftReduce::Action::SPLIT_ARC_TAG split_arc_tag = make_split_arc(root, child, actions, tree);
				actions.push_back(ActionFactory::make_split_arc(split_arc_tag, parse.form(child)));
			}*/
		}
    }
  }
}

} //  end for namespace ShiftReduce
} //  end for namespace ZGen
