#include "string_split.h"
#include "string_util.h"

namespace base {
std::vector<std::string> SplitString(
  std::string str,
  std::string delimiter,
  WhitespaceHandling whitespace,
  SplitResult result_type) {
  std::vector<std::string> result;
  if (str.empty())
    return result;

  size_t start = 0;
  while (start != std::string::npos) {
    size_t end = str.find(delimiter, start);

    std::string piece;
    if (end == std::string::npos) {
      piece = str.substr(start);
      start = std::string::npos;
    }
    else {
      piece = str.substr(start, end - start);
      start = end + 1;
    }

    if (whitespace == TRIM_WHITESPACE)
      piece = TrimString(piece, " ", TRIM_ALL);

    if (result_type == SPLIT_WANT_ALL || !piece.empty())
      result.push_back(piece);
  }
  return result;
}
}