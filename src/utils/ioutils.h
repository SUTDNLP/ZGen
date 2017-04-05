#ifndef __ZGEN_UTILS_IOUTILS_H__
#define __ZGEN_UTILS_IOUTILS_H__

#include <iostream>
#include <vector>
#include "types/instance.h"
#include "types/graph.h"
#include "engine/token_alphabet.h"
#include <map>
#include <unordered_map>

namespace ZGen {
namespace IO {

/**
 * Get the ifstream from the file specified by filename. If opening fails,
 * use the provided default stream instead.
 *
 *  @param[in]  filename        The name of the file.
 *  @param[in]  default_stream  The user specified stream.
 *  @return     std::istream    The built stream.
 */
std::istream* get_istream(const char* filename,
    std::istream* default_stream = (&(std::cin)));

//! std::string wrapper for get_istream
std::istream* get_istream(const std::string& filename,
    std::istream* default_stream = (&(std::cin)));

/**
 * Get the ostream from the file specified by filename. If opening fails,
 * use the provided default stream instead.
 *
 *  @param[in]  filename        The name of the file.
 *  @param[in]  default_stream  The user specified stream.
 *  @return     std::ostream    The built stream.
 */
std::ostream* get_ostream(const char* filename,
    std::ostream* default_stream = (&(std::cout)));

//! std::string wrapper for get_ostream.
std::ostream* get_ostream(const std::string& filename,
    std::ostream* default_stream = (&(std::cout)));

void read_previous_words(std::istream& is, Engine::TokenAlphabet& forms_alphabet, std::set<int> & gold_previous_words);
/**
 * Reads mapping between input node and gold input node to be used during training
 */
void read_mapping(std::istream& is, std::vector<dependency_t>& data);

void read_mapping_instance(std::istream& is, std::map<int, int>& input_gold_node_mapping, std::vector<int>& function_words, std::map<int,int>& gold_input_node_mapping);

/**
 * Read dataset from the input stream.
 *
 *  @param[in]  is                The input stream.
 *  @param[in]  forms_alphabet    The alphabet of the forms.
 *  @param[in]  postags_alphabet  The alphabet of the postags.
 *  @param[in]  deprels_alphabet  The alphabet of the dependency relations.
 *  @param[out] dataset           The output dataset.
 *  @return     int               The number of input instances.
 */
int read_dependency_dataset(std::istream& is,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& deprels_alphabet,
    std::vector<dependency_t>& dataset);

/**
 * Read one instannce from the input stream.
 *
 *  @param[in]  is                The input stream.
 *  @param[in]  forms_alphabet    The alphabet of the forms.
 *  @param[in]  postags_alphabet  The alphabet of the postags.
 *  @param[in]  deprels_alphabet  The alphabet of the dependency relations.
 *  @param[out] dataset           The output dataset.
 *  @return     int               The number of input instances.
 */
int read_dependency_instance(std::istream& is,
    Engine::TokenAlphabet& forms_alphabet,
    Engine::TokenAlphabet& postags_alphabet,
    Engine::TokenAlphabet& deprels_alphabet,
    dependency_t& parse, std::map<std::string, Participle> * participle_map, std::map<std::string, Tense> * tense_map, std::map<std::string, Number> * number_map, std::map<std::string, Quoted> * quoted_map, std::map<std::string, Bracket> * bracket_map, std::map<std::string, QuoteBracket> * quote_bracket_map);

void write_dependency_instance(std::ostream& os,
    const Engine::TokenAlphabet& forms_alphabet,
    const Engine::TokenAlphabet& postags_alphabet,
    const Engine::TokenAlphabet& deprels_alphabet,
    const dependency_t& parse,
	std::vector<int> & order,
	std::map<int,int>& order_map);

void write_dependency_dataset(std::ostream& os,
    const std::vector<dependency_t>& dataset);

int read_graph(std::istream& is, graph_t& graph, Engine::TokenAlphabet& deprels_alphabet);

int read_graphs(std::istream& is, std::vector<graph_t> &data, Engine::TokenAlphabet& deprels_alphabet);

} //  end for namespace Utility

} //  end for namespace ZGen

#endif  //  end for __ZGEN_UTILS_IOUTILS_H__
