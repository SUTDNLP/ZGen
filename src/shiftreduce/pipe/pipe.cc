#include <iostream>
#include <fstream>
#include <sstream>
#include "utils/ioutils.h"
#include "utils/logging.h"
#include "types/instance_utils.h"
#include "shiftreduce/types/internal_types.h"
#include "shiftreduce/types/action_utils.h"
#include "shiftreduce/decode/none_decoder.h"
#include "shiftreduce/decode/partial_decoder.h"
#include "shiftreduce/decode/full_decoder.h"
#include "shiftreduce/decode/graph_decoder.h"
#include "shiftreduce/pipe/pipe.h"

namespace ZGen {
namespace ShiftReduce {

int label_VC = 0;
int label_APPO = 0;
int word_comma = 0;
int word_to = 0;
int word_that = 0;
int pos_comma = 0;
int pos_to = 0;
int pos_that = 0;
int sentence_no = 0;
int const_sentence_no = -1;
std::set<int> subtree_root_node_that;
std::set<int> gold_previous_words;
bool learn_mode = false;
int det_a = 0;
int det_an = 0;
int det_pos = 0;
namespace eg = ZGen::Engine;
namespace kn = ZGen::Knowledge;

Pipe::Pipe(const option_t& _opts)
  : opts(_opts), learner(0), decoder(0),
  ctx(0), model(0), constraint(0) {
  //! Create a model an load.
  model = new Model(_opts);
  load_model(opts.model_path);
}

Pipe::~Pipe() {
  if (learner)    { delete learner;     }
  if (decoder)    { delete decoder;     }
  if (ctx)        { delete ctx;         }
  if (model)      { delete model;       }
  if (constraint) { delete constraint;  }
}

bool
Pipe::load_model(const std::string& model_path) {
  std::ifstream mfs(model_path);

  if (opts.ngram_path == "") {
	_WARN << "Pipe: ngram model doesn't exist.";
  	model->ngram = NULL;
  }
  else {
	model->load(opts);
  }
  // Construct the model and alphabet.
  if (!mfs.good()) {
    _WARN << "Pipe: model doesn't exist.";
  } else {
    forms_alphabet.load(mfs);
    postags_alphabet.load(mfs);
    deprels_alphabet.load(mfs);
    deprels_alphabet_graph.load(mfs);
    model->load(mfs);
  }
  return true;
}
bool
Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(opts.model_path);
  if (!mfs.good()) {
    _WARN << "Pipe: Failed to save model.";
    return false;
  } else {
    forms_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    deprels_alphabet.save(mfs);
    deprels_alphabet_graph.save(mfs);
    model->save(mfs);
  }
  //  The model saving.
  return true;
}

void
Pipe::build_output(const StateItem& item,
    const dependency_t& input,
    dependency_t* output, std::vector<int> & final_order) {
  int N = item.ref->size();
  std::vector<int> surface_words;
  std::vector<int> order;
  action_t previous_action = action_t(Action::kShift, ZGen::Engine::TokenAlphabet::BEGIN, ZGen::Engine::TokenAlphabet::BEGIN, -1);
  for (const StateItem* p = &item; p->previous; p = p->previous) {
    if (ActionUtils::is_shift(p->last_action)) {
    	if(p->last_action.tag() == det_pos && p->last_action.word == det_a && (previous_action.prefix_type == PrefixType::kBeginsA
				|| previous_action.prefix_type == PrefixType::kBeginsE
				|| previous_action.prefix_type == PrefixType::kBeginsI
				|| previous_action.prefix_type == PrefixType::kBeginsO)){
    		surface_words.push_back(det_an);
    	}else{
    		surface_words.push_back(p->last_action.word);
    	}
    	previous_action = p->last_action;
    	order.push_back(p->last_action.index);
    }
  }
  output->clear();
//  std::map<int, std::pair<int,int>> fw_insert;
  std::vector<const StateItem*> q;
  for (const StateItem* p = &item; p->previous; p = p->previous) {
	  q.push_back(p);
  }
  std::reverse(q.begin(), q.end());
  std::reverse(surface_words.begin(), surface_words.end());
  std::reverse(order.begin(), order.end());
  std::vector<int> rank;
  // collect the rank for each form.
  rank.resize(N);
  for (int i = 0; i < N; ++ i) { rank[order[i]] = i; }
  int shift_nodes_index =0;
  int current_shift_index = 0;
  for(auto p:q){
	  if(ActionUtils::is_insert(p->last_action) || ActionUtils::is_split_arc(p->last_action)){
		  for(int rank_index = 0; rank_index< rank.size(); rank_index++){
			  if(rank[rank_index] >= current_shift_index){
				  rank[rank_index]++;
			  }
		  }
		  final_order.push_back(-1);
		  current_shift_index++;
	  }else if (ActionUtils::is_shift(p->last_action)) {
		  current_shift_index++;
		  final_order.push_back(p->last_action.index);
	  }
  }

  std::map<int,bool> has_child;
  for(auto p:q){
	  if (ActionUtils::is_shift(p->last_action)) {
		  int j = p->last_action.index;
		  has_child[item.heads[j]] = true;
	  }
  }
  int word_index = 0;
  int loop_index = 0;
  int that_index = -1;
  int that_word_index = -1;
  int last_shift = -1;
  for(auto p:q){
	  if(ActionUtils::is_insert(p->last_action)){
		std::vector<word_t> words;
		int head = -1;
		head = rank[p->last_action.index];
		output->push_back(word_comma, pos_comma,
						head,
					0, words,
					std::pair<int, int>(1,1),
					false);
		word_index++;
	  }else if (ActionUtils::is_shift(p->last_action)) {
    	int j = p->last_action.index;
    	std::vector<word_t> words(input.words.begin() + input.phrases[j].first,
    	        input.words.begin() + input.phrases[j].second);
    	int head =item.heads[j] == -1 ? -1 : rank[item.heads[j]];
    	if(word_index > 0 ){
    		const ZGen::ShiftReduce::Action last_action = q.at(loop_index - 1)->last_action;
    		if(ActionUtils::is_split_arc(last_action) && last_action.word == word_to){
			head = word_index - 1;
    		}
    		if(that_index == head && that_index > -1){
    			head = that_word_index;
    			that_word_index = -1;
    			that_index = -1;
    		}
    	}
        output->push_back(surface_words[shift_nodes_index], item.postags[j],
        		head,
            item.deprels[j], words,
			std::pair<int, int>(j,j),
            input.is_phrases[j]);
        last_shift = j;
        shift_nodes_index++;
        word_index++;
    }else if(ActionUtils::is_split_arc(p->last_action)) {
    	std::vector<word_t> words;
    	int head = rank[p->last_action.index];
    	int pos_fw = -1;
    	if(p->last_action.word == word_to){
    		pos_fw = pos_to;
		}else{
			pos_fw = pos_that;
			that_index = head;
			that_word_index = word_index;
		}
    	output->push_back(p->last_action.word, pos_fw,
    													head,
    												0, words,
    												std::pair<int, int>(1,1),
    												false);
    	word_index++;
    }
	  loop_index++;
  /*std::reverse(order.begin(), order.end());
  std::reverse(surface_words.begin(), surface_words.end());
  std::vector<int> rank;
  // collect the rank for each form.
  rank.resize(N);
  for (int i = 0; i < N; ++ i) { rank[order[i]] = i; }
  for (int i = 0, k = 0; i < N; ++ i) {
    int j = order[i];
    std::vector<word_t> words(input.words.begin() + input.phrases[j].first,
        input.words.begin() + input.phrases[j].second);
    output->push_back(input.forms[j], item.postags[j],
        (item.heads[j] == -1 ? -1 : rank[item.heads[j]]),
        item.deprels[j], words,
        dependency_t::range_t(k, k + words.size()),
        input.is_phrases[j]);
    output->push_back(surface_words[i], item.postags[j],
            (item.heads[j] == -1 ? -1 : rank[item.heads[j]]),
            item.deprels[j], words,
            dependency_t::range_t(k, k + words.size()),
            input.is_phrases[j]);
    k += words.size();*/
  }
}

bool
Pipe::learn_dataset_and_referset_validation() {
  auto nr_refer = referset.size();
  auto nr_data = dataset.size();

  if (nr_refer <= 0) {
    _ERROR << "Pipe: reference dataset should not be empty.";
    return false;
  }

  if (nr_data != nr_refer) {
    _ERROR << "Pipe: #(input) not equal #(refer)";
    return false;
  }
  return true;
}

bool
Pipe::learn_dataset_and_graphset_validation() {
  auto nr_graphs = graphs.size();
  auto nr_data = dataset.size();

  if (nr_graphs <= 0) {
    _ERROR << "Pipe: graphs dataset should not be empty.";
    return false;
  }

  if (nr_data != nr_graphs) {
    _ERROR << "Pipe: #(input) not equal #(graph)";
    return false;
  }
  return true;
}

bool
Pipe::realize() {
  namespace ioutils = ZGen::IO;

  // Read input data.
  std::istream* is = ioutils::get_istream(opts.input_path);
  bool is_inp_dep = true;
  int nr_data = ioutils::read_dependency_dataset((*is),
      forms_alphabet, postags_alphabet, deprels_alphabet,
      dataset );

  _INFO << "Pipe: " << nr_data << " input instance(s) is loaded.";
  if (nr_data <= 0) {
    _ERROR << "Pipe: input dataset should not be empty.";
    return false;
  }

  learn_mode = opts.learn;
  if (opts.learn) {
    // Open the data, 
    std::ifstream rfs(opts.reference_path.c_str());
    if (!rfs.good()) {
      _ERROR << "Pipe: failed to open reference file.";
      return false;
    }

    // Load reference dataset.
    int nr_ref_data = ioutils::read_dependency_dataset(rfs, forms_alphabet, postags_alphabet,
        deprels_alphabet, referset);

    _INFO << "Pipe(report): " << nr_ref_data << " reference instance(s) is loaded now.";

    // Validation check.
    if (!learn_dataset_and_referset_validation()) {
      return false;
    }

    std::ifstream mfs(opts.mapping_path.c_str());
    if(!mfs.good()){
    	_ERROR << "Pipe: failed to open mapping file.";
    	return false;
    }
    ioutils::read_mapping(mfs, referset);
  }
    std::ifstream pfs(opts.previous_words_that_path.c_str());
	if(!pfs.good()){
		_ERROR << "Pipe: failed to open previous_words_that_path file.";
		return false;
	}
	ioutils::read_previous_words(pfs, forms_alphabet, gold_previous_words);

    /*std::ifstream ffs(opts.function_words_path.c_str());
    if(!ffs.good()){
    	_ERROR << "Pipe: failed to open function words file.";
    	return false;
    }
    ioutils::read_function_words(ffs, referset);*/
//  }


  std::ifstream gfs(opts.graph_path.c_str());
    if(!gfs.good()){
  	  _ERROR << "Pipe: failed to open graph file.";
  	  return false;
    }
    ioutils::read_graphs(gfs, graphs,deprels_alphabet_graph);

    label_VC = deprels_alphabet_graph.encode("VC");
    label_APPO = deprels_alphabet_graph.encode("APPO");
    word_comma = forms_alphabet.encode(",");
    word_to = forms_alphabet.encode("to");
    word_that = forms_alphabet.encode("that");
    pos_comma = postags_alphabet.encode(",");
    pos_that = postags_alphabet.encode("IN");
    pos_to = postags_alphabet.encode("TO");

  det_a = forms_alphabet.encode("a");
  det_an = forms_alphabet.encode("an");
  det_pos = postags_alphabet.encode("DT");

    subtree_root_node_that.insert(forms_alphabet.encode("$"));
    subtree_root_node_that.insert(forms_alphabet.encode("asset"));
    subtree_root_node_that.insert(forms_alphabet.encode("from"));
    subtree_root_node_that.insert(forms_alphabet.encode("high"));
    subtree_root_node_that.insert(forms_alphabet.encode("if"));
    subtree_root_node_that.insert(forms_alphabet.encode("in"));
    subtree_root_node_that.insert(forms_alphabet.encode("loss"));
    subtree_root_node_that.insert(forms_alphabet.encode("practices"));
    subtree_root_node_that.insert(forms_alphabet.encode("unless"));
    subtree_root_node_that.insert(forms_alphabet.encode("why"));
  //! Load constrain.
  constraint = new kn::PostagConstraint(opts.postag_dict_path, opts.most_frequent_postag_dict_path,
      forms_alphabet, postags_alphabet);

  ctx = new DecodeContext;

  int NP = postags_alphabet.encode("NP");
  if (opts.input_type == Option::NONE) {
    decoder = new NoneDecoder(opts, ctx, model, 
					constraint,forms_alphabet, NP, deprels_alphabet.size());
  } else if (opts.input_type == Option::FULL) {
    decoder = new FullDecoder(opts, ctx, model,
					constraint, forms_alphabet, NP, deprels_alphabet.size());
  } else if (opts.input_type == Option::PARTIAL) {
    decoder = new PartialDecoder(opts, ctx, model, 
					constraint, forms_alphabet, NP, deprels_alphabet.size());
  } else if (opts.input_type == Option::GRAPH) {
	decoder = new GraphDecoder(opts, ctx, model,
					constraint, forms_alphabet, NP, deprels_alphabet.size());
  } else {
    _ERROR << "unimplemented mode.";
    exit(1);
  }
  learner = (opts.learn ? new Learner(opts, model, ctx) : NULL);
  decoder->set_pos_alphabet(&postags_alphabet);
  decoder->set_deprels_alphabet(&deprels_alphabet_graph);


  _INFO << "Pipe(report): found " << forms_alphabet.size()   << " forms.";
  _INFO << "Pipe(report): found " << postags_alphabet.size() << " postags.";
  if (opts.output_label) { _INFO << "Pipe: found " << deprels_alphabet.size() << " deprels."; }
  _INFO << "Pipe(report): found " << deprels_alphabet_graph.size() << " graph labels.";
  _INFO << "Pipe(report): labeled = " << (opts.output_label ? "True.": "False.");

  // Open output data.
  std::ostream* os = (opts.learn ? NULL: ioutils::get_ostream(opts.output_path));

  int M = dataset.size();
  _INFO << "Pipe(trace): Start " << (opts.learn? "learning.": "testing.");
  std::vector<int> out_of_beam;
  for (int i = 0; i < M; ++ i) {
    action_sequence_t gold_actions;
    std::vector<int> order;
    const dependency_t* instance = &dataset[i];
    const dependency_t* gold = NULL;
    const graph_t* graph = &graphs[i];
    graph_t shuffled_graph;
    dependency_t shuffled_instance;
    dependency_t shuffled_gold_instance;
    dependency_t output;
    sentence_no = i;

    std::map<int,int> order_map;
    if (opts.learn) {
    	gold = &referset[i];
      ActionUtils::get_correct_actions(referset[i], opts.output_label, gold_actions);
      for (const Action& a: gold_actions) { _TRACE << "Pipe(gold action): " << a <<" "<< forms_alphabet.decode(a.word); }

      graphs[i].set_ref();
      graphs[i].compute_path(deprels_alphabet_graph, forms_alphabet);
/*=======
      for (const Action& a: gold_actions) {
    	  _TRACE << "Pipe(gold action): " << a <<" "<< forms_alphabet.decode(a.word);
      }
>>>>>>> morph_a_ref*/
    } else if (opts.shuffle_instance) {
      // For the TEST phase, should shuffle the items in an instance.
      InstanceUtils::shuffle_instance((*instance), shuffled_instance, shuffled_gold_instance, order, (*graph), shuffled_graph);

      instance = (&shuffled_instance);
      shuffled_graph.set_ref();
      shuffled_graph.compute_path(deprels_alphabet_graph, forms_alphabet);
      graph = (&shuffled_graph);
      int loop_counter=0;
      for(auto elem: order){
    	  order_map[elem]=loop_counter++;
      }
    }

    Decoder::decode_result_t result = decoder->decode(instance, gold, gold_actions, graph, out_of_beam);
    /*if(const_sentence_no != -1){
    	_INFO<<"sentence_no trace "<<const_sentence_no;
    	result = decoder->decode(instance, gold, gold_actions, graph, out_of_beam);
    	const_sentence_no = -1;
    }*/

    if (opts.learn) {
      learner->set_timestamp(i+ 1);
      learner->learn(result.first, result.second, forms_alphabet);
    } else {
      std::vector<int> final_order;
      build_output(*(result.first), (*instance), &output, final_order);
      ioutils::write_dependency_instance((*os), forms_alphabet, postags_alphabet,
          deprels_alphabet, output, final_order, order_map);
    }

    if ((i+ 1) % opts.display_interval == 0) {
      _INFO << "Pipe(trace): parsed #" << (i+ 1) << " instances.";
    }

    /*if (out_of_beam.size() % opts.display_interval == 0){
    	_INFO << "Pipe(trace): out of beam #" << out_of_beam.size() << " instances.";
    }*/
  }
  _INFO << "Pipe(trace): parsed #" << M << " instances.";
  _INFO << "Pipe(trace): out of beam #"<< out_of_beam.size() << " instances.";

  if (opts.learn) {
    model->flush(M);
    _INFO << "Pipe(trace): save model to " << opts.model_path;
    save_model(opts.model_path);
  }

  if (is != (&std::cin)) { delete is; }
}

} //  end for namespace ShiftReduce
} // end for namespace ZGen
