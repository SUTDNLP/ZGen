#include "shiftreduce/frontend/opt.h"
#include "utils/logging.h"

// Parse the configuration.
bool parse_options(bool learn,
    boost::program_options::variables_map& vm,
    ZGen::ShiftReduce::option_t& opts) {
  namespace utils = ZGen::Utility;
  namespace sr = ZGen::ShiftReduce;

  opts.learn = learn;
  opts.shuffle_instance = true; // always shuffle the instance.

  // If show help, return false.
  if (vm.count("help")) { return false; }

  // [initialize the boost logging
  opts.verbose = (vm.count("verbose")!= 0);
  utils::init_boost_logging(opts.verbose);

  // [Parse the option 
  if (!vm.count("type")) {
    _ERROR << "Type [none|postag|partial|full] must be specified.";
    return false;
  } else {
    if (vm["type"].as<std::string>() == "none") {
      opts.input_type = sr::option_t::NONE;
    } else if (vm["type"].as<std::string>() == "partial") {
      opts.input_type = sr::option_t::PARTIAL;
    } else if (vm["type"].as<std::string>() == "full") {
      opts.input_type = sr::option_t::FULL;
    } else if (vm["type"].as<std::string>() == "full-guide-feature") {
      opts.input_type = sr::option_t::FULL_WITH_GUIDANCE_FEATURE;
    } else if (vm["type"].as<std::string>() == "full-topdown-constrain") {
      opts.input_type = sr::option_t::FULL_WITH_TOPDOWN_CONSTRAIN;
    } else if (vm["type"].as<std::string>() == "full-topdown-feature") {
      opts.input_type = sr::option_t::FULL_WITH_TOPDOWN_FEATURE;
	//ljm
	//{
	} else if (vm["type"].as<std::string>() == "ngram") {
	  opts.input_type = sr::option_t::NGRAM;
	//}
    } else {
      _ERROR << "parseopt: Unknown type [" << vm["type"].as<std::string>() << "]";
      return false;
    }
  }

  if (!vm.count("model")) {
    _ERROR << "parseopt: Model path must be specified.";
//    return false;
  } else { opts.model_path = vm["model"].as<std::string>(); }
  //ljm
  //{
  if (!vm.count("ngram")) {
    opts.ngram_path = "";
  } else { opts.ngram_path = vm["ngram"].as<std::string>(); }
  //}
  if (!vm.count("input")) { opts.input_path = ""; }
  else { opts.input_path = vm["input"].as<std::string>(); }

  if (!vm.count("reference")) {
    if (learn) {
      _ERROR << "parseopt: Reference file should be specified in learning pharse";
      return false;
    } else { opts.reference_path = ""; }
  }
  else { opts.reference_path = vm["reference"].as<std::string>(); }

  if (!vm.count("output")) { opts.output_path = ""; }
  else { opts.output_path = vm["output"].as<std::string>(); }

  if (vm.count("labeled")) { opts.output_label = true; }
  else { opts.output_label = false; }

  // [Parse beam size
  opts.beam_size = 64;
  if (vm.count("beam")) { opts.beam_size = vm["beam"].as<int>(); }

  if (opts.input_type == sr::option_t::FULL_WITH_TOPDOWN_CONSTRAIN
      || opts.input_type == sr::option_t::FULL_WITH_TOPDOWN_FEATURE
      || opts.input_type == sr::option_t::FULL_WITH_GUIDANCE_FEATURE) {
    // This two conditions do not load path.
  } else {
    // If the input type is none, perform none reference generation. At the condition,
    // the postags dict is loaded to speed up the training.
    if (vm.count("posdict")) {
      opts.postag_dict_path = vm["posdict"].as<std::string>();
    } else {
      _WARN << "parseopt: postag dict is specified.";
    }
  }

  opts.display_interval = 1000;
  if (vm.count("display")) { opts.display_interval = vm["display"].as<int>(); }

  return true;
}


