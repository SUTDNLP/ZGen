#ifndef __ZGEN_SHIFTREDUCE_OPTION_H__
#define __ZGEN_SHIFTREDUCE_OPTION_H__

#include <string>

namespace ZGen {
namespace ShiftReduce {

//
struct Option {
  enum {
    NONE,
    PARTIAL,
    FULL,
    FULL_WITH_GUIDANCE_FEATURE,
    FULL_WITH_TOPDOWN_FEATURE,
    FULL_WITH_TOPDOWN_CONSTRAIN,
	//ljm
	//{
	NGRAM,
	//}
  };

  int input_type;               //! The input type
  std::string model_path;       //! Path to the model.
  std::string input_path;       //! The input path.
  std::string output_path;      //! The output path
  std::string reference_path;   //! The reference path
  std::string postag_dict_path; //! AUX: the postag dict path, only actived at NONE mode.
  int display_interval;         //! [MISC
  int beam_size;                //! The size of beam
  bool learn;                   //! Specify whether perform learning.
  bool output_label;            //! Specify whether output labels.
  bool shuffle_instance;        //!
  bool verbose;                 //! Specify verbose logging.
  int mode;
  bool lookahead;

  //ljm
  //{
  std::string ngram_path;       //! Path to the ngram language model.
  //}
};

typedef Option option_t;

} //  end for namespace ShiftReduce
} //  end for namespace ZGen

#endif  //  end for __ZGEN_SHIFTREDUCE_OPTION_H__
