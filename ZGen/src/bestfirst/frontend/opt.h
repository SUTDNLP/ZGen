#ifndef __ZGEN_BESTFIRST_FRONTEND_OPT_H__
#define __ZGEN_BESTFIRST_FRONTEND_OPT_H__

#include <boost/program_options.hpp>
#include "bestfirst/option.h"

/**
 * Convert the options in variable maps into an option types.
 *
 *  @param[in]  learn If the learning mode is activated.
 *  @param[in]  vm    The variables map.
 *  @param[out] opts  The options.
 *  @return     bool  If the input variable_map is correct, return
 *                    true, otherwise false.
 */
bool parse_options(bool learn,
    const boost::program_options::variables_map& vm,
    ZGen::BestFirst::Option& opts);

#endif  //  end for __ZGEN_BESTFIRST_FRONTEND_OPT_H__
