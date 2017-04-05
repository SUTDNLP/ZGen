#include <algorithm>
#include "types/instance_utils.h"
#include "engine/token_alphabet.h"

namespace ZGen {

void
InstanceUtils::shuffle_instance(const dependency_t & instance, dependency_t & shuffled_instance, dependency_t & shuffled_gold_instance,
    std::vector<int> & order, const graph_t & graph, graph_t & shuffled_graph) {
  int N = instance.forms.size();
  order.clear();
  for (int i = 0; i < N; ++ i) {
    order.push_back(i);
  }

  // std::srand( std::time(0) );
  std::random_shuffle(order.begin(), order.end());

  shuffled_instance.forms.resize(N);
  shuffled_instance.lexemes.resize(N);
  shuffled_instance.prefix_types.resize(N);
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

  shuffled_instance.tense_participles.resize(N);
	shuffled_instance.numbers.resize(N);
	shuffled_instance.brackets.resize(N);
	shuffled_instance.quotes.resize(N);
	shuffled_instance.quote_brackets.resize(N);

  std::map<int,int> order_map;
  int loop_counter=0;
  for(auto elem: order){
	  order_map[loop_counter++]=elem;
  }
  for(auto elem : graph.parent_map)
  {
	 std::vector<std::pair<int,int>> parents;
	 for(auto elem2: elem.second){
		 std::pair<int,int> parent;
		 if(order_map.count(elem2.first)==0){
			 parent = std::make_pair(-1, elem2.second);
		 }else{
			 parent = std::make_pair(order_map[elem2.first], elem2.second);
		 }
		 parents.push_back(parent);
	 }

	 shuffled_graph.parent_map[order_map[elem.first]]= parents;
  }

  /*shuffled_gold_instance.forms = gold.forms;
  shuffled_gold_instance.heads = gold.heads;
  for(auto elem: gold.input_gold_node_mapping){
	  shuffled_gold_instance.input_gold_node_mapping[order_map[elem.first]] = elem.second;
  }

  for(auto elem: gold.gold_input_node_mapping){
	  shuffled_gold_instance.gold_input_node_mapping[elem.first] = order_map[elem.second];
  }

  shuffled_gold_instance.function_words = gold.function_words;*/


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

  /*for(auto elem:graph.ancestors){
	  for(auto entry: elem.second){
		  int ancestor = entry;
		  if(entry != -1){
			  ancestor = order_map[entry];
		  }
		  shuffled_graph.ancestors[order_map[elem.first]].push_back(ancestor);
	  }
  }*/
  
  for(auto elem:graph.path_map){
	  if(elem.first.second == -1){
		  shuffled_graph.path_map[std::make_pair(order_map[elem.first.first],elem.first.second)] = elem.second;
	  }else{
		  shuffled_graph.path_map[std::make_pair(order_map[elem.first.first],order_map[elem.first.second])] = elem.second;
	  }
  }
  
  for (int i = 0; i < N; ++ i) {
    shuffled_instance.forms[order[i]] = instance.forms[i];
    shuffled_instance.lexemes[order[i]] = instance.lexemes[i];
    shuffled_instance.prefix_types[order[i]] = instance.prefix_types[i];
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

//    shuffled_instance.participles[order[i]] = instance.participles[i];
    shuffled_instance.tense_participles[order[i]] = instance.tense_participles[i];
//    shuffled_instance.tenses[order[i]] = instance.tenses[i];
    shuffled_instance.brackets[order[i]] = instance.brackets[i];
    shuffled_instance.numbers[order[i]] = instance.numbers[i];
    shuffled_instance.quotes[order[i]] = instance.quotes[i];
    shuffled_instance.quote_brackets[order[i]] = instance.quote_brackets[i];
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
