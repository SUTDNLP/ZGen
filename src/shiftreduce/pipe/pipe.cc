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
#include "shiftreduce/pipe/pipe.h"

namespace ZGen {
namespace ShiftReduce {

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
    model->save(mfs);
  }
  //  The model saving.
  return true;
}

void
Pipe::build_output(const StateItem& item,
    const dependency_t& input,
    dependency_t* output) {
  int N = item.ref->size();
  std::vector<int> order;
  for (const StateItem* p = &item; p->previous; p = p->previous) {
    if (ActionUtils::is_shift(p->last_action)) { order.push_back(p->last_action.index); }
  }
  output->clear();
  if (order.size() != N) {
    _WARN << "#"<< timestamp << " ORDER is not equal to the sent size "
      << "order: " << order.size() << " N: " << N;
    return;
  }
  std::reverse(order.begin(), order.end());
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
    k += words.size();
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
Pipe::realize() {
  namespace ioutils = ZGen::IO;

  // Read input data.
  std::istream* is = ioutils::get_istream(opts.input_path);
  int nr_data = ioutils::read_dependency_dataset((*is),
      forms_alphabet, postags_alphabet, deprels_alphabet,
      dataset);

  _INFO << "Pipe: " << nr_data << " input instance(s) is loaded.";
  if (nr_data <= 0) {
    _ERROR << "Pipe: input dataset should not be empty.";
    return false;
  }

  if (opts.learn) {
    // Open the data, 
    std::ifstream rfs(opts.reference_path.c_str());
    if (!rfs.good()) {
      _ERROR << "Pipe: failed to open reference file.";
      return false;
    }

    // Load reference dataset.
    ioutils::read_dependency_dataset(rfs, forms_alphabet, postags_alphabet,
        deprels_alphabet, referset);

    _INFO << "Pipe(report): " << nr_data << " reference instance(s) is loaded.";

    // Validation check.
    if (!learn_dataset_and_referset_validation()) {
      return false;
    }
  }

  //! Load constrain.
  constraint = new kn::PostagConstraint(opts.postag_dict_path,
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
  } else {
    _ERROR << "unimplemented mode.";
    exit(1);
  }
  learner = (opts.learn ? new Learner(opts, model, ctx) : NULL);


  _INFO << "Pipe(report): found " << forms_alphabet.size()   << " forms.";
  _INFO << "Pipe(report): found " << postags_alphabet.size() << " postags.";
  if (opts.output_label) { _INFO << "Pipe: found " << deprels_alphabet.size() << " deprels."; }
  _INFO << "Pipe(report): labeled = " << (opts.output_label ? "True.": "False.");

  // Open output data.
  std::ostream* os = (opts.learn ? NULL: ioutils::get_ostream(opts.output_path));

  int M = dataset.size();
  _INFO << "Pipe(trace): Start " << (opts.learn? "learning.": "testing.");
  for (int i = 0; i < M; ++ i) {
    action_sequence_t gold_actions;
    std::vector<int> order;
    const dependency_t* instance = &dataset[i];
    dependency_t shuffled_instance;
    dependency_t output;

    if (opts.learn) {
      ActionUtils::get_correct_actions(referset[i], opts.output_label, gold_actions);
      for (const Action& a: gold_actions) { _TRACE << "Pipe(gold action): " << a; }
    } else if (opts.shuffle_instance) {
      // For the TEST phase, should shuffle the items in an instance.
      InstanceUtils::shuffle_instance((*instance), shuffled_instance, order);
      instance = (&shuffled_instance);
    }

    Decoder::decode_result_t result = decoder->decode(instance, gold_actions);

    if (opts.learn) {
      learner->set_timestamp(i+ 1);
      learner->learn(result.first, result.second, forms_alphabet);
    } else {
      build_output(*(result.first), (*instance), &output);
      ioutils::write_dependency_instance((*os), forms_alphabet, postags_alphabet,
          deprels_alphabet, output);
    }

    if ((i+ 1) % opts.display_interval == 0) {
      _INFO << "Pipe(trace): parsed #" << (i+ 1) << " instances.";
    }
  }
  _INFO << "Pipe(trace): parsed #" << M << " instances.";

  if (opts.learn) {
    model->flush(M);
    _INFO << "Pipe(trace): save model to " << opts.model_path;
    save_model(opts.model_path);
  }

  if (is != (&std::cin)) { delete is; }
}

} //  end for namespace ShiftReduce
} // end for namespace ZGen
