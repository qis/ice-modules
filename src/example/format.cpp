module;
#include <format>
#include <string>
module ice.example:format;
import :modify;

namespace ice::example {

extern int modify(int);

std::string format(int value)
{
  return std::format("value: {}", modify(value));
}

}  // namespace ice::example
