#include <iostream>
#include <fstream>
#include <string>
#include <boost/program_options.hpp>
#include "config.h"
#include "time.h"
#include "utils/logging.h"
#include "shiftreduce/option.h"
#include "shiftreduce/pipe/pipe.h"
#include "shiftreduce/frontend/opt.h"

#define APP_NAME      "ZGen_SR"
#define AUTHOR_NAME   "Yijia Liu"
#define AUTHOR_EMAIL  "oneplus.lau@gmail.com"

namespace po = boost::program_options;
namespace sr = ZGen::ShiftReduce;

int test(int argc, char* argv[]) {
  std::string usage = "ZGen v" ZGEN_VERSION " - Transition-based linearizer.\n\n";
  usage += "Usage: " APP_NAME " test [options]\n\n";
  usage += "Options";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model", po::value<std::string>(), "The path to the model.")
	("ngram", po::value<std::string>(), "The path to the ngram language model.")
    ("type", po::value<std::string>(), "The type of constrain, valid options includes:\n"
                        " - none: No input constrain/Postag constrain\n"
                        " - partial: Partial tree constrain\n"
                        " - full: Full tree constrain.")
    ("input", po::value<std::string>(), "The path to the input file.")
    ("output", po::value<std::string>(), "The path to the output file.")
    ("posdict", po::value<std::string>(), "The path to the postag dict [optional]\n"
                        "The postag dict is optional but it's highly "
                        "recommanded inorder to shrink the search space.")
    ("labeled", "Specify to output labeled linearization [default=false].")
    ("display", po::value<int>(), "The display interval.")
    ("beam", po::value<int>(), "The size for beam.")
    ("verbose", "Logging every detail.")
	("mode",po::value<int>(), "mode used for trying different configurations")
	("lookahead", "Specify to enable lookahead features [default = false]")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  sr::option_t opts;
  if (!parse_options(false, vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  _INFO << "";
  _INFO << "ZGen::shift-reduce linearizer << TEST >> " << ZGEN_VERSION;
  _INFO << "----------------------------------------------";

  time_t begin,end;
  double cost;
  begin = clock();
  sr::Pipe pipe(opts);
  end = clock();
  cost = (double)(end - begin) / CLOCKS_PER_SEC;
  _INFO << "loading cost "<<cost<<" seconds";	  

  begin = clock();
  pipe.realize();
  end = clock();
  cost = (double)(end - begin) / CLOCKS_PER_SEC;
  _INFO << "parsing cost "<<cost<<" seconds";
	  

  return 0;
}

int learn(int argc, char* argv[]) {
  std::string usage = "ZGen v" ZGEN_VERSION " - Transition-based linearizer.\n\n";
  usage += "Usage: " APP_NAME " learn [options]\n\n";
  usage += "Options";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model", po::value<std::string>(), "The path to the model.")
    ("ngram", po::value<std::string>(), "The path to the ngram language model.")
	("type", po::value<std::string>(), "The type of constrain, valid options includes:\n"
                        " - none: No input constrain/Postag constrain\n"
                        " - partial: Partial tree constrain\n"
                        " - full: Full tree constrain.")
    ("reference", po::value<std::string>(), "The path to the reference file.")
    ("input", po::value<std::string>(), "The path to the input file.")
    ("posdict", po::value<std::string>(), "The path to the postag dict [optional]\n"
                        "The postag dict is optional but it's highly "
                        "recommanded inorder to shrink the search space.")
    ("display", po::value<int>(), "The display interval [default=1000].")
    ("beam", po::value<int>(), "The size for beam [default=64].")
    ("labeled", "Specify to output labeled linearization [default=false].")
    ("verbose", "Logging every detail.")
	("mode",po::value<int>(), "mode used for trying different configurations")
	("lookahead", "Specify to enable lookahead features [default = false]")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  sr::option_t opts;
  if (!parse_options(true, vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  _INFO << "";
  _INFO << "ZGen::shift-reduce linearizer << LEARN >> " << ZGEN_VERSION;
  _INFO << "-----------------------------------------------";

  time_t begin,end;
  double cost;
  begin = clock();
  sr::Pipe pipe(opts);
  end = clock();
  cost = (double)(end - begin) / CLOCKS_PER_SEC;
  _INFO << "loading cost "<<cost<<" seconds";
	  
  begin = clock();
  pipe.realize();
  end = clock();
  cost = (double)(end - begin) / CLOCKS_PER_SEC;
  _INFO << "parsing cost "<<cost<<" seconds";
	  

  return 0;
}

int main(int argc, char * argv[]) {
  std::string usage = "ZGen v";
  usage += ZGEN_VERSION;
  usage += " - Transition-based linearizer\n\n";
  usage += " " APP_NAME " is the transition-based linearizer in ZGen project, which takes a\n"
           " bag of words along with some optional syntactic constraints and outputs a\n"
           " fluent and grammatical sentence.\n"
           " For the details of the algorithm, please refer our NAACL2015 paper:\n"
           " > Transition-based Syntactic Linearization.\n\n"
           " Bug report, please contact: " AUTHOR_NAME " <" AUTHOR_EMAIL ">\n\n";
  usage += "Usage: " APP_NAME " [learn|test] [options]\n";

  bool train_mode = false;
  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
  } else if (strcmp(argv[1], "learn") == 0) {
    learn(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "test") == 0) {
    test(argc- 1, argv+ 1);
  } else {
    std::cerr << "Unknown mode: " << argv[1] << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }

  return 0;
}
