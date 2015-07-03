#ifndef __ZGEN_SR_OPT_H__
#define __ZGEN_SR_OPT_H__

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "shiftreduce/option.h"

bool parse_options(bool learn,
    boost::program_options::variables_map& vm,
    ZGen::ShiftReduce::option_t& opts);

#endif  //  end for __ZGEN_SR_OPT_H__
