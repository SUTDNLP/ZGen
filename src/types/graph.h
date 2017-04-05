/*
 * graph.h
 *
 *  Created on: 06-Jul-2015
 *      Author: ratish
 */

#ifndef SRC_SHIFTREDUCE_TYPES_GRAPH_H_
#define SRC_SHIFTREDUCE_TYPES_GRAPH_H_
#include "settings.h"
#include "types/instance.h"
#include <utility>
#include <string>
#include <tuple>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/tuple/tuple_io.hpp>
#include "engine/token_alphabet.h"
#include <boost/functional/hash.hpp>
namespace ZGen {
//namespace eg = ZGen::Engine;
enum class Direction{
	kNone=0,
	kAncestor,
	kDescendant,
	kSibling
};
/*
enum EdgeLabel {
	SUBJ=0,
	SUB,
	A0,
	A1,
	A2,
	A3,
	A4,
	A5,
	OTHER
};*/

struct pairhash {
public:
  template <typename T, typename U>
  std::size_t operator()(const std::pair<T, U> &x) const
  {
//    return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
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
	typedef std::unordered_set<int> edgeset_t;
	typedef std::bitset<kMaxNumberOfWords> nodes_mask_t;
	typedef std::unordered_map<word_t, nodes_mask_t> map_t;
	typedef std::unordered_map<word_t, edgeset_t> map_s;
	typedef std::unordered_map<std::pair<int, int>,edgeset_t, pairhash> map_u;
	std::unordered_map<int, std::vector<std::pair<int, int>>> parent_map; //map of node to parent. parent is represented by node id and label of the edge
	std::unordered_map<int, std::vector<std::pair<int, int>>> children_map;
	std::unordered_map<int, std::vector<std::pair<int, int>>> sibling_map;
	std::unordered_map<std::pair<int,int>,boost::tuples::tuple<int,Direction,int>, pairhash> path_map;
	std::unordered_map<std::pair<int, int>,int, pairhash> label_path_map;
	//TODO Very Important: any new map, handle order in shuffled graph
	int add_to_parent_map(int node, int parent, int label);
	void bfs();
	graph::edgeset_t get_direct_descendants(int word_index);
	void compute_path(Engine::TokenAlphabet& deprels_alphabet, Engine::TokenAlphabet& forms_alphabet);
	void add_ancestors();
	graph::edgeset_t get_non_direct_descendants(int top0);


	map_t children_map_;
	map_t descendants_map_;
	map_t parent_map_;
	map_t siblings_map_;

	map_s children_;
	map_s parent_;
	map_s descendants_;
	map_s siblings_;
	map_u siblings_parent_;

	int num_nodes;
	std::unordered_map<int,int> nodes;

	/**
	* Clear all the cached information.
	*/
	void reset(int N = kMaxNumberOfWords);

	/**
	   * Set the dependency tree reference onto the tree.
	   *
	   *  @param[in]  _ref    The reference of the dependency tree.
	   *  @return     int     The root index.
	   */
	void set_ref();

	//! Convert the dependency parse into the dependency tree.
	void dependency_to_graph();

	/**
	* The deep first process.
	*
	*  @param[in]  now   The currently visited ode.
	*/
	void go(int now);

	//! Record the in degree of the graph.
	int indgr_[kMaxNumberOfWords];

	//! Return if there is an edge between source and target.
	bool arc(int source, int target) const;
	/**
	* Add edge from `u` to `v`
	*
	*  @param[in]  u   The out link node.
	*  @param[in]  v   The in link node.
	*/
	void add_edge(int u, int v);

	bool has_common_parent(int u, int v) const;

	graph::edgeset_t get_common_parents(int u, int v) const;

	graph::edgeset_t get_common_parents(int u, int v, int w) const;

	bool is_parent_sibling(int u, int v) const;

	bool sibling_parent_entry_exists(int node_id, int parent, int entry) const;

	bool is_function_word(int index) const;

	bool descendant_exists(int u) const;

	bool is_descendant(int u, int v) const;

	bool path_map_exists(int u, int v) const;

	boost::tuples::tuple<int,Direction,int> get_path_map_entry(int u, int v) const;

	int get_path_label_entry(int u, int v) const;

	int is_sibling(int u, int v) const;

	std::set<int> get_inflection_points(int u, int v) const;

	int is_parent(int u, int v) const;

	bool any_arc(int u, int v) const;

	bool has_children(int source) const;

	graph::edgeset_t bypass_shifted_nodes(int parent, std::bitset<kMaxNumberOfWords> buffer) const;

	graph::edgeset_t get_parent_and_siblings(int elem, std::bitset<kMaxNumberOfWords> buffer, int sentence_no) const;

	friend std::ostream& operator << (std::ostream & os, const graph & tree);

/*private:
	std::map<std::string,EdgeLabel> edgelabel_map;
	void initialize_edgelabel();
	EdgeLabel mapped_edgelabel(std::string label);*/
};
typedef graph graph_t;
}
#endif /* SRC_SHIFTREDUCE_TYPES_GRAPH_H_ */
