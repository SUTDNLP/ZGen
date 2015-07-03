#include "shiftreduce/types/action.h"
#include "shiftreduce/ngram/ngram.h"
#include "shiftreduce/ngram/lm/ngram_query.hh"
#include "math.h"
namespace ZGen {
namespace ShiftReduce {

#define _u(name) [](const ctx_t& ctx, const action_t& act, \
    std::vector<us_t>& cache) -> void{ \
  if (ctx.name) { \
    cache.push_back( us_t(ctx.name) ); \
  } \
}

#define _b(name1, name2) [](const ctx_t& ctx, const action_t& act, \
    std::vector<bs_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2) { \
    cache.push_back( bs_t(ctx.name1, ctx.name2) ); \
  } \
}

#define _t(name1, name2, name3) [](const ctx_t& ctx, const action_t& act, \
    std::vector<ts_t>& cache) -> void{ \
  if (ctx.name1 && ctx.name2 && ctx.name3) { \
    cache.push_back( ts_t(ctx.name1, ctx.name2, ctx.name3) ); \
  } \
}

#define add_unigram_feature(label_related, name) do { \
  if (!label_related || opts.output_label) { \
    uscore_repo.push_back( ScoreMap<us_t>( _u(name) ) ); \
  } \
} while (0);

#define add_bigram_feature(label_related, name1, name2) do { \
  if (!label_related || opts.output_label) { \
    bscore_repo.push_back( ScoreMap<bs_t>( _b(name1, name2) ) ); \
  } \
} while (0);

#define add_trigram_feature(label_related, name1, name2, name3) do { \
  if (!label_related || opts.output_label) { \
    tscore_repo.push_back( ScoreMap<ts_t>( _t(name1, name2, name3) ) ); \
  } \
} while (0);

Ngram::Ngram(const std::string& filename) {
	load(filename);
}

int
Ngram::score(const StateItem& state, const Action& act,
			lm::ngram::ProbingModel::State& out, const Engine::TokenAlphabet& forms_alphat) {
  floatval_t result;
  lm::ngram::ProbingModel::State prev = state.ngstate;
  if (act.name() != Action::kShift) {
  	return -1;
  }
  int index = act.index;
  const dependency_t *ref = state.ref;

  int begin_index = ref->phrases[index].first;
  int end_index = ref->phrases[index].second;
  bool first = true;
  for (int i = begin_index; i < end_index; i ++) {
	  word_t word = ref->words[i];
	  lm::WordIndex vocab = model->GetVocabulary().Index(forms_alphat.decode(word));
	  lm::FullScoreReturn ret;
	  ret = model->FullScore(prev, vocab, out);
	  prev = out;
	  if (first) {
	      result = ret.prob;
		  first = false;
	  }
  }
  return bin(result);
}

int
Ngram::bin(floatval_t result) { 
   if (result >= 0) return 101;
   else{
	  if (result <= -12.5) return 102;
	  //else if (result >= -2.5) return 103;
	  else {
		//return (int)((result+2.5)/0.5);
		return (int)(result/0.625);
		//return (int)((pow(10,result)-pow(10,-12.5)) / ((1 - pow(10,-12.5))/20));
	  }
   }
}

bool
Ngram::load(const std::string& filename) {
  lm::ngram::Config config;
  const char* file = filename.c_str();
  model = new lm::ngram::ProbingModel(file, config);
  return true;
}

lm::ngram::ProbingModel::State
Ngram::BeginSentenceState() {
  return model->BeginSentenceState();
}

} //  end for namespace ShiftReduce
} //  end for namespace ZGen
