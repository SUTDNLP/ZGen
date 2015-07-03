#include "shiftreduce/types/action.h"
#include "shiftreduce/model/model.h"

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

Model::Model(const option_t& opts) {
  uscore_repo.reserve(64);
  bscore_repo.reserve(80);
  tscore_repo.reserve(80);

  add_unigram_feature( false, S0w );
  add_unigram_feature( false, S0p );
  add_unigram_feature( false, S0ldw );
  add_unigram_feature( false, S0ldp );
  add_unigram_feature( true,  S0ldl  );
  add_unigram_feature( false, S0l2dw );
  add_unigram_feature( false, S0l2dp );
  add_unigram_feature( true,  S0l2dl );
  add_unigram_feature( false, S0rdw );
  add_unigram_feature( false, S0rdp );
  add_unigram_feature( true,  S0rdl  );
  add_unigram_feature( false, S0r2dw );
  add_unigram_feature( false, S0r2dp );
  add_unigram_feature( true, S0r2dl );
  add_bigram_feature( false, S0w, S0ldw );
  add_bigram_feature( false, S0w, S0ldp );
  add_bigram_feature( true,  S0w, S0ldl );
  add_bigram_feature( false, S0p, S0ldw );
  add_bigram_feature( false, S0p, S0ldp );
  add_bigram_feature( true,  S0p, S0ldl );
  add_bigram_feature( false, S0w, S0rdw );
  add_bigram_feature( false, S0w, S0rdp );
  add_bigram_feature( true,  S0w, S0rdl );
  add_bigram_feature( false, S0p, S0rdw );
  add_bigram_feature( false, S0p, S0rdp );
  add_bigram_feature( true,  S0p, S0rdl );
  add_trigram_feature( false, S0w, S0ldw, S0S0ldDist );
  add_trigram_feature( false, S0w, S0ldp, S0S0ldDist );
  add_trigram_feature( true,  S0w, S0ldl, S0S0ldDist );
  add_trigram_feature( false, S0p, S0ldw, S0S0ldDist );
  add_trigram_feature( false, S0p, S0ldp, S0S0ldDist );
  add_trigram_feature( true,  S0p, S0ldl, S0S0ldDist );
  add_trigram_feature( false, S0w, S0rdw, S0S0rdDist );
  add_trigram_feature( false, S0w, S0rdp, S0S0rdDist );
  add_trigram_feature( true,  S0w, S0rdl, S0S0rdDist );
  add_trigram_feature( false, S0p, S0rdw, S0S0rdDist );
  add_trigram_feature( false, S0p, S0rdp, S0S0rdDist );
  add_trigram_feature( true,  S0p, S0rdl, S0S0rdDist );
  add_trigram_feature( false, S0w, S0p, S0ldw );
  add_trigram_feature( false, S0w, S0p, S0ldp );
  add_trigram_feature( false, S0w, S0ldw, S0ldp );
  add_trigram_feature( false, S0p, S0ldw, S0ldp );
  add_trigram_feature( false, S0w, S0p, S0rdw );
  add_trigram_feature( false, S0w, S0p, S0rdp );
  add_trigram_feature( false, S0w, S0rdw, S0rdp );
  add_trigram_feature( false, S0p, S0rdw, S0rdp );
  add_bigram_feature( false, S0w, S0la );
  add_bigram_feature( false, S0w, S0ra );
  add_bigram_feature( false, S0p, S0la );
  add_bigram_feature( false, S0p, S0ra );
  add_bigram_feature( false, S0w, S0ls );
  add_bigram_feature( false, S0w, S0rs );
  add_bigram_feature( false, S0p, S0ls );
  add_bigram_feature( false, S0p, S0rs );
  add_bigram_feature( false, S0ldw, S0l2dw );
  add_bigram_feature( false, S0ldp, S0l2dp );
  add_bigram_feature( true, S0ldl, S0l2dl );
  add_unigram_feature( false, S1w );
  add_unigram_feature( false, S1p );
  add_unigram_feature( false, S1ldw );
  add_unigram_feature( false, S1ldp );
  add_unigram_feature( true,  S1ldl  );
  add_unigram_feature( false, S1l2dw );
  add_unigram_feature( false, S1l2dp );
  add_unigram_feature( true,  S1l2dl );
  add_unigram_feature( false, S1rdw );
  add_unigram_feature( false, S1rdp );
  add_unigram_feature( true,  S1rdl  );
  add_unigram_feature( false, S1r2dw );
  add_unigram_feature( false, S1r2dp );
  add_unigram_feature( true, S1r2dl );
  add_bigram_feature( false, S1w, S1ldw );
  add_bigram_feature( false, S1w, S1ldp );
  add_bigram_feature( true,  S1w, S1ldl );
  add_bigram_feature( false, S1p, S1ldw );
  add_bigram_feature( false, S1p, S1ldp );
  add_bigram_feature( true,  S1p, S1ldl );
  add_bigram_feature( false, S1w, S1rdw );
  add_bigram_feature( false, S1w, S1rdp );
  add_bigram_feature( true,  S1w, S1rdl );
  add_bigram_feature( false, S1p, S1rdw );
  add_bigram_feature( false, S1p, S1rdp );
  add_bigram_feature( true,  S1p, S1rdl );
  add_trigram_feature( false, S1w, S1ldw, S1S1ldDist );
  add_trigram_feature( false, S1w, S1ldp, S1S1ldDist );
  add_trigram_feature( true,  S1w, S1ldl, S1S1ldDist );
  add_trigram_feature( false, S1p, S1ldw, S1S1ldDist );
  add_trigram_feature( false, S1p, S1ldp, S1S1ldDist );
  add_trigram_feature( true,  S1p, S1ldl, S1S1ldDist );
  add_trigram_feature( false, S1w, S1rdw, S1S1rdDist );
  add_trigram_feature( false, S1w, S1rdp, S1S1rdDist );
  add_trigram_feature( true,  S1w, S1rdl, S1S1rdDist );
  add_trigram_feature( false, S1p, S1rdw, S1S1rdDist );
  add_trigram_feature( false, S1p, S1rdp, S1S1rdDist );
  add_trigram_feature( true,  S1p, S1rdl, S1S1rdDist );
  add_trigram_feature( false, S1w, S1p, S1ldw );
  add_trigram_feature( false, S1w, S1p, S1ldp );
  add_trigram_feature( false, S1w, S1ldw, S1ldp );
  add_trigram_feature( false, S1p, S1ldw, S1ldp );
  add_trigram_feature( false, S1w, S1p, S1rdw );
  add_trigram_feature( false, S1w, S1p, S1rdp );
  add_trigram_feature( false, S1w, S1rdw, S1rdp );
  add_trigram_feature( false, S1p, S1rdw, S1rdp );
  add_bigram_feature( false, S1w, S1la );
  add_bigram_feature( false, S1w, S1ra );
  add_bigram_feature( false, S1p, S1la );
  add_bigram_feature( false, S1p, S1ra );
  add_bigram_feature( false, S1w, S1ls );
  add_bigram_feature( false, S1w, S1rs );
  add_bigram_feature( false, S1p, S1ls );
  add_bigram_feature( false, S1p, S1rs );
  add_bigram_feature( false, S1r2dw, S1rdw );
  add_bigram_feature( false, S1r2dp, S1rdp );
  add_bigram_feature( true, S1r2dl, S1rdl );
  add_bigram_feature( false, S0w, S1w );
  add_bigram_feature( false, S0w, S1p );
  add_bigram_feature( false, S0p, S1w );
  add_bigram_feature( false, S0p, S1p );
  add_trigram_feature( false, S0w, S0p, S1w );
  add_trigram_feature( false, S0w, S0p, S1p );
  add_trigram_feature( false, S0w, S1w, S1p );
  add_trigram_feature( false, S0p, S1w, S1p );
  add_trigram_feature( false, S0w, S1w, S0S1Dist );
  add_trigram_feature( false, S0w, S1p, S0S1Dist );
  add_trigram_feature( false, S0p, S1w, S0S1Dist );
  add_trigram_feature( false, S0p, S1p, S0S1Dist );
  add_unigram_feature( false, W0 );
  add_unigram_feature( false, P0 );
  add_bigram_feature( false, W0, W1 );
  add_bigram_feature( false, P0, P1 );
  add_trigram_feature( false, W0, W1, W2 );
  add_trigram_feature( false, P0, P1, P2 );
}

