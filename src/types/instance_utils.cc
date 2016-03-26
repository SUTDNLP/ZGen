#include <algorithm>
#include "types/instance_utils.h"
#include "engine/token_alphabet.h"

namespace ZGen {

void
InstanceUtils::shuffle_instance(const dependency_t & instance,
    dependency_t & shuffled_instance,
    std::vector<int> & order, const graph_t & graph, graph_t & shuffled_graph) {
  int N = instance.forms.size();
  order.clear();
  for (int i = 0; i < N; ++ i) {
    order.push_back(i);
  }

  // std::srand( std::time(0) );
  std::random_shuffle(order.begin(), order.end());

  shuffled_instance.forms.resize(N);
  shuffled_instance.is_phrases.resize(N);

  if (N == instance.postags.size()) {
    shuffled_instance.postags.resize(N);
  }

  if (N == instance.heads.size()) {
    shuffled_instance.heads.resize(N);
    shuffled_instance.deprels.resize(N);
  }

  if (N == instance.extras.size()) {
    shuffled_instance.extras.resize(N);
  }
  std::map<int,int> order_map;
  int loop_counter=0;
  for(auto elem: order){
	  order_map[loop_counter++]=elem;
  }
  for(auto elem : graph.parent_map)
  {
	 std::pair<int,int> parent;
	 if(order_map.count(elem.second.first)==0){
		 parent = std::make_pair(-1, elem.second.second);
	 }else{
		 parent = std::make_pair(order_map[elem.second.first], elem.second.second);
	 }
	 shuffled_graph.parent_map[order_map[elem.first]]= parent;
  }

  for(auto elem:graph.children_map){
	  std::vector<std::pair<int,int>> children;
	  for(auto elem2:elem.second){
		  children.push_back(std::make_pair(order_map[elem2.first],elem2.second));
	  }
	  if(order_map.count(elem.first)==0){
		  shuffled_graph.children_map[-1]=children;
	  }else{
		  shuffled_graph.children_map[order_map[elem.first]]= children;
	  }
  }

  for(auto elem:graph.sibling_map){
	  std::vector<std::pair<int,int>> siblings;
	  for(auto elem2: elem.second){
		  siblings.push_back(std::make_pair(order_map[elem2.first],elem2.second));
	  }
	  shuffled_graph.sibling_map[order_map[elem.first]] = siblings;
  }

  for (int i = 0; i < N; ++ i) {
    shuffled_instance.forms[order[i]] = instance.forms[i];
    shuffled_instance.is_phrases[order[i]] = instance.is_phrases[i];
    if (N == instance.postags.size()) {
      shuffled_instance.postags[order[i]] = instance.postags[i];
    }

    if (N == instance.heads.size()) {
      if (instance.heads[i] == -1) {
        shuffled_instance.heads[order[i]] = -1;
        shuffled_instance.deprels[order[i]] = Engine::TokenAlphabet::NONE;
      } else {
        shuffled_instance.heads[order[i]] = order[instance.heads[i]];
        shuffled_instance.deprels[order[i]] = instance.deprels[i];
      }
    }

    if (N == instance.extras.size()) {
      shuffled_instance.extras[order[i]] = instance.extras[i];
    }
  }

  shuffled_instance.phrases.clear();
  shuffled_instance.phrases.resize(N);
  shuffled_instance.words.clear();
  shuffled_instance.words.reserve(instance.words.size());

  for (int rank = 0, k = 0; rank < order.size(); ++ rank) {
    for (int j = 0; j < order.size(); ++ j) {
      if (order[j] == rank) {
        shuffled_instance.phrases[rank].first = k;
        for (int l = instance.phrases[j].first;
            l < instance.phrases[j].second; ++ l, ++ k) {
          shuffled_instance.words.push_back(instance.words[l]);
        }
        shuffled_instance.phrases[rank].second = k;
        break;
      }
    }
  }
}

} //  end for namespace ZGen
