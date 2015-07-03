#ifndef __ZGEN_ENGINE_TOKEN_ALPHABET_H__
#define __ZGEN_ENGINE_TOKEN_ALPHABET_H__

#include <iostream>
#include <unordered_map>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#include <boost/serialization/unordered_map.hpp>

namespace ZGen {
namespace Engine {

class TokenAlphabet {
//! Tokenalphabet is the class used to store bidirection relations on string->int
//! int->string.
public:
  //! Constructor
  TokenAlphabet();

  //! Destructor
  ~TokenAlphabet();

  /**
   * Insert a token into the alphabet.
   *
   *  @param[in]  name        The word string.
   *  @return     int         Return 1 if the word doesn't exist in the alphabet,
   *                          otherwise 0.
   */
  int insert(const char* name);

  int insert(const std::string& name);

  /**
   * Get the name for a given index.
   *
   *  @param[in]  id          The index.
   *  @return     const char* Return the pointer to the name. If the index is
   *                          not in legal range, NULL is returned.
   */
  const char* decode(int id) const;

  /**
   * Get the index for a given name.
   *
   *  @param[in]  name        The name string.
   *  @return     int         Return the index of the name.
   */
  int encode(const char* name) const;

  int encode(const std::string& name) const;
  
  /**
   * Save the word alphabet into the output file stream.
   *
   *  @param[out] ofs   The output file stream.
   */
  bool save(std::ostream& ofs) const;

  /**
   * Load the word alphabet from input file stream.
   *
   *  @param[in]  ifs   The input file stream.
   */
  bool load(std::istream& ifs);

  //! Get the number of elements in the alphabet.
  size_t size() const;

  enum SPECIAL_TOKEN {
    NONE, BEGIN, END
  };

private:
  //! Define the mapping type.
  typedef std::unordered_map<std::string, int> map_t;
  //! The serialization entry.
  friend class boost::serialization::access;
  //! The index to name payload.
  std::vector<std::string> id2name;
  //! The name to index payload.
  map_t name2id;
  //! The global index of the word.
  int global_id;

  //! The function of serialization.
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & global_id;
    ar & name2id;
    ar & id2name;
  }
};


} //  end for namespace Engine
} //  end for namespace ZGen

#endif    //  end for __ZGEN_ENGINE_TOKEN_ALPHABET_H__
