#ifndef __ZGEN_TYPES_DEPENDENCY_TREE_H__
#define __ZGEN_TYPES_DEPENDENCY_TREE_H__

#include "settings.h"
#include "types/instance.h"

namespace ZGen {

class DependencyTree {
public:
  typedef std::vector<int> edgeset_t;

  DependencyTree();

  ~DependencyTree();

  /**
   * Set the dependency tree reference onto the tree.
   *
   *  @param[in]  _ref    The reference of the dependency tree.
   *  @return     int     The root index.
   */
  virtual int set_ref(const dependency_t* _ref);

  //
  friend std::ostream & operator << (std::ostream & ofs, const DependencyTree & tree);

  //! Access the siblings of node i
  const edgeset_t& siblings(int i);

  //! Access the descendants of node i
  const edgeset_t& descendants(int i);

  //! Access the children of node i
  const edgeset_t& children(int i);

  //! Access the head of node i
  int head(int i);

  //! Return if there is an edge between source and target.
  bool arc(int source, int target);
protected:
  /**
   * Clear all the cached information.
   */
  virtual void reset(int N = kMaxNumberOfWords);

  //! Convert the dependency parse into the dependency tree.
  void dependency_to_tree(const dependency_t* parse);

  /**
   * The deap first process.
   *
   *  @param[in]  now   The currently visited ode.
   */
  void go(int now);

  //! The reference to the dependency parse instance.
  const dependency_t * ref;

  //! Record the children for each node.
  edgeset_t children_[kMaxNumberOfWords];

  //! Record the parent for each node.
  int parent_[kMaxNumberOfWords];

  //! Record the in degree of the graph.
  int indgr_[kMaxNumberOfWords];

  //! Record the descendant for each node.
  edgeset_t descendants_[kMaxNumberOfWords];

  //! Record the siblings for each node.
  edgeset_t siblings_[kMaxNumberOfWords];

private:
  DependencyTree(DependencyTree & other);

  //! ban the reference assign
  DependencyTree& operator = (const DependencyTree & other);

  /**
   * Add edge from `u` to `v`
   *
   *  @param[in]  u   The out link node.
   *  @param[in]  v   The in link node.
   */
  void add_edge(int u, int v);
};


class DependencyTreeWithGuidance: public DependencyTree {
public:
  int set_ref(const dependency_t* ref);

  /**/
  int lvl0(int i) const;

  /**/
  int lvl1(int i) const;

  /**/
  int lvl2(int i) const;

protected:
  void reset(int N = kMaxNumberOfWords);

private:
  void color(int now, int _0, int _1, int _2);

  int lvl0_deprels[kMaxNumberOfWords];
  int lvl1_deprels[kMaxNumberOfWords];
  int lvl2_deprels[kMaxNumberOfWords];
};

//
class DependencyForest : public DependencyTree {
public:
  /**/
  int set_ref(const dependency_t* ref);

  /**/
  int root(int i) const;

  /**/
  int depth(int i) const;

  /**/
  int lca(int u, int v) const;

  /**/
  bool is_root(int i) const;

  /**/
  bool same_ancestor(int u, int v) const;

  //
  friend std::ostream & operator << (std::ostream & ofs, const DependencyForest& tree);
protected:
  void reset(int N = kMaxNumberOfWords);

private:
  void rootize(int now, int root, int depth);

  int root_[kMaxNumberOfWords];
  int depth_[kMaxNumberOfWords];
  int lca_[kMaxNumberOfWords][kMaxNumberOfWords];
};


}

#endif  //  end for __SR_DEPENDENCY_H__
