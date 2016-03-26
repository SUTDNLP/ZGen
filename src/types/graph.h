/*
 * graph.h
 *
 *  Created on: 06-Jul-2015
 *      Author: ratish
 */

#ifndef SRC_SHIFTREDUCE_TYPES_GRAPH_H_
#define SRC_SHIFTREDUCE_TYPES_GRAPH_H_
#include <utility>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>
#include "engine/token_alphabet.h"
#include <boost/functional/hash.hpp>
namespace ZGen {
enum Mode{
	kAncestor=0,
	kDescendant,
	kSibling
};

struct pairhash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
    std::size_t seed = 0;
    boost::hash_combine(seed, x.first);
    boost::hash_combine(seed, x.second);
    return seed;
  }
};


class graph {
public:
	graph();
	virtual ~graph();
	std::unordered_map<int, std::pair<int, int>> parent_map; //map of node to parent. parent is represented by node id and label of the edge
	std::unordered_map<int, std::vector<std::pair<int, int>>> children_map;
	std::unordered_map<int, std::vector<std::pair<int, int>>> sibling_map;
	//TODO Very Important: any new map, handle order in shuffled graph
	int add_to_parent_map(int node, int parent, int label);
	void add_ancestors();
};
typedef graph graph_t;
}
#endif /* SRC_SHIFTREDUCE_TYPES_GRAPH_H_ */
