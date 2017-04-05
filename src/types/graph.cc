/*
 * graph.cpp
 *
 *  Created on: 06-Jul-2015
 *      Author: ratish
 */

#include "graph.h"
#include <algorithm>
#include <string>
#include <queue>
#include "utils/logging.h"
#include <boost/algorithm/string/predicate.hpp>

namespace ZGen {
//namespace eg = ZGen::Engine;
graph::graph() {
	// TODO Auto-generated constructor stub

}

graph::~graph() {
	// TODO Auto-generated destructor stub
}

int graph::add_to_parent_map(int node, int parent, int label){
	parent_map[node].push_back(std::make_pair(parent,label));
	children_map[parent].push_back(std::make_pair(node,label));
	return parent_map.size();
}

bool sort_pair (std::pair<int , int> i,std::pair<int , int> j) { return (i.second<j.second); }	//sort by label

void graph::add_ancestors(){
//<<<<<<< HEAD
	for(auto elem: parent_map){
		int key = elem.first;
		std::vector<std::pair<int,int>> parent = elem.second;
		std::sort(parent.begin(), parent.end(), sort_pair);
		parent_map[key] = parent;
	}
//=======
//	int num_nodes = parent_map.size();
	/*for(int index=0;index<num_nodes;index++){
		int node = index;
		while(parent_map.count(node)>0){
			node = parent_map.at(node).first;
			if(node == -1){
				continue;
			}
			ancestors[index].push_back(node);
		}
		std::reverse(ancestors[index].begin(),ancestors[index].end());
	}*/
//>>>>>>> morph_a_ref

	for(auto elem: children_map){
		int key = elem.first;
		std::vector<std::pair<int,int>> children = elem.second;
		std::sort(children.begin(),children.end(),sort_pair);
		children_map[key] = children;
	}

	/**
	 * iterate through children map
	 * if number of children ==1 , then sibling entry does not exist
	 * if number of children > 1, then make entry in sibling map
	 * if root node, then no entry in sibling map
	 */
	for(auto elem: children_map){
//		int key = elem.first;
		std::vector<std::pair<int,int>> children = elem.second;
		if(children.size()==1){
			continue;
		}
		for (int counter1 = 0; counter1 < children.size(); ++counter1) {
			int key = children.at(counter1).first;
			for (int counter2 = 0; counter2 < children.size(); ++counter2) {
				if(counter2 == counter1){
					continue;
				}
				sibling_map[key].push_back(children.at(counter2));
			}
		}
	}

	for(auto elem: sibling_map){
		int key = elem.first;
		std::vector<std::pair<int,int>> siblings = elem.second;
		std::sort(siblings.begin(), siblings.end(), sort_pair);
		sibling_map[key] = siblings;
	}
}
graph::edgeset_t graph::get_non_direct_descendants(int top0) {
	graph::edgeset_t non_direct_descendants;
	if(!descendants_.count(top0)){
		return non_direct_descendants;
	}
	const graph::edgeset_t& descendants = descendants_.at(top0);
	std::unordered_map<int,bool> queue_map;
	std::queue<int> queue_nodes;
	queue_nodes.push(top0);
	queue_map[top0] = true;
	//compute non_direct_descendants
	while (!queue_nodes.empty()) {
		const graph::edgeset_t& children = children_.at(
				queue_nodes.front());
		queue_nodes.pop();
		for (const int c : children) {
			if (queue_map.count(c)) {
				continue;
			} else {
				queue_map[c] = true;
			}
			bool is_non_direct_descendant = false;
			const graph::edgeset_t& parents = parent_.at(c);
			if (parents.size() > 1) {
				for (const int p : parents) {
					if(p!=top0 && descendants.find(p) == descendants.end()){
						non_direct_descendants.insert(c);
						is_non_direct_descendant = true;
						break;
					}
				}
				if (!is_non_direct_descendant) {
					queue_nodes.push(c);
				}
			} else {
				queue_nodes.push(c);
			}
		}
	}
	return non_direct_descendants;
}

graph::edgeset_t graph::get_direct_descendants(int word_index){
	const graph::edgeset_t& descendants = descendants_.at(word_index);
	graph::edgeset_t direct_descendants;
	graph::edgeset_t non_direct_descendants = get_non_direct_descendants(word_index);
	std::unordered_map<int, bool> queue_map;
	std::deque<int> dequeue_nodes;
	dequeue_nodes.push_back(word_index);
	while (!dequeue_nodes.empty()) {
		const graph::edgeset_t& children = children_.at(
				dequeue_nodes.front());
		direct_descendants.insert(dequeue_nodes.front());
		dequeue_nodes.pop_front();
		for (const int c : children) {
			if (queue_map.count(c)) {
				continue;
			} else {
				queue_map[c] = true;
			}
			dequeue_nodes.push_back(c);

			if (non_direct_descendants.size() > 0) {
				for (auto element : non_direct_descendants) {
					if (descendants_.at(element).find(c)
							!= descendants_.at(element).end()
							|| element == c) {
						//node c is a descendant of non-direct descendant
						dequeue_nodes.pop_back(); //pop out the last added element
						break;
					}
				}
			}
		}
	}
	//returns direct_descendants including the original input node
	return direct_descendants;
}


void graph::compute_path(Engine::TokenAlphabet& deprels_alphabet_graph, Engine::TokenAlphabet& forms_alphabet){
	std::vector<int> keys;
	for(auto kv: parent_map){
		keys.push_back(kv.first);
	}
	int graph_labels_size = deprels_alphabet_graph.size();
	int prime = 7;
	for(int input: keys){
		std::unordered_map<int,bool> queue_map;
		std::queue<int> queue_nodes;
		queue_nodes.push(input);
		queue_map[input] = true;
		label_path_map[std::make_pair(input, input)] = 0;
		while(!queue_nodes.empty()){
			int input_node = queue_nodes.front();
			queue_nodes.pop();
			if(parent_map.count(input_node)){
				std::vector<std::pair<int,int>> parent_nodes = parent_map.at(input_node);
				for(auto parent:parent_nodes){
					if(queue_map.count(parent.first)==0){
						queue_map[parent.first] = true;
						queue_nodes.push(parent.first);
						label_path_map[std::make_pair(input, parent.first)] = prime* label_path_map.at(std::make_pair(input, input_node)) + parent.second;
					}
				}
			}
			if(children_map.count(input_node)){
				std::vector<std::pair<int,int>> child_nodes = children_map.at(input_node);
				for(auto child: child_nodes){
					if(queue_map.count(child.first)==0){
						queue_map[child.first] = true;
						queue_nodes.push(child.first);
						label_path_map[std::make_pair(input, child.first)] = prime* label_path_map.at(std::make_pair(input, input_node)) + (graph_labels_size +child.second);
						//adding delta of graph_labels_size  to compute label_path_map for child node
					}
				}
			}
		}
	}
	/*for(auto item: label_path_map){
		std::string node_1 = "ROOT";
		std::string node_2 = "ROOT";
		if(item.first.first != -1){
			node_1 = forms_alphabet.decode(instance->form(item.first.first));
		}
		if(item.first.second != -1){
			node_2 = forms_alphabet.decode(instance->form(item.first.second));
		}
		_INFO << "node 1 "<< node_1 << " node 2 "<< node_2;
		_INFO <<"value "<<item.second;
		int value = item.second;
		std::vector<int> path_vector;
		while(value > 0){
			int mod =value%31;
			path_vector.push_back(mod);
			value = value/31;
			_INFO<<"mod "<<mod <<" value "<<value;
		}
		std::reverse(path_vector.begin(), path_vector.end());
		for(auto label: path_vector){
			bool child = false;
			if(label > graph_labels_size){
				label = label-graph_labels_size;
				child = true;
			}
			std::string child_out = child?" child":"";
			_INFO <<"label: "<<deprels_alphabet_graph.decode(label) << child_out ;
		}
	}*/
}

void graph::bfs(){
//	int input = 0;
//	int num_nodes = parent_map.size();
//	for(int index=0;index<num_nodes;index++){
	std::vector<int> keys;
	for(auto kv: parent_map){
		keys.push_back(kv.first);
	}
//	for(std::unordered_map<int,std::pair<int,int>>::iterator iter = parent_map.begin(); iter != parent_map.end(); ++iter){
	for(int input: keys){
//		input = iter->first;
		std::unordered_map<int,bool> queue_map;
		std::queue<int> queue_nodes;
		queue_nodes.push(input);
		queue_map[input] = true;
		path_map[std::make_pair(input,input)] = boost::make_tuple(0,Direction::kAncestor,0);
		while(!queue_nodes.empty()){
			int input_node = queue_nodes.front();
			queue_nodes.pop();
			boost::tuples::tuple<int,Direction,int> path_map_entry = path_map.at(std::make_pair(input,input_node));
			int distance = path_map_entry.get<0>()+1;

			if(parent_map.count(input_node)){
				std::vector<std::pair<int,int>> parent_nodes = parent_map.at(input_node);
				for(auto parent:parent_nodes){
//					std::pair<int,int> parent = parent_map.at(input_node);
					if(queue_map.count(parent.first)==0){
						queue_map[parent.first] = true;
						queue_nodes.push(parent.first);
						Direction mode = path_map_entry.get<1>();
						if(distance == 1){
							mode = Direction::kAncestor;
						}else if(mode == Direction::kDescendant){
							mode = Direction::kSibling;
						}
						path_map[std::make_pair(input,parent.first)] = boost::tuples::make_tuple(distance,mode,parent.second);
					}/*else{
						boost::tuples::tuple<int,Direction,int> path = path_map.at(std::make_pair(input,parent.first));
						int path_distance = path.get<0>();
						int path_label = path.get<2>();
						if(path_distance == distance){
							Direction mode = path_map_entry.get<1>();
							if(mapped_edgelabel(deprels_alphabet->decode(path_label))> mapped_edgelabel(parent.second)){
//							if(mode == Direction::kAncestor && path.get<1>()==Direction::kSibling){//update path map if new mode is ancestor and current mode is sibling
								path_map[std::make_pair(input,parent.first)]  = boost::tuples::make_tuple(distance,mode,parent.second);
							}
						}
					}*/
				}
			}
			if(children_map.count(input_node)){
				std::vector<std::pair<int,int>> child_nodes = children_map.at(input_node);
				for(auto child: child_nodes){
					if(queue_map.count(child.first)==0){
						queue_map[child.first] = true;
						queue_nodes.push(child.first);
						Direction mode = path_map_entry.get<1>();
						if(distance ==1){
							mode = Direction::kDescendant;
						}else if(mode == Direction::kAncestor){
							mode = Direction::kSibling;
						}
						path_map[std::make_pair(input,child.first)] = boost::tuples::make_tuple(distance,mode,child.second);
					}/*else{
						boost::tuples::tuple<int,Direction,int> path = path_map.at(std::make_pair(input,child.first));
						int path_distance = path.get<0>();
						int path_label = path.get<2>();
						if(path_distance == distance){
							Direction mode = path_map_entry.get<1>();
							if(mode == Direction::kDescendant && path.get<1>()==Direction::kSibling){	//update path map if new mode is descendant and current mode is sibling
								path_map[std::make_pair(input,child.first)]  = boost::tuples::make_tuple(distance,mode,child.second);
							}
						}
					}*/
				}
			}
		}
	}
	/*for(auto item: path_map){
//		_TRACE << "node 1 "<<item.first.first << " "<<" node 2 "<<item.first.second;
//		boost::tuples::tuple<int,int,int> value = item.second;
//		_TRACE << " distance "<<value.get<0>() << " mode "<< value.get<1>()  << " label "<< value.get<2>()  <<std::endl;
	}*/

}


void
graph::dependency_to_graph() {
  int N = parent_map.size();
  num_nodes = parent_map.size();

  int index = 0;
  std::vector<int> keys;
  for(auto kv: parent_map){
  	keys.push_back(kv.first);
  }
  for(int input: keys){
	nodes[index] = input;
	index++;
  }
  reset(N);

  for (int i = 0; i < N; ++ i) {
	int node_id = nodes.at(i);
    std::vector<std::pair<int,int>> parents = parent_map.at(node_id);
    for(auto parent: parents){
    	if(parent.first >=0){
    		add_edge(parent.first, node_id);
    	}
    }
  }
}

void
graph::set_ref() {
  dependency_to_graph();

  int root = -1;
  int N = parent_map.size();
  for (int i = 0; i < N; ++ i) {
	  int node_id = nodes.at(i);
    if (indgr_[node_id] == 0) {
      root = node_id;
      break;
    }
  }

  go(root);
  for (int i = 0; i < N; ++ i) {
  	  int node_id = nodes.at(i);
//  	  _INFO<<"node is "<<forms_alphabet.decode(input->form(node_id));
  	  edgeset_t direct_descendants = get_direct_descendants(node_id);
  	  /*for(auto elem: siblings_.at(node_id)){
  		  _INFO<<"sibling "<< forms_alphabet.decode(input->form(elem));
  	  }*/
//  	int before = siblings_.at(node_id).size();
//  	_INFO<<"sibling size "<<siblings_.at(node_id).size();
  	  for(auto elem: direct_descendants){
  		if(siblings_.at(node_id).find(elem) != siblings_.at(node_id).end()){
  			siblings_.at(node_id).erase(elem);
  			siblings_map_.at(node_id).reset(elem);
//  			_INFO<<"removing descendant from sibling "<< forms_alphabet.decode(input->form(elem));
  		}
  	  }
  	  /*for(auto elem: siblings_.at(node_id)){
  		  _INFO<<"sibling after "<< forms_alphabet.decode(input->form(elem));
  	  }
  	  _INFO<<"sibling size "<<siblings_.at(node_id).size();*/
  	  /**
  	   * iterate through parents in parent map
  	   *
  	   */
  	  for(auto elem: parent_.at(node_id)){
//  		  siblings_parent_[std::make_pair(node_id,elem)].insert(descendants_.at(elem).begin(), descendants_.at(elem).end());
//  		  edgeset_t siblings = descendants_.at(elem);
//  		  edgeset_t direct_descendants = get_direct_descendants(node_id);
  		  /*edgeset_t result;
  		  std::set_difference(descendants_.at(elem).begin(), descendants_.at(elem).end(), direct_descendants.begin(), direct_descendants.end(),
  				  std::inserter(result, result.end()));
  		  siblings_parent_[std::make_pair(node_id,elem)].insert(result.begin(), result.end());*/
  		  /*edgeset_t result;
  		  for(auto elem2: descendants_.at(elem)){
  			  if(!direct_descendants.count(elem2)){
  				  result.insert(elem2);
  			  }
  		  }
  		  siblings_parent_[std::make_pair(node_id,elem)].insert(result.begin(), result.end());*/

  		  std::unordered_map<int, bool> queue_map;
  		  graph::edgeset_t result;
  		  std::queue<int> queue_nodes;
  		  queue_nodes.push(elem);
  		  queue_map[elem] = true;
  		  while (!queue_nodes.empty()) {
  			  const graph::edgeset_t& children = children_.at(queue_nodes.front());
  			  queue_nodes.pop();
  			  for (const int c : children) {
  				  if (queue_map.count(c)) {
  					  continue;
  				  } else {
  					  queue_map[c] = true;
  				  }
  				  if (c == node_id) {
  					continue;
  				  }
  				  result.insert(c);
  				  queue_nodes.push(c);
  			  }
  		  }
  		  siblings_parent_[std::make_pair(node_id,elem)].insert(result.begin(), result.end());
  	  }
  }
//  return root;
}

graph::edgeset_t graph::get_parent_and_siblings(int input, std::bitset<kMaxNumberOfWords> buffer, int sentence_no) const{
	std::unordered_map<int, bool> queue_map;
	graph::edgeset_t result;
	std::queue<int> queue_nodes;
	/**
	 * if parent is shifted exclude
	 * */
	for(auto parent: parent_.at(input)){
		if(!buffer.test(parent)){
			_INFO<<"graph::get_parent_and_siblings: parent shifted"<<sentence_no;
			continue;
		}
		queue_nodes.push(parent);
		queue_map[parent] = true;
		result.insert(parent);
		while (!queue_nodes.empty()) {
		  const graph::edgeset_t& children = children_.at(queue_nodes.front());
		  queue_nodes.pop();
		  for (const int c : children) {
			  if (queue_map.count(c)) {
				  continue;
			  } else {
				  queue_map[c] = true;
			  }
			  if (!buffer.test(c)) {	//if c is shifted then do not include its descendants
				continue;
			  }
			  result.insert(c);
			  queue_nodes.push(c);
		  }
		}
	}
	return result;
}

graph::edgeset_t graph::bypass_shifted_nodes(int parent, std::bitset<kMaxNumberOfWords> buffer) const{
	std::unordered_map<int, bool> queue_map;
	graph::edgeset_t result;
	std::queue<int> queue_nodes;
	queue_nodes.push(parent);
	queue_map[parent] = true;
	while (!queue_nodes.empty()) {
	  const graph::edgeset_t& children = children_.at(queue_nodes.front());
	  queue_nodes.pop();
	  for (const int c : children) {
		  if (queue_map.count(c)) {
			  continue;
		  } else {
			  queue_map[c] = true;
		  }
		  if (!buffer.test(c)) {	//if c is shifted then do not include its descendants
			continue;
		  }
		  result.insert(c);
		  queue_nodes.push(c);
	  }
	}
	return result;
}

void
graph::reset(int N) {
  // Set all the in degree to zero.
  memset(indgr_, 0, sizeof(indgr_));

  for (int i = 0; i < N; ++ i) {
	int node_id = nodes.at(i);
	parent_[node_id].clear();
    // Clear chidren cache.
    children_[node_id].clear();
    // Clear descendant cache.
    descendants_[node_id].clear();
    // Clear siblings cache.
    siblings_[node_id].clear();
  }
}


bool graph::is_function_word(int index) const{
	  return parent_map.count(index) == 0;
}

bool graph::descendant_exists(int u) const{
	return descendants_map_.count(u) >0;
}

bool graph::is_descendant(int u, int v) const{
	return descendants_map_.count(u) >0 && descendants_map_.at(u).test(v);
}

bool graph::path_map_exists(int u, int v) const{
	return path_map.count(std::make_pair(u, v))>0;
}

boost::tuples::tuple<int,Direction,int> graph::get_path_map_entry(int u, int v) const{
	return path_map.at(std::make_pair(u,v));
}

int graph::get_path_label_entry(int u, int v) const{
	return label_path_map.at(std::make_pair(u, v));
}

int graph::is_sibling(int u, int v) const{
	return siblings_map_.count(u) && siblings_map_.at(u).test(v);
}

/**
 * Inflection points are the parents of node u which are ancestors of node v
 */
std::set<int> graph::get_inflection_points(int u, int v) const{
	std::set<int> inflection_points;
	for(auto elem: parent_.at(u)){
		if(elem != -1 && descendants_map_.at(elem).test(v)){
			inflection_points.insert(elem);
		}
	}
	return inflection_points;
}

int graph::is_parent(int u, int v) const{
	return children_map_.count(u) && children_map_.at(u).test(v);
}

bool graph::has_common_parent(int u, int v) const{
	for(auto elem: parent_.at(u)){
		if(children_map_.at(elem).test(v)){
			return true;
		}
	}
	return false;
}

graph::edgeset_t graph::get_common_parents(int u, int v) const{
	graph::edgeset_t result;
	for(auto elem: parent_.at(u)){
		if(children_map_.at(elem).test(v)){
			result.insert(elem);
		}
	}
	return result;
}

graph::edgeset_t graph::get_common_parents(int u, int v, int w) const{
	graph::edgeset_t result;
	for(auto elem: parent_.at(u)){
		if(children_map_.at(elem).test(v) && children_map_.at(elem).test(w)){
			result.insert(elem);
		}
	}
	return result;
}

bool graph::is_parent_sibling(int u, int v) const{
	bool ret_value = false;
	for(auto elem: parent_.at(v)){
		if(siblings_map_.count(u) && siblings_map_.at(u).test(elem)){
			ret_value = true;
			break;
		}
	}
	return ret_value;
}


bool graph::sibling_parent_entry_exists(int node_id, int parent, int entry) const{
	return siblings_parent_.at(std::make_pair(node_id, parent)).find(entry)
			!= siblings_parent_.at(std::make_pair(node_id, parent)).end();
}
void
graph::add_edge(int u, int v) {
  // Link v to u
//  parent_[v].push_back(u);
	parent_[v].insert(u);
	parent_map_[v].set(u);

  // Increase the in-degree
  ++ indgr_[v];

//  children_[u].push_back(v);
  	children_[u].insert(v);
  	children_map_[u].set(v);
}

bool graph::arc(int source, int target) const{
	return (children_map_.at(source).test(target));
}

bool graph::any_arc(int u, int v) const{
	return ((children_map_.count(u)&& children_map_.at(u).test(v)) || (children_map_.count(v) && children_map_.at(v).test(u)));
}

bool graph::has_children(int source) const{
	return children_map.count(source)>0;
}

void
graph::go(int now) {
	for(auto child: children_.at(now)){
		go(child);
	}

  edgeset_t& tree = descendants_.at(now);

  for(auto child: children_.at(now)){
    const edgeset_t& subtree = descendants_.at(child);

    // First push the certain children
    tree.insert(child);
    descendants_map_[now].set(child);

    // Then extend the descendant.
    tree.insert(subtree.begin(), subtree.end());
    for(auto subtree_element: subtree){
    	descendants_map_[now].set(subtree_element);
    }
  }

  // Main the siblings.
	for(auto child: children_.at(now)){
    edgeset_t& sib = siblings_.at(child);

//    for (int j = 0; j < children_[now].size(); ++ j) {
//      int child2 = children_[now][j];
	for(auto child2: children_.at(now)){
      if (child == child2) {
        continue;
      }

      const edgeset_t& subtree = descendants_.at(child2);
      // Push back the sibling node.
      sib.insert(child2);
      siblings_map_[child].set(child2);
      //
      sib.insert(subtree.begin(), subtree.end());
      for(auto subtree_element: subtree){
    	  siblings_map_[child].set(subtree_element);
      }
    }
  }
}

std::ostream& operator << (std::ostream & os, const graph & tree) {
//  int N = tree.ref->forms.size();
//  tree.

  for (int i = 0; i < tree.num_nodes; ++ i) {
	int node_id = tree.nodes.at(i);
    os << "id : " << node_id << std::endl;
//    os << " - head: " << tree.parent_[node_id] << std::endl;
    const graph::edgeset_t & par = tree.parent_.at(node_id);
    const graph::edgeset_t & chi = tree.children_.at(node_id);
    const graph::edgeset_t & des = tree.descendants_.at(node_id);
    const graph::edgeset_t & sib = tree.siblings_.at(node_id);
    int j = 0;
#define SHOW(fullname, name) os << " - " << #fullname << ": ["; \
	j = 0;\
    for(auto element: name){\
      os << element; if (j < (name).size() - 1) { os << ", "; } \
      j++;\
    } \
    os << "]" << std::endl;

    SHOW(parent, par);
    SHOW(children,   chi);
    SHOW(descendants,des);
    SHOW(siblings, sib);

#undef SHOW
  }

  return os;
}






}
