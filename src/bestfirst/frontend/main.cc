#include <iostream>
#include <fstream>
#include <string>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include "config.h"
#include "bestfirst/option.h"
#include "bestfirst/pipe/pipe.h"
#include "bestfirst/frontend/opt.h"

int main(int argc, char** argv) {
  namespace po = boost::program_options;
  namespace bf = ZGen::BestFirst;

  std::string usage = "ZGen v";
  usage += ZGEN_VERSION;
  usage += " - Best-first linearizer.\n\n";
  usage += "Usage: zgen-bf [learn|test] [options]\n\n";
  usage += "Options";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "show help information.")
    ("type,t", po::value<std::string>(), "the input type.")
    ("input,i", po::value<std::string>(), "the path to the input file.")
    ("output,o", po::value<std::string>(), "the path to the output file.")
    ("model,m", po::value<std::string>(), "the path to the model.")
    ("posdict,p", po::value<std::string>(), "the path to the postag dict.")
    ("reference,r", po::value<std::string>(), "the path to the reference file.")
    ("verbose,v", "output details.")
    ("timeout", po::value<float>(), "the timeout for decoding strategy")
    ("nbest", po::value<int>(), "the nbest")
    ("greedy", "use greedy search strategy")
    ;

  bool train_mode = false;
  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  } else if (!strcmp(argv[1], "learn")) {
    train_mode = true;
  } else if (!strcmp(argv[1], "test")) {
    train_mode = false;
  } else {
    std::cerr << "unknown mode: " << argv[1] << std::endl;
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc- 1, argv+ 1, optparser), vm);

  bf::option_t opts;
  if (!parse_options(!strcmp(argv[1], "learn"), vm, opts)) {
    return 1;
  }

  BOOST_LOG_TRIVIAL(info) << "";
  BOOST_LOG_TRIVIAL(info) << "ZGen::best-first component <<" << argv[1] << ">> " << ZGEN_VERSION;
  BOOST_LOG_TRIVIAL(info) << "-----------------------------------------";

  bf::Pipe pipe(opts);
  pipe.realize();
  pipe.save_model();
  return 0;
}
