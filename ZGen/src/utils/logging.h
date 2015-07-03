#ifndef __ZGEN_UTILS_LOGGING_H__
#define __ZGEN_UTILS_LOGGING_H__

#include <boost/log/trivial.hpp>
#define _TRACE  BOOST_LOG_TRIVIAL(trace)
#define _DEBUG  BOOST_LOG_TRIVIAL(debug)
#define _INFO   BOOST_LOG_TRIVIAL(info)
#define _WARN   BOOST_LOG_TRIVIAL(warning)
#define _ERROR  BOOST_LOG_TRIVIAL(error)

namespace ZGen {

namespace Utility {

void init_boost_logging(bool verbose);

} //  end for namespace Utils

} //  end for namespace ZGen

#endif  //  end for __ZGEN_UTILS_LOGGING_H__
