#ifndef __ZGEN_UTILS_IOUTILS_H__
#define __ZGEN_UTILS_IOUTILS_H__

#include <iostream>
#include <vector>
#include "types/instance.h"
#include "engine/token_alphabet.h"

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
    dependency_t& parse);

void write_dependency_instance(std::ostream& os,
    const Engine::TokenAlphabet& forms_alphabet,
    const Engine::TokenAlphabet& postags_alphabet,
    const Engine::TokenAlphabet& deprels_alphabet,
    const dependency_t& parse);

void write_dependency_dataset(std::ostream& os,
    const std::vector<dependency_t>& dataset);
} //  end for namespace Utility

} //  end for namespace ZGen

#endif  //  end for __ZGEN_UTILS_IOUTILS_H__
