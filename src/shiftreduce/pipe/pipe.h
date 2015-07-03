#ifndef __ZGEN_SHIFTREDUCE_PIPE_PIPE_H__
#define __ZGEN_SHIFTREDUCE_PIPE_PIPE_H__

#include "settings.h"
#include "engine/token_alphabet.h"
#include "types/instance.h"
#include "knowledge/postag_constraint.h"
#include "shiftreduce/option.h"
#include "shiftreduce/types/action.h"
#include "shiftreduce/types/state.h"
#include "shiftreduce/types/internal_types.h"
#include "shiftreduce/model/model.h"
#include "shiftreduce/decode/decode_context.h"
#include "shiftreduce/decode/decoder.h"
#include "shiftreduce/learning/learner.h"

namespace ZGen {

namespace ShiftReduce {

// The basic pipe class
class Pipe {
public:
  /**/
  Pipe(const option_t& opts);

  /**/
  virtual ~Pipe();

  /**
   * Perform realization
   *
   *  @return bool  If successfully perform realize, return true; otherwise return
   *                false.
   */
  bool realize();
private:

  /**
   * Save model according to the options.
   *
   *  @return bool  If successfully save model, return true; otherwise return
   *                false.
   */
  bool save_model(const std::string& model_path);

  /**
   * Load model according to the options.
   *
   *  @return bool  If successfully load model, return true; otherwise return
   *                false.
   */
  bool load_model(const std::string& model_path);

  /**
   * Build output from the state item.
   *
   *
   */
  void build_output(const StateItem& item,
      const dependency_t& input,
      dependency_t* output);

  // Validating the data when dataset and reference set is both loaded.
  bool learn_dataset_and_referset_validation();
private:
  const option_t& opts;   /*< The options */
  int timestamp;          //! The timestamp.
  DecodeContext* ctx;     //! The decode context, shared by the decoder and the learner
  Decoder* decoder;       //! The decoder
  Learner* learner;       //! The pointer to the learner;
  Model* model;           //! The pointer to the model, which is shared by the scorer,
                          //! learner
  Knowledge::PostagConstraint* constraint;  //! The postag constrain.
  Engine::TokenAlphabet forms_alphabet;   //! The forms alphabet.
  Engine::TokenAlphabet postags_alphabet; //! The postags alphabet.
  Engine::TokenAlphabet deprels_alphabet; //! The dependency relation alphabet.
private:
  std::vector<dependency_t> dataset;
  std::vector<dependency_t> referset;
};

}

}
#endif  //  end for __ZGEN_SHIFTREDUCE_PIPE_PIPE_H__
