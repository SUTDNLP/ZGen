/*
 * graph.cpp
 *
 *  Created on: 06-Jul-2015
 *      Author: ratish
 */

#include "graph.h"
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
	parent_map[node] = std::make_pair(parent,label);
	children_map[parent].push_back(std::make_pair(node,label));
	return parent_map.size();
}

bool sort_pair (std::pair<int , int> i,std::pair<int , int> j) { return (i.second<j.second); }	//sort by label

void graph::add_ancestors(){

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
	 * in case of no dep input, no entry in sibling map
	 */
	for(auto elem: children_map){
//		int key = elem.first;
		std::vector<std::pair<int,int>> children = elem.second;
		if(children.size()==1 || elem.first==-1){
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

}
}
