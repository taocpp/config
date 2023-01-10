// Copyright (c) 2021-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_LIMITS_HPP
#define TAO_CONFIG_INTERNAL_LIMITS_HPP

#include <cstdint>

namespace tao::config::internal
{
   static constexpr std::size_t global_nesting_limit = 96;

}  // namespace tao::config::internal

#endif
