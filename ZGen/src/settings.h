#ifndef __ZGEN_SETTINGS_H__
#define __ZGEN_SETTINGS_H__


namespace ZGen {

const int kMaxNumberOfWords = 180;

const int kMaxLengthOfBuffer = 2048;

const int kMaxNumberOfPostags = 1024;

const int kStartIndexOfValidPostag = 3;

const int kStartIndexOfValidDependencyRelation = 1;

typedef double floatval_t;

const floatval_t inf = 1e20;

} //  end for namespace ZGen

#endif  //  end for __ZGEN_SETTINGS_H__
