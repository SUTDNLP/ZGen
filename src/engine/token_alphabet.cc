#include "engine/token_alphabet.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZGen {

namespace Engine {

TokenAlphabet::TokenAlphabet(): global_id(0) {
  insert("-NONE-");
  insert("-BEGIN-");
  insert("-END-");
}

TokenAlphabet::~TokenAlphabet() {}

int TokenAlphabet::insert(const char * name) {
  std::string key(name);
  map_t::const_iterator itx = name2id.find(key);
  if (itx != name2id.end()) { return itx->second; }

  id2name.push_back(key);
  name2id[key] = global_id;
  return global_id ++;
}

int TokenAlphabet::insert(const std::string& name) {
  return insert(name.c_str());
}

const char * TokenAlphabet::decode(int id) const {
  if (id < 0 || id >= global_id) { return 0; }
  return id2name.at(id).c_str();
}

int TokenAlphabet::encode(const char* name) const {
  map_t::const_iterator itx = name2id.find(std::string(name));
  if (itx != name2id.end()) { return itx->second; }
  return -1;
}

int TokenAlphabet::encode(const std::string& name) const {
  return encode(name.c_str());
}

size_t TokenAlphabet::size() const { return name2id.size(); }

bool TokenAlphabet::save(std::ostream& ofs) const {
  if (!ofs.good()) { return false; }

  boost::archive::text_oarchive oa(ofs);
  oa << id2name;
  oa << name2id;
  oa << global_id;
  return true;
}

bool TokenAlphabet::load(std::istream& ifs) {
  if (!ifs.good()) { return false; }

  boost::archive::text_iarchive ia(ifs);
  ia >> id2name;
  ia >> name2id;
  ia >> global_id;
  return true;
}

} //  end for namespace Engine
} //  end for namespace ZGen
