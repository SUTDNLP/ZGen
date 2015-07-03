#include <iostream>
#include "shiftreduce/types/action_utils.h"
#include <boost/log/trivial.hpp>

namespace ZGen {
namespace ShiftReduce {

bool ActionUtils::is_shift(const action_t& act)     { return (act.name() == Action::kShift); }
bool ActionUtils::is_left_arc(const action_t& act)  { return (act.name() == Action::kLeftArc); }
bool ActionUtils::is_right_arc(const action_t& act) { return (act.name() == Action::kRightArc); }

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
  return (int)actions.size();
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

  actions.push_back( action_t(Action::kShift, parse.postags[root],
        parse.forms[root], root) );

  for (int j = i; j < children.size(); ++ j) {
    int child = children[j];
    get_correct_actions_travel(child, labeled, parse, tree, actions);
    if (labeled) {
      actions.push_back( ActionFactory::make_right_arc(parse.deprels[child]) );
    } else {
      actions.push_back( ActionFactory::make_right_arc() );
    }
  }

  for (int j = i - 1; j >= 0; -- j) {
    int child = children[j];
    if (labeled) {
      actions.push_back( ActionFactory::make_left_arc(parse.deprels[child]) );
    } else {
      actions.push_back( ActionFactory::make_left_arc() );
    }
  }
}

} //  end for namespace ShiftReduce
} //  end for namespace ZGen
