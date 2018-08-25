#ifndef KVNSFER_HTTP_SERVER_STRING_SPLIT_H_
#define KVNSFER_HTTP_SERVER_STRING_SPLIT_H_

#include <vector>
#include <string>

namespace base {

enum WhitespaceHandling {
  KEEP_WHITESPACE,
  TRIM_WHITESPACE,
};

enum SplitResult {
  // Strictly return all results.
  //
  // If the input is ",," and the separator is ',' this will return a
  // vector of three empty strings.
  SPLIT_WANT_ALL,

  // Only nonempty results will be added to the results. Multiple separators
  // will be coalesced. Separators at the beginning and end of the input will
  // be ignored. With TRIM_WHITESPACE, whitespace-only results will be dropped.
  //
  // If the input is ",," and the separator is ',', this will return an empty
  // vector.
  SPLIT_WANT_NONEMPTY,
};

std::vector<std::string> SplitString(
  std::string input,
  std::string separators,
  WhitespaceHandling whitespace,
  SplitResult result_type);

} // namespace base

#endif // KVNSFER_HTTP_SERVER_STRING_SPLIT_H_