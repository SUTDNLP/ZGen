#include <iostream>
#include <fstream>
#include <cstring>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include "utils/ioutils.h"
#include "utils/logging.h"
#include <map>
#include <unordered_map>
//#include <regex>

namespace ZGen {

namespace IO {

namespace algo = boost::algorithm;
namespace eg = ZGen::Engine;

std::istream*
get_istream(const char* filename,
    std::istream* default_stream) {
  std::istream* is = NULL;
  if (filename!= NULL && strlen(filename) > 0) {
    is = new std::ifstream(filename);

    if (!is->good()) {
      _WARN << "ioutils: Opening file failure, use default stream";
      is = default_stream;
    } else {
      _INFO << "ioutils: Opening file stream from "<< filename << ".";
    }
  } else {
    _WARN << "ioutils: Input file not specified, use default stream";
    is = default_stream;
  }

  return is;
}

std::istream*
get_istream(const std::string& filename, std::istream* default_stream) {
  return get_istream(filename.c_str(), default_stream);
}

std::ostream*
get_ostream(const char* filename, std::ostream* default_stream) {
  std::ostream* os = NULL;
  if (filename!= NULL && strlen(filename) > 0) {
    os = new std::ofstream(filename);

    if (!os->good()) {
      _WARN << "ioutils: Opening file failure, use default stream";
      os = default_stream;
    } else {
      _WARN << "ioutils: File stream from: "<< filename << " is opened.";
    }
  } else {
    _WARN << "ioutils: Output file not specified, use default stream";
    os = default_stream;
  }
  return os;
}

std::ostream*
get_ostream(const std::string& filename, std::ostream* default_stream) {
  return get_ostream(filename.c_str(), default_stream);
}

int read_graph(std::istream& is, graph_t& graph,eg::TokenAlphabet& deprels_alphabet){
	std::string line;
	while(std::getline(is,line)){
		algo::trim(line);
		if(line.size()==0){
			break;
		}
		std::vector<std::string> items;
		algo::split(items, line, boost::is_any_of("\t "),boost::token_compress_on);
		int node1 = atoi(items[0].c_str());
		int node2 = atoi(items[1].c_str());
		std::string label = items[2];
		int deprel_index = deprels_alphabet.insert(label);
		graph.add_to_parent_map(node1,node2,deprel_index);
	}
	return graph.parent_map.size();
}

int read_graphs(std::istream& is, std::vector<graph_t> &data, eg::TokenAlphabet& deprels_alphabet){
	data.clear();
	std::string data_context((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
	boost::regex INSTANCE_DELIMITER("\n\n");
	boost::sregex_token_iterator instance(data_context.begin(),data_context.end(), INSTANCE_DELIMITER, -1);
	boost::sregex_token_iterator eos;

	int num_instance = 0;
	//loop over instances;
	while(instance != eos){
		std::istringstream iss(*instance);
		graph_t graph;
		read_graph(iss, graph,deprels_alphabet);
		graph.add_ancestors();
		graph.bfs();
		data.push_back(graph);
//		std::cout<< "number of nodes "<<graph.parent_map.size() <<std::endl;
		instance++;
		num_instance++;
	}
	_INFO << num_instance << " instances " << data.size() <<" graph size ";
	return 0;
}

void set_mapping_extra(std::map<std::string, Participle>* participle_map, std::map<std::string, Tense> * tense_map, std::map<std::string, Number> * number_map, std::map<std::string, Quoted> * quoted_map, std::map<std::string, Bracket> * bracket_map, std::map<std::string, QuoteBracket> * quote_bracket_map){
	participle_map->insert(std::make_pair("none", Participle::kNone));
	participle_map->insert(std::make_pair("pres", Participle::kPresent));
	participle_map->insert(std::make_pair("past", Participle::kPast));

	tense_map->insert(std::make_pair("none",Tense::kNone));
	tense_map->insert(std::make_pair("pres",Tense::kPresent));
	tense_map->insert(std::make_pair("past",Tense::kPast));

	number_map->insert(std::make_pair("none",Number::kNone));
	number_map->insert(std::make_pair("sg",Number::kSingular));
	number_map->insert(std::make_pair("pl",Number::kPlural));

	quoted_map->insert(std::make_pair("none",Quoted::kNone));
	quoted_map->insert(std::make_pair("s",Quoted::kSingle));
	quoted_map->insert(std::make_pair("d",Quoted::kDouble));
	quoted_map->insert(std::make_pair("ds",Quoted::kDoubleSingle));
	quoted_map->insert(std::make_pair("dd",Quoted::kDoubleDouble));
	quoted_map->insert(std::make_pair("ddd",Quoted::kDoubleDoubleDouble));

	bracket_map->insert(std::make_pair("none",Bracket::kNone));
	bracket_map->insert(std::make_pair("r",Bracket::kRound));
	bracket_map->insert(std::make_pair("c",Bracket::kCurly));
	bracket_map->insert(std::make_pair("rr",Bracket::kRoundRound));

	quote_bracket_map->insert(std::make_pair("`", QuoteBracket::kSingleQuoteBegin));
	quote_bracket_map->insert(std::make_pair("'", QuoteBracket::kSingleQuoteEnd));
	quote_bracket_map->insert(std::make_pair("``", QuoteBracket::kDoubleQuoteBegin));
	quote_bracket_map->insert(std::make_pair("''", QuoteBracket::kDoubleQuoteEnd));
	quote_bracket_map->insert(std::make_pair("(", QuoteBracket::kRoundBracketBegin));
	quote_bracket_map->insert(std::make_pair(")", QuoteBracket::kRoundBracketEnd));
	quote_bracket_map->insert(std::make_pair("{", QuoteBracket::kCurlyBracketBegin));
	quote_bracket_map->insert(std::make_pair("}", QuoteBracket::kCurlyBracketEnd));
}

void read_previous_words(std::istream& is, Engine::TokenAlphabet& forms_alphabet, std::set<int> & gold_previous_words){
	std::string line;
	int line_no = 0;
	while (std::getline(is, line)) {
		algo::trim(line);
		if (line.size() == 0) { break; }
		gold_previous_words.insert(forms_alphabet.insert(line));
	}
}

void
read_mapping(std::istream& is, std::vector<dependency_t>& data){
	std::string data_context((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

	boost::regex INSTANCE_DELIMITER("\n\n\n");
	boost::sregex_token_iterator instance(data_context.begin(), data_context.end(), INSTANCE_DELIMITER, -1);
	boost::sregex_token_iterator eos;
	int instanceCount = 0;
	while (instance != eos) {
	    std::istringstream iss(*instance);
	    std::map<int,int> input_gold_node_mapping;
	    std::map<int,int> gold_input_node_mapping;
	    std::vector<int> function_words;
	    read_mapping_instance(iss, input_gold_node_mapping, function_words, gold_input_node_mapping);
	    data[instanceCount].input_gold_node_mapping = input_gold_node_mapping;
	    data[instanceCount].function_words = function_words;
	    data[instanceCount].gold_input_node_mapping = gold_input_node_mapping;
	    /*for(auto elem: data[instanceCount].input_gold_node_mapping){
			_INFO<<elem.first<<" mapped to "<<elem.second;
		}
	    for(auto elem: function_words){
	    	_INFO<<"function word "<<elem;
		}*/
	    instanceCount++;
	    instance++;
	}
}

void read_mapping_instance(std::istream& is, std::map<int,int>& input_gold_node_mapping, std::vector<int>& function_words, std::map<int,int>& gold_input_node_mapping){
	std::string line;
	int line_no = 0;
	while (std::getline(is, line)) {
		algo::trim(line);
		if (line.size() == 0) { break; }
		std::vector<std::string> items;
		algo::split(items, line, boost::is_any_of("\t "),
				boost::token_compress_on);

		input_gold_node_mapping[atoi(items[0].c_str())] = atoi(items[1].c_str());
		gold_input_node_mapping[atoi(items[1].c_str())] = atoi(items[0].c_str());
	}

	while (std::getline(is, line)) {
		algo::trim(line);
		if (line.size() == 0) { break; }
		if(atoi(line.c_str()) == -1){
			break;
		}
		function_words.push_back(atoi(line.c_str()));
//		_INFO<<"line.c_str() "<<line.c_str();
	}
}

int
read_dependency_dataset(std::istream& is,
    eg::TokenAlphabet& forms_alphabet,
    eg::TokenAlphabet& postags_alphabet,
    eg::TokenAlphabet& deprels_alphabet,
    std::vector<dependency_t>& data) {
  data.clear();
  std::string data_context((std::istreambuf_iterator<char>(is)),
      std::istreambuf_iterator<char>());

  boost::regex INSTANCE_DELIMITER("\n\n");
  boost::sregex_token_iterator instance(data_context.begin(),
      data_context.end(), INSTANCE_DELIMITER, -1);
  boost::sregex_token_iterator eos;
  std::map<std::string, Participle> participle_map;
  std::map<std::string, Tense> tense_map;
  std::map<std::string, Number> number_map;
  std::map<std::string, Quoted> quoted_map;
  std::map<std::string, Bracket> bracket_map;
  std::map<std::string, QuoteBracket> quote_bracket_map;
  set_mapping_extra(&participle_map, &tense_map, &number_map, &quoted_map, &bracket_map, &quote_bracket_map);
  // Loop over the instances
  while (instance != eos) {
    std::istringstream iss(*instance);
    dependency_t parse;
	graph_t graph;
	read_dependency_instance(iss, forms_alphabet,
//<<<<<<< HEAD
		postags_alphabet, deprels_alphabet, parse, &participle_map, &tense_map, &number_map, &quoted_map, &bracket_map, &quote_bracket_map);
	/*if(is_dep_input){
=======
		postags_alphabet, deprels_alphabet, parse, is_dep_input, graph, &participle_map, &tense_map, &number_map, &quoted_map, &bracket_map, &quote_bracket_map);
	if(is_dep_input){
>>>>>>> morph_a_ref
//		graph.bfs(&deprels_alphabet);
		graph.add_ancestors();
		graphs.push_back(graph);
	}*/
    data.push_back(parse);
    instance ++;
  }

  return (int)data.size();
}

int
read_dependency_instance(std::istream& is,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& deprels_alphabet,
    dependency_t& parse, std::map<std::string, Participle> * participle_map, std::map<std::string, Tense> * tense_map, std::map<std::string, Number> * number_map, std::map<std::string, Quoted> * quoted_map, std::map<std::string, Bracket> * bracket_map, std::map<std::string, QuoteBracket> * quote_bracket_map) {
  std::string line;
  int line_no = 0;
  while (std::getline(is, line)) {
    algo::trim(line);
    if (line.size() == 0) { break; }

    std::vector<std::string> items;
    std::vector<word_t> extended_words;

    algo::split(items, line, boost::is_any_of("\t "),
        boost::token_compress_on);

    word_t form = 0;
    // "_" is a special tag representing unset.
    postag_t postag = (items[1] == "_" ? 0: postags_alphabet.insert(items[1]));
    deprel_t deprel = (items[3] == "_" ? 0: deprels_alphabet.insert(items[3]));

    dependency_t::range_t phrase;
    bool is_phrase;
    if (algo::starts_with(items[0], "__") && algo::ends_with(items[0], "__")) {
      // Detect the word is pre-recognized noun phrase, decode the phrase.
      std::vector<std::string> tokens;

      algo::trim_left_if(items[0], boost::is_any_of("_"));
      algo::trim_right_if(items[0], boost::is_any_of("_"));
      // Store the splitted token into chunk.
      algo::split(tokens, items[0], boost::is_any_of("_"), boost::token_compress_on);

      for (const std::string& token: tokens) {
        // temporally store the word in the extended words.
        extended_words.push_back(forms_alphabet.insert(token));
      }
      // If the current word is the last word of the phrase, set it as head.
      form = forms_alphabet.insert(tokens.back());

      // maintain the phrase.
      phrase.first = parse.words.size();
      phrase.second = phrase.first + tokens.size();
      is_phrase = true;
    } else {
      word_t word = forms_alphabet.insert(items[0]);
      form = word;
      extended_words.push_back(word);
      phrase.first = parse.words.size();
      phrase.second = phrase.first + 1;
      is_phrase = false;
    }


    Tense tense = Tense::kNone;
    Participle participle = Participle::kNone;
    Bracket bracket = Bracket::kNone;
    Quoted quoted = Quoted::kNone;
    Number number = Number::kNone;
    TenseParticiple tense_participle = TenseParticiple::kNone;
    QuoteBracket quote_bracket = QuoteBracket::kNone;
    if(quote_bracket_map->count(items[0]) >0){
    	quote_bracket = quote_bracket_map->at(items[0]);
    }
    if(postag==postags_alphabet.encode("VB")){
    	tense_participle = TenseParticiple::kDefault;
    }

    if (items.size() == 5) {
      parse.push_back(form, postag, atoi(items[2].c_str()),
          deprel, extended_words, phrase, is_phrase);
    } else if (items.size() > 5) {
      // Handle the instance with extra information.
      parse.push_back(form, postag, atoi(items[2].c_str()),
          deprel, extended_words, phrase, is_phrase, items[5]);

      for(int loop_index = 5; loop_index <items.size(); loop_index++){
    	  std::vector<std::string> extra_items;
    	  algo::split(extra_items, items[loop_index], boost::is_any_of("="),boost::token_compress_on);
    	  if(extra_items[0].compare("participle")==0){
    		  participle = participle_map->at(extra_items[1]);
    		  if(participle == Participle::kPresent){
    			  tense_participle = TenseParticiple::kPresentParticiple;
    		  }else{
    			  tense_participle = TenseParticiple::kPastParticiple;
    		  }
    	  }else if(extra_items[0].compare("tense")==0){
    		  tense = tense_map->at(extra_items[1]);
    		  if(tense == Tense::kPresent){
    			  tense_participle = TenseParticiple::kSimplePresent;
    		  }else{
    			  tense_participle = TenseParticiple::kSimplePast;
    		  }
    	  }else if(extra_items[0].compare("bracket")==0){
    		  bracket = bracket_map->at(extra_items[1]);
    	  }else if(extra_items[0].compare("quoted")==0){
			  quoted = quoted_map->at(extra_items[1]);
    	  }else if(extra_items[0].compare("num")==0){
			  number = number_map->at(extra_items[1]);
		  }
      }
    }
    std::vector<std::string> lexemes;
    boost::regex re("###");
	boost::sregex_token_iterator i(items[4].begin(), items[4].end(), re, -1);
	boost::sregex_token_iterator j;
	while(i != j){
		lexemes.push_back(*i++);
	}

    std::vector<word_t> word_lexemes;
    std::vector<PrefixType> prefix_types;
    for(std::string lexeme: lexemes){
    	word_lexemes.push_back(forms_alphabet.insert(lexeme));
    	bool contains_alpha = boost::regex_match(lexeme, boost::regex("^[a-z]+$"));
    	PrefixType prefix_type = PrefixType::kNone;
    	if(contains_alpha){
		  if(boost::starts_with(lexeme,"a")){
			prefix_type = PrefixType::kBeginsA;
		  }else if(boost::starts_with(lexeme,"e") && !boost::starts_with(lexeme,"eu")){
			prefix_type = PrefixType::kBeginsE;
		  }else if(boost::starts_with(lexeme,"i")){
			prefix_type = PrefixType::kBeginsI;
		  }else if(boost::starts_with(lexeme,"o") && !boost::starts_with(lexeme,"on")){
			prefix_type = PrefixType::kBeginsO;
		  }else if(boost::starts_with(lexeme,"u")){
			prefix_type = PrefixType::kBeginsU;
		  }else if(boost::starts_with(lexeme,"h")){
			prefix_type = PrefixType::kBeginsH;
		  }else if(boost::starts_with(lexeme,"eu")){
			prefix_type = PrefixType::kBeginsEu;
		  }else if(boost::starts_with(lexeme,"on")){
			prefix_type = PrefixType::kBeginsOn;
		  }
		}
		prefix_types.push_back(prefix_type);
//    	_INFO <<"lexeme "<< lexeme << " contains_alpha "<<contains_alpha << " prefix type "<<static_cast<std::underlying_type<PrefixType>::type>(prefix_type);
    }
    /*for(int word_lexeme: word_lexemes){
    	_TRACE<<word_lexeme;
    }*/
    parse.push_back(word_lexemes, prefix_types);
    parse.push_back_extra_info(tense_participle, number, quoted, bracket, quote_bracket);

    /*if(is_dep_input){
    	graph.add_to_parent_map(line_no++,atoi(items[2].c_str()),deprel);
    }*/
  }
}

void write_dependency_instance(std::ostream & os,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& deprels_alphabet,
    const dependency_t& parse,
	std::vector<int> & order,
	std::map<int,int>& order_map) {
  namespace eg = ZGen::Engine;

  int N = parse.forms.size();
  for (int i = 0; i < N; ++ i) {
    if (parse.is_phrases.at(i)) {
      os << "__";
      for (int j = parse.phrases.at(i).first; j < parse.phrases.at(i).second; ++ j) {
        os << forms_alphabet.decode(parse.words[j]);
        if (j + 1 < parse.phrases[i].second) {
          os << "_";
        }
      }
      os << "__";
    } else {
      os << forms_alphabet.decode(parse.forms.at(i));
    }
    os << "\t"
      << postags_alphabet.decode(parse.postags.at(i))
      << "\t"
      << parse.heads.at(i)
      << "\t"
      << deprels_alphabet.decode(parse.deprels.at(i))
	  << "\t"
	  << (order[i] != -1 ? order_map.at(order[i]): order[i])
	  << std::endl;
  }
  os << std::endl;
}

} //  end for namespace Utility
} //  end for namespace ZGen
