
#ifndef _DD_TEST_UTIL_H__
#define _DD_TEST_UTIL_H__
#include <string>

// when len <= prefixLen
//   test_str(adfadf, 3) == adf
// when len < 20
//   test_str(adf, 13) == adfa:67890123
// when len >= 20
//   test_str(asdf, 45) = asdf:=====(20_56789)(30_56789)12345
std::string test_str(const std::string& str_prefix, const size_t str_len);

#endif
