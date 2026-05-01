#ifndef OHAL_CORE_ACCESS_HPP
#define OHAL_CORE_ACCESS_HPP

#include <cstdint>

namespace ohal::core {

enum class Access : uint8_t {
  ReadOnly = 0,
  WriteOnly = 1,
  ReadWrite = 2,
};

} // namespace ohal::core

#endif // OHAL_CORE_ACCESS_HPP
