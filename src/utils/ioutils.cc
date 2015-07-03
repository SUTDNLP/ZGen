#include <iostream>
#include <fstream>
#include <cstring>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include "utils/ioutils.h"
#include "utils/logging.h"

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

  // Loop over the instances
  while (instance != eos) {
    std::istringstream iss(*instance);
    dependency_t parse;
    read_dependency_instance(iss, forms_alphabet,
        postags_alphabet, deprels_alphabet, parse);
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
    dependency_t& parse) {
  std::string line;
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

    if (items.size() == 4) {
      parse.push_back(form, postag, atoi(items[2].c_str()),
          deprel, extended_words, phrase, is_phrase);
    } else if (items.size() > 4) {
      // Handle the instance with extra information.
      parse.push_back(form, postag, atoi(items[2].c_str()),
          deprel, extended_words, phrase, is_phrase, items[4]);
    }
  }
}

void write_dependency_instance(std::ostream & os,
    const eg::TokenAlphabet& forms_alphabet,
    const eg::TokenAlphabet& postags_alphabet,
    const eg::TokenAlphabet& deprels_alphabet,
    const dependency_t& parse) {
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
      << std::endl;
  }
  os << std::endl;
}

} //  end for namespace Utility
} //  end for namespace ZGen
