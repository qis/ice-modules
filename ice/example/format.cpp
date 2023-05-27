module;
import std.format;
import std.string;

export module ice.example:format;

import :modify;

namespace ice::example {

std::string format(int value)
{
  return std::format("value: {}", modify(value));
}

}  // namespace ice::example
