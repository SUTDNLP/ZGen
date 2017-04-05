#include "settings.h"
#include "utils/logging.h"
#include "shiftreduce/decode/decoder.h"
#include "shiftreduce/types/action_utils.h"
#include <boost/assert.hpp>

namespace ZGen {
namespace ShiftReduce {

extern int sentence_no;
extern int const_sentence_no;
action_sequence_t decoder_gold_actions;
int step_no;
namespace eg = ZGen::Engine;
namespace kn = ZGen::Knowledge;

static bool
TransitionHeapMore(const scored_transition_t& x, const scored_transition_t& y) {
  return x.get<2>() > y.get<2>();
}

Decoder::Decoder(const option_t& _opts,
    DecodeContext* _ctx,
    Model* _model,
    const kn::PostagConstraint* _constraint,
    const Engine::TokenAlphabet& _forms_alphabet,
	int _tag, int _nr_relations)
  : opts(_opts),
  ctx(_ctx),
  model(_model),
  constraint(_constraint),
  forms_alphabet(_forms_alphabet),
  NP(_tag), nr_relations(_nr_relations) {}

Decoder::~Decoder() {}

void
Decoder::get_possible_shift_actions(const StateItem& item,
    int j, word_t word, postag_t tag) {
  const dependency_t* ref = item.ref;

  if (tag == eg::TokenAlphabet::NONE) {
    if (ref->is_phrases[j]) {
      // NP tag should be passed into the decoder!
      possible_actions.push_back(ActionFactory::make_shift(NP, word, j, PrefixType::kNone));
    } else {
      std::vector<postag_t> possible_tags;
      constraint->get_possible_tags(ref, j, word, possible_tags);
      for (const postag_t& t: possible_tags) {
        std::vector<word_t> lexemes =  item.ref->lexemes[j];
		std::vector<PrefixType> prefix_types = item.ref->prefix_types[j];
		int loop_index = 0;
		for(word_t lexeme: lexemes){
		  possible_actions.push_back(ActionFactory::make_shift(t, lexeme, j, prefix_types[loop_index]));
		  loop_index++;
		}
      }
    }
  } else {
	  std::vector<word_t> lexemes =  item.ref->lexemes[j];
	  std::vector<PrefixType> prefix_types = item.ref->prefix_types[j];
	  int loop_index = 0;
	  for(word_t lexeme: lexemes){
		  possible_actions.push_back(ActionFactory::make_shift(tag, lexeme, j, prefix_types[loop_index]));
		  loop_index++;
	  }
  }
}

void
Decoder::score_possible_actions(const StateItem& from) {
  packed_score.clear();
  ScoreContext ctx(from);
  for (const Action& act: possible_actions) {
	ctx.load_generic_actions(from,act);
	lm::ngram::ProbingModel::State out;
    packed_score[act] = model->score(ctx, from, act, !opts.learn, out, forms_alphabet);
	if (act.name() == Action::kShift || act.name() == Action::kInsert || act.name() == Action::kSplitArc) {
		packed_ngstate[act] = out;
	}
	else {
		packed_ngstate[act] = from.ngstate;
	}
  }
}

void
Decoder::clear_candidate_transition() {
  for (int i = 0; i < opts.beam_size; ++ i) {
    candidate_transitions[i].get<0>() = NULL;
    candidate_transitions[i].get<2>() = -inf;
  }
}

int
Decoder::extend_candidate_transition(const scored_transition_t & trans,
    const int current_beam_size) {
  if (current_beam_size == opts.beam_size) {
    if (trans.get<2>() > candidate_transitions[0].get<2>()) {
      std::pop_heap(candidate_transitions,
          candidate_transitions+ opts.beam_size,
          TransitionHeapMore);
      candidate_transitions[opts.beam_size- 1] = trans;
      std::push_heap(candidate_transitions,
          candidate_transitions + opts.beam_size,
          TransitionHeapMore);
    }
    return 0;
  }
  candidate_transitions[current_beam_size] = trans;
  std::push_heap(candidate_transitions,
      candidate_transitions + current_beam_size + 1,
      TransitionHeapMore);
  return 1;
}

void
Decoder::transit(const StateItem& from, const action_t& act, floatval_t score,
    StateItem* to) {
  to->ngstate = packed_ngstate[act];
  if (ActionUtils::is_shift(act)) {
    from.shift(act.tag(), act.word, act.index, act.prefix_type, to);
    to->score = score;
  } else if (ActionUtils::is_left_arc(act)) {
    from.left_arc(act.tag(), to);
    to->score = score;
  } else if (ActionUtils::is_right_arc(act)) {
    from.right_arc(act.tag(), to);
    to->score = score;
  } else if (ActionUtils::is_insert(act)){
	from.insert(act.index, to);
	to->score = score;
  } else if (ActionUtils::is_split_arc(act)){
	from.split_arc(act.tag(), act.word, to);
	to->score = score;
  } else if (ActionUtils::is_idle(act)){
	from.idle(to);
	to->score = score;
  } else{
    _WARN << "Decoder(transit): Unknown action";
  }
}

const StateItem*
Decoder::search_correct_state(const action_t& act,
    const StateItem* previous_correct_state,
    StateItem* begin,
    StateItem* end) {
  const StateItem* correct_state = NULL;
  if (!ActionUtils::is_shift(act)) {
    for (const StateItem* p = begin; p != end; ++ p) {
      if (p->previous == previous_correct_state && p->last_action == act) {
        correct_state = p; break;
      }
    }
  } else {
    for (const StateItem* p = begin; p != end; ++ p) {
      if (p->previous == previous_correct_state && p->last_action == act &&
          p->last_action.index == act.index) {
        correct_state = p; break;
      }
    }
    if (NULL== correct_state) {
      for (StateItem* p = begin; p != end; ++ p) {
        if (p->previous == previous_correct_state && p->last_action == act) {
          int shifted_index = p->last_action.index;
          int expected_shifted_index = act.index;

          if (!p->buffer.test(expected_shifted_index)) {
            _WARN << "decode: the expected shifted index should be empty";
            break;
          }
          p->buffer.set(expected_shifted_index, 0);
          p->buffer.set(shifted_index, 1);
          p->postags[expected_shifted_index] = p->postags[shifted_index];
          p->postags[shifted_index] = 0;
          p->stack.back() = expected_shifted_index;
          p->top0 = expected_shifted_index;
          p->last_action = act;
          p->words_shifted_map[expected_shifted_index] = act.word;
          correct_state = p;
          break;
        }
      }
    }
  }
  return correct_state;
}

const StateItem*
Decoder::search_best_state(const StateItem* begin, const StateItem* end) {
  const StateItem* best = begin;
  for (const StateItem* p = begin+ 1; p!= end; ++ p) {
    if (p->score > best->score) { best = p; }
  }
  return best;
}

Decoder::decode_result_t
Decoder::decode(const dependency_t* input, const dependency_t* gold,
    const action_sequence_t& gold_actions, const graph_t* graph , std::vector<int>& out_of_beam) {
  config_input(gold);

  ctx->lattice[0].clear();
  if(model->ngram != NULL)
	ctx->lattice[0].ngstate = (model->ngram)->BeginSentenceState();
  ctx->lattice[0].set_reference(input);
  ctx->lattice[0].set_graph(graph);
  ctx->lattice[0].set_forms_alphabet(&forms_alphabet);
  ctx->lattice[0].set_deprels_alphabet(deprels_alphabet);
  ctx->lattice[0].set_pos_alphabet(pos_alphabet);
  ctx->lattice[0].set_pos_constraint(constraint);
  ctx->lattice_index[0] = ctx->lattice;
  ctx->lattice_index[1] = ctx->lattice+ 1;
  decoder_gold_actions = gold_actions;
  const StateItem* correct_state = NULL;
  int step;
  int steps;
  steps = (input->size()* 4) - 2;

  if (opts.learn) {
    correct_state= ctx->lattice;
  }
  for (step = 1; step <= steps; ++ step) {
	  if(sentence_no == const_sentence_no){
    _TRACE << "Decoder(round): #" << step;
	  }
    step_no = step;
    int current_beam_size = 0;
    clear_candidate_transition();
    for (const StateItem* from = ctx->lattice_index[step- 1];
        from != ctx->lattice_index[step]; ++ from) {
      get_possible_actions((*from));
      score_possible_actions((*from));

      for (const Action& act: possible_actions) {
        current_beam_size += extend_candidate_transition(
            scored_transition_t(from, act, from->score+ packed_score[act]),
            current_beam_size);
      }
    }

    for (int i = 0; i < current_beam_size; ++ i) {
      const scored_transition_t& tran = candidate_transitions[i];
      if(sentence_no == const_sentence_no){
      _TRACE << "Decoder(transit): " << (void*)(tran.get<0>())
        << "->" << (void*)(ctx->lattice_index[step]+ i)
        << " by " << tran.get<1>() <<" word "<< this->forms_alphabet.decode(tran.get<1>().word)<< ",s=" << tran.get<2>() ;
      }
      transit((*tran.get<0>()), tran.get<1>(), tran.get<2>(),
          (ctx->lattice_index[step]+ i));
    }
    ctx->lattice_index[step+ 1] = ctx->lattice_index[step]+ current_beam_size;
    if (opts.learn) {
      const StateItem* next_correct_state= search_correct_state(
            gold_actions[step- 1], correct_state,
            ctx->lattice_index[step], ctx->lattice_index[step+ 1]);
      bool correct_state_in_beam = (next_correct_state!= NULL);
      if (!correct_state_in_beam) {
    	  out_of_beam.push_back(sentence_no);
        _TRACE << "Decoder(decode terminal): correct state fallout beam at #"
          << step- 1 << " to #" << step;
        const StateItem* best_to = search_best_state(ctx->lattice_index[step],
            ctx->lattice_index[step+ 1]);
        transit((*correct_state), gold_actions[step-1], 0, ctx->lattice_index[step+1]);
        return std::make_pair(best_to, ctx->lattice_index[step+1]);
      } else {
        correct_state = next_correct_state;
      }
    }
  }
  const StateItem* best_to = search_best_state(ctx->lattice_index[step-1],
      ctx->lattice_index[step]);
  return std::make_pair(best_to, correct_state);
}

/*void Decoder::set_forms_alphabet(Engine::TokenAlphabet forms_alphabet){
	this->forms_alphabet = forms_alphabet;
}*/
void Decoder::set_pos_alphabet(Engine::TokenAlphabet* pos_alphabet){
	this->pos_alphabet = pos_alphabet;
}
void Decoder::set_deprels_alphabet(Engine::TokenAlphabet* deprels_alphabet){
	this->deprels_alphabet = deprels_alphabet;
}
} //  end for namespace ShiftReduce
} //  end for namespace ZGen
