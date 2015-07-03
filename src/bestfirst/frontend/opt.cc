#include <iostream>
#include <boost/log/trivial.hpp>
#include "utils/logging.h"
#include "bestfirst/frontend/opt.h"

bool parse_options(bool learn,
    const boost::program_options::variables_map& vm,
    ZGen::BestFirst::Option& opts) {
  namespace utils = ZGen::Utility;
  // 
  utils::init_boost_logging(vm.count("verbose") != 0);

  opts.learn = learn;

  if (!vm.count("model")) {
    // Model option is not specified.
    BOOST_LOG_TRIVIAL(error) << "model path is not specified.";
    return false;
  }
  opts.model_path = vm["model"].as<std::string>();

  if (!vm.count("input")) {
    // Input path is not specified, use stdin instead.
    opts.input_path = "";
  } else {
    opts.input_path = vm["input"].as<std::string>();
  }

  if (!vm.count("reference")) {
    // Reference path is not specified, maybe test mode is activated.
    opts.reference_path = "";
  } else {
    opts.reference_path = vm["reference"].as<std::string>();
  }

  if (!vm.count("output")) {
    // Output path is not specified, use stdout instead.
    opts.output_path = "";
  } else {
    opts.output_path = vm["output"].as<std::string>();
  }

  if (!vm.count("posdict")) {
    // Posdict is not specified.
    opts.postag_dict_path = "";
  } else {
    opts.postag_dict_path = vm["posdict"].as<std::string>();
  }

  if (!vm.count("nbest")) {
    // Output nbest result.
    opts.nbest = 1;
  } else {
    opts.nbest = vm["nbest"].as<int>();
  }

  if (!vm.count("timeout")) {
    // Set the timeout to default 5.0 second.
    opts.timeout = 5.;
  } else {
    opts.timeout = vm["timeout"].as<double>();
  }

  opts.display_interval = 1;
  if (vm.count("display")) {
    opts.display_interval = vm["display"].as<int>();
  }

  return true;
}