floatval_t
Model::score(const StateItem& state, const Action& act, bool avg) {
  ScoreContext ctx(state);
  return score(ctx, act, avg);
}

floatval_t
Model::score(const ScoreContext& ctx, const Action& act, bool avg) {
  floatval_t ret = 0;
  for (ScoreMap<UnigramMetaFeature>& e: uscore_repo) { ret += e.score(ctx, act, avg); }
  for (ScoreMap<BigramMetaFeature>&  e: bscore_repo) { ret += e.score(ctx, act, avg); }
  for (ScoreMap<TrigramMetaFeature>& e: tscore_repo) { ret += e.score(ctx, act, avg); }
  return ret;
}

floatval_t
Model::score(const StateItem& state, const Action& act, bool avg, lm::ngram::ProbingModel::State &out, const Engine::TokenAlphabet& forms_alphabet) {
  ScoreContext ctx(state);
  if(ngram == NULL) return score(ctx, act, avg);

  int tmp = ngram->score(state, act, out, forms_alphabet);
  if(tmp == -1) { // without any LM feature
	ctx.LM1 = 0;
	ctx.LM2 = 0;
	ctx.LM3 = 0;
	ctx.LM4 = 0;
  }
  else if(tmp == 101 || tmp == 102 || tmp == 103) { // special points
  	ctx.LM1 = tmp;
	ctx.LM2 = tmp;
	ctx.LM3 = tmp;
	ctx.LM4 = tmp;
  }
  else { // change from 0-19 to 1-20, and then bin them
  	ctx.LM1 = (tmp / 1) + 1;
	ctx.LM2 = (tmp / 2) + 1;
	ctx.LM3 = (tmp / 4) + 1;
	ctx.LM4 = (tmp / 10) + 1;
  }
  return score(ctx, act, avg);
  
}

