#include <iostream>
#include <fstream>
#include <ctime>
#include <boost/log/trivial.hpp>
#include "utils/ioutils.h"
#include "engine/engine.h"
#include "bestfirst/pipe/pipe.h"
#include "bestfirst/search/span_utils.h"

namespace ZGen {

namespace BestFirst {

namespace kn= ZGen::Knowledge;
namespace eg= ZGen::Engine;
namespace ioutils = ZGen::IO;

Pipe::Pipe(const Option& _opts)
  : opts(_opts) {
  model= new Model;
  std::ifstream mfs(_opts.model_path.c_str());

  if (!mfs.good()) {
    BOOST_LOG_TRIVIAL(warning) << "Pipe: model doesn't exists.";
  } else {
    bool loaded = eg::WordEngine::get_mutable_instance().load(mfs);
    if (!loaded) {
      BOOST_LOG_TRIVIAL(warning) << "Pipe: failed to load word alphabet.";
    }
    if (loaded) {
      if (!model->load(mfs)) {
        BOOST_LOG_TRIVIAL(warning) << "Pipe: failed to load model.";
      } else {
        BOOST_LOG_TRIVIAL(info) << "Pipe: model successfully loaded.";
      }
    }
  }

  allocator = new SpanAllocator(10005);
  constrain = new kn::PostagConstrain(_opts.postag_dict_path.c_str());
  factory = new SpanFactory(constrain, allocator);

  learner = (opts.learn? new Learner(model, 5): NULL);
}

Pipe::~Pipe() {
  if (learner)    { delete learner; }
  if (model)      { delete model; }
  if (allocator)  { delete allocator; }
  if (factory)    { delete factory; }
}

void
Pipe::realize() {
  if (!prepare_data()) {
    return;
  }

  int N = dataset.size();

  for (int i = 0; i < N; ++ i) {
    const dependency_t* data= &dataset[i];
    const dependency_t* refer= &referset[i];

    gold_spans.clear();
    chart_mask.clear();
    chart.init(65535);
    agenda.clear();

    if (opts.learn) {
      factory->create_gold_spans_from_reference(data, refer, gold_spans);
    }

    int L = data->size();
    std::vector<Span *> spans;
    factory->create_initial_spans_from_input(data, spans);

    // Push these spans into the agenda.
    for (int j = 0; j < spans.size(); ++ j) {
      Span* span = spans[j];
      span->score = model->score(*span, true);
      agenda.push(span);
      BOOST_LOG_TRIVIAL(trace) << "Pipe(realize init span): " << (*span);
    }

    if (opts.learn) {
      //learner->config_timestamp(i);
      learner->learn(gold_spans, agenda);
    }

    long start_time = clock();
    while (!agenda.empty()) {
      Span* best_span = agenda.top(); agenda.pop();
      BOOST_LOG_TRIVIAL(trace) << "Pipe(realize visit span): " << (*best_span);

      if (best_span->complete() && opts.greedy) {
        // Use greedy search and found answer.
        break;
      }

      if (opts.learn) {
        // Perform the learning.
        int code = learner->learn(gold_spans, agenda);
        if (code == Learner::NO_GOLD_IN_AGENDA) {
          BOOST_LOG_TRIVIAL(info) << "Pipe(learning result): no gold in agenda.";
          break;
        }
      }

      if (!opts.learn && (clock() - start_time)/ CLOCKS_PER_SEC > opts.timeout) {
        break;
      }

      factory->create_spans_from_chart(best_span, &chart, spans);
      for (int k = 0; k < spans.size(); ++ k) {
        Span* span = spans[k];
        span->score = model->score(*span, true);
        agenda.push(span);
      }

      if (chart_mask.find(best_span) == chart_mask.end()) {
        chart.insert(best_span);
        chart_mask.insert(best_span);
      }
    }

    if (!opts.learn) {
      dependency_t output;
      const Span* best = NULL;
      for (SpanKMaxHeap::iterator itx = chart.begin(); itx != chart.end(); ++ itx) {
        if (best == NULL || best->score < (*itx)->score) {
          best = (*itx);
        }
      }
      build_output(best, output);
      ioutils::write_dependency_instance(std::cout, output);
    }
    if ((i+ 1) % opts.display_interval == 0) {
      BOOST_LOG_TRIVIAL(info) << "Pipe(trace): parsed #" << (i+ 1) << " instances.";
    }
  }
}

void
Pipe::save_model() {
  std::ofstream ofs(opts.model_path.c_str());
  if (!ofs.good()) {
    return;
  }

  Engine::WordEngine::get_mutable_instance().save(ofs);
  model->save(ofs);
  ofs.close();
}

bool
Pipe::prepare_data() {
  namespace ioutils = ZGen::IO;
  std::istream* is = ioutils::get_istream(opts.input_path);
  int nr_data = ioutils::read_dependency_dataset((*is), dataset);

  BOOST_LOG_TRIVIAL(info) << "Pipe: [" << nr_data << "] input instance(s) is loaded.";
  if (nr_data <= 0) {
    BOOST_LOG_TRIVIAL(error) << "Pipe: input dataset should not be empty.";
    return false;
  }

  if (opts.learn) {
    // Open the data,
    std::ifstream rfs(opts.reference_path.c_str());
    if (!rfs.good()) {
      BOOST_LOG_TRIVIAL(error) << "Pipe: failed to open reference file.";
      return false;
    }
    // Load.
    int nr_refer = ioutils::read_dependency_dataset(rfs, referset);
    BOOST_LOG_TRIVIAL(info) << "Pipe: [" << nr_data
      << "] reference instance(s) is loaded.";
    // Validation check.
    if (nr_refer <= 0) {
      BOOST_LOG_TRIVIAL(error) << "Pipe: reference dataset should not be empty.";
      return false;
    }
    if (nr_data != nr_refer) {
      BOOST_LOG_TRIVIAL(error) << "Pipe: #(input) not equal #(refer)";
      return false;
    }
  }

  return true;
}

void
Pipe::build_output(const Span* span, dependency_t& output) {
  std::vector<int> indices;
  SpanUtils::get_all_linearized_indices(span, indices);
  const dependency_t* ref = span->ref;

  for (int i = 0, k = 0; i < indices.size(); ++ i) {
    int j = indices[i];
    std::vector<word_t> words(ref->words.begin() + ref->phrases[j].first,
        ref->words.begin() + ref->phrases[j].second);
    output.push_back(ref->forms[j],
        ref->postags[j],
        -1,
        //(item.heads[j] == -1 ? -1 : rank[item.heads[j]]),
        ref->deprels[j],
        words,
        dependency_t::range_t(k, k + words.size()),
        ref->is_phrases[j]);
    k += words.size();
  }
}

} //  end for namespace BestFirst

} //  end for namespace ZGen

