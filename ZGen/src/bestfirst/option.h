#ifndef __ZGEN_BEST_FIRST_OPTION_H__
#define __ZGEN_BEST_FIRST_OPTION_H__

#include <string>

namespace ZGen {

namespace BestFirst {

struct Option {
  enum {NONE, PARTIAL, FULL};

  //!
  int input_type;

  //! the path to the model.
  std::string model_path;

  //! the path to the postag dict.
  std::string postag_dict_path;

  //!  the path to the reference input.
  std::string reference_path;

  //! the path to the input file.
  std::string input_path;

  //! the path to the output file.
  std::string output_path;

  //! the timeout threshold for timeout strategy.
  double timeout;

  //! the nbest number.
  int nbest;

  //! The display interval.
  int display_interval;

  bool greedy;

  bool learn;
};

typedef Option option_t;

} //  end for namespace BestFirst

} //  end for namespace ZGen

#endif  //  end for __ZGEN_BEST_FIRST_OPTION_H__