void
Model::update(const StateItem& state, const Action& act, int timestamp,
    const floatval_t& scale) {
  ScoreContext ctx(state);
  update(ctx, act, timestamp, scale);
}

void
Model::update(const ScoreContext& ctx, const Action& act, int timestamp,
    const floatval_t& scale) {
  for (ScoreMap<UnigramMetaFeature>& e: uscore_repo) { e.update(ctx, act, timestamp, scale); }
  for (ScoreMap<BigramMetaFeature>&  e: bscore_repo) { e.update(ctx, act, timestamp, scale); }
  for (ScoreMap<TrigramMetaFeature>& e: tscore_repo) { e.update(ctx, act, timestamp, scale); }
}

void
Model::update(const StateItem& state, const Action& act, int timestamp,
	const floatval_t& scale,  const Engine::TokenAlphabet& forms_alphabet) {
  ScoreContext ctx(state);
  if(ngram == NULL) {
	update(ctx, act, timestamp, scale);
	return;
  }

  lm::ngram::ProbingModel::State out;
  int tmp = ngram->score(state, act, out, forms_alphabet);
  if(tmp == -1) {// without any LM feature
  	ctx.LM1 = 0;
	ctx.LM2 = 0;
	ctx.LM3 = 0;
	ctx.LM4 = 0;
  }
  else if(tmp == 101 || tmp == 102 || tmp == 103) {// special point
	ctx.LM1 = tmp;
	ctx.LM2 = tmp;
	ctx.LM3 = tmp;
	ctx.LM4 = tmp;
  }
  else { // change from 0-19 to 1-20, and then bin them
	ctx.LM1 = (tmp / 1) + 1;
	ctx.LM2 = (tmp / 2) + 1;
	ctx.LM3 = (tmp / 4) + 1;
	ctx.LM4 = (tmp / 10) + 1;
  }
  update(ctx, act, timestamp, scale);
}

void
Model::flush(int timestamp) {
  for (ScoreMap<UnigramMetaFeature>& e: uscore_repo) { e.flush(timestamp); }
  for (ScoreMap<BigramMetaFeature>&  e: bscore_repo) { e.flush(timestamp); }
  for (ScoreMap<TrigramMetaFeature>& e: tscore_repo) { e.flush(timestamp); }
}

bool
Model::save(std::ostream& os) {
  boost::archive::text_oarchive oa(os);
  for (ScoreMap<UnigramMetaFeature>& e: uscore_repo) { e.save(oa); }
  for (ScoreMap<BigramMetaFeature>&  e: bscore_repo) { e.save(oa); }
  for (ScoreMap<TrigramMetaFeature>& e: tscore_repo) { e.save(oa); }
  return true;
}

bool
Model::load(std::istream& is) {
  boost::archive::text_iarchive ia(is);
  for (ScoreMap<UnigramMetaFeature>& e: uscore_repo) { e.load(ia); }
  for (ScoreMap<BigramMetaFeature>&  e: bscore_repo) { e.load(ia); }
  for (ScoreMap<TrigramMetaFeature>& e: tscore_repo) { e.load(ia); }
  return true;
}
bool
Model::load(const option_t& opts) {
  if (opts.ngram_path != "") {
	_INFO << "Pipe(report): loading ngram language model " << opts.ngram_path;
	ngram = new Ngram(opts.ngram_path);

	if (ngram != NULL) {
		_INFO << "Pipe(report): loading ngram language model sucess.";
		add_unigram_feature(false, LM1);//level1 -- 20 bins
		add_unigram_feature(false, LM2);//level2 -- 10 bins
		add_unigram_feature(false, LM3);//level3 -- 5 bins
		add_unigram_feature(false, LM4);//level4 -- 2 bins
		return true;
	}
	else {
		_WARN << "Pipe: loading errors in" << opts.ngram_path;
		return false;
	}
  }
  else {
    return false;
  }
}
} //  end for namespace ShiftReduce
} //  end for namespace ZGen
