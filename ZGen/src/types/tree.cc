#include "types/tree.h"
#include <cstring>

namespace ZGen {

DependencyTree::DependencyTree() : ref(0) {}

DependencyTree::~DependencyTree() {}

DependencyTree& DependencyTree::operator = (const DependencyTree& other) {
  std::cerr << "Copy operation is not allowed." << std::endl;
  return (*this);
}

void
DependencyTree::dependency_to_tree(const dependency_t* _ref) {
  ref = _ref;
  int N = _ref->size();

  reset(N);

  for (int i = 0; i < N; ++ i) {
    int v = _ref->heads[i];
    if (v >= 0) {
      add_edge(v, i);
    }
  }
}

int
DependencyTree::set_ref(const dependency_t* _ref) {
  dependency_to_tree(_ref);

  int root = -1;
  int N = _ref->size();
  for (int i = 0; i < N; ++ i) {
    if (indgr_[i] == 0) {
      root = i;
      break;
    }
  }

  go(root);

  return root;
}

const DependencyTree::edgeset_t&
DependencyTree::descendants(int i) {
  return descendants_[i];
}

const DependencyTree::edgeset_t&
DependencyTree::siblings(int i) {
  return siblings_[i];
}

const DependencyTree::edgeset_t&
DependencyTree::children(int i) {
  return children_[i];
}

int
DependencyTree::head(int i) {
  return parent_[i];
}

bool
DependencyTree::arc(int from, int to) {
  return parent_[to] == from;
}

void
DependencyTree::reset(int N) {
  // Set all parent to un init.
  memset(parent_, -1, sizeof(parent_));

  // Set all the in degree to zero.
  memset(indgr_, 0, sizeof(indgr_));

  for (int i = 0; i < N; ++ i) {
    // Clear chidren cache.
    children_[i].clear();

    // Clear descendant cache.
    descendants_[i].clear();

    // Clear siblings cache.
    siblings_[i].clear();
  }
}


void
DependencyTree::add_edge(int u, int v) {
  // Link v to u
  parent_[v] = u;

  // Increase the in-degree
  ++ indgr_[v];

  //
  children_[u].push_back(v);
}


void
DependencyTree::go(int now) {
  for (int i = 0; i < children_[now].size(); ++ i) {
    int child = children_[now][i];
    go(child);
  }

  edgeset_t& tree = descendants_[now];

  for (int i = 0; i < children_[now].size(); ++ i) {
    int child = children_[now][i];
    const edgeset_t& subtree = descendants_[child];

    // First push the certain children
    tree.push_back(child);

    // Then extend the descendant.
    tree.reserve(tree.size() + std::distance(subtree.begin(), subtree.end()));
    tree.insert(tree.end(), subtree.begin(), subtree.end());
  }

  // Main the siblings.
  for (int i = 0; i < children_[now].size(); ++ i) {
    int child = children_[now][i];
    edgeset_t& sib = siblings_[child];

    for (int j = 0; j < children_[now].size(); ++ j) {
      int child2 = children_[now][j];
      if (child == child2) {
        continue;
      }

      const edgeset_t& subtree = descendants_[child2];
      // Push back the sibling node.
      sib.push_back(child2);
      //
      sib.reserve(sib.size() + std::distance(subtree.begin(), subtree.end()));
      sib.insert(sib.end(), subtree.begin(), subtree.end());
    }
  }
}

std::ostream& operator << (std::ostream & os, const DependencyTree & tree) {
  int N = tree.ref->forms.size();

  for (int i = 0; i < N; ++ i) {
    os << "id : " << i << std::endl;
    os << " - head: " << tree.parent_[i] << std::endl;

    const DependencyTree::edgeset_t & chi = tree.children_[i];
    const DependencyTree::edgeset_t & des = tree.descendants_[i];
    const DependencyTree::edgeset_t & sib = tree.siblings_[i];

#define SHOW(fullname, name) os << " - " << #fullname << ": ["; \
    for (int j = 0; j < (name).size(); ++ j) { \
      os << (name)[j]; if (j < (name).size() - 1) { os << ", "; } \
    } \
    os << "]" << std::endl;

    SHOW(children,   chi);
    SHOW(descendants,des);
    SHOW(siblings, sib);

#undef SHOW
  }

  return os;
}

void
DependencyTreeWithGuidance::reset(int N) {
  DependencyTree::reset(N);

  memset(lvl0_deprels, 0, sizeof(lvl0_deprels));
  memset(lvl1_deprels, 0, sizeof(lvl1_deprels));
  memset(lvl2_deprels, 0, sizeof(lvl2_deprels));
}

int
DependencyTreeWithGuidance::set_ref(const dependency_t* ref) {
  int root = DependencyTree::set_ref(ref);
  color(root, 0, 0, 0);
  return root;
}


void
DependencyTreeWithGuidance::color(int now, int _0, int _1, int _2) {
  //std::cout << now << " " << _0 << " " << _1 << " " << _2 << std::endl;
  lvl0_deprels[now]= _0;
  lvl1_deprels[now]= _1;
  lvl2_deprels[now]= _2;
  for (int i = 0; i < children_[now].size(); ++ i) {
    int child = children_[now][i];
    int new_0 = (_0 ?             _0: ref->deprels[child]);
    int new_1 = (_1 && _0 ?       _1: (_0 ? ref->deprels[child]: 0));
    int new_2 = (_2 && _1 && _0 ? _2: (_1 ? ref->deprels[child]: 0));
    color(child, new_0, new_1, new_2);
  }
}

int
DependencyTreeWithGuidance::lvl0(int i) const {
  return lvl0_deprels[i];
}

int
DependencyTreeWithGuidance::lvl1(int i) const {
  return lvl1_deprels[i];
}

int
DependencyTreeWithGuidance::lvl2(int i) const {
  return lvl2_deprels[i];
}

int
DependencyForest::set_ref(const dependency_t* ref) {
  dependency_to_tree(ref);

  int N = ref->size();

  for (int i = 0; i < N; ++ i) {
    if (indgr_[i] == 0) {
      go(i);
      rootize(i, i, 0);
    }
  } 

  for (int i =0 ;i < N; ++ i) {
    for (int j= i+ 1; j < N; ++ j) {
      if (same_ancestor(i, j)) {
        bool done = false;
        for (int p = i; p != root_[i] && !done; p = parent_[p]) {
          for (int q = j; q != root_[j]; q = parent_[q]) {
            if (p == q) { lca_[i][j] = p; done = true; break; }
          }
        }
        if (!done) {
          lca_[i][j] = root_[i];
        }
      }
    }
  }
  return 0;
}

void
DependencyForest::reset(int N) {
  DependencyTree::reset(N);
  memset(root_, -1, sizeof(root_));
  memset(depth_, 0, sizeof(depth_));
  memset(lca_, -1, sizeof(lca_));
}

void
DependencyForest::rootize(int now, int root_flag, int depth) {
  root_[now] = root_flag;
  depth_[now] = depth;
  const edgeset_t& cc = children_[now];
  for (int i = 0; i < cc.size(); ++ i) {
    int c = cc[i];
    rootize(c, root_flag, depth+ 1);
  }
}

int
DependencyForest::root(int i) const {
  return root_[i];
}

int
DependencyForest::depth(int i) const {
  return depth_[i];
}

int
DependencyForest::lca(int u, int v) const {
  if (u > v) {
    std::swap(u, v);
  }
  return lca_[u][v];
}

bool
DependencyForest::is_root(int i) const {
  return (root_[i] == i);
}

bool
DependencyForest::same_ancestor(int u, int v) const {
  return (root_[u] == root_[v]);
}

std::ostream& operator << (std::ostream & os, const DependencyForest & tree) {
  int N = tree.ref->forms.size();

  for (int i = 0; i < N; ++ i) {
    os << "id : " << i << std::endl;
    os << " - head: " << tree.parent_[i];
    os << " - root: " << tree.root_[i] << std::endl;
    os << " - lca: [";
    for (int j = 0; j < N; ++ j) {
      if (tree.lca(i, j) >= 0) {
        os << j<< ":" <<tree.lca(i, j) << ", ";
      }
    }
    os << "]" << std::endl;
    const DependencyTree::edgeset_t & chi = tree.children_[i];
    const DependencyTree::edgeset_t & des = tree.descendants_[i];
    const DependencyTree::edgeset_t & sib = tree.siblings_[i];

#define SHOW(fullname, name) os << " - " << #fullname << ": ["; \
    for (int j = 0; j < (name).size(); ++ j) { \
      os << (name)[j]; if (j < (name).size() - 1) { os << ", "; } \
    } \
    os << "]" << std::endl;

    SHOW(children,   chi);
    SHOW(descendants,des);
    SHOW(siblings, sib);

#undef SHOW
  }
  return os;
}

} //  end for namespace ZGen
