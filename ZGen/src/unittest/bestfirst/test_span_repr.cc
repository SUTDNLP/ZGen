#define private public
#include "settings.h"
#include "types/instance.h"
#include "utils/ioutils.h"
#include "bestfirst/search/span_utils.h"
#include "bestfirst/search/span_allocator.h"
#include "unittest/bestfirst/fixture1.h"
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( span_repr ) {
  namespace bf = ZGen::BestFirst;
  Fixture1 f;
  std::string s; s.reserve(512);
  f.root->repr(s);

  std::cout << s << std::endl;
  //BOOST_CHECK_EQUAL(s.c_str(), "((01");
}
