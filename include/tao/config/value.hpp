// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_VALUE_HPP
#define TAO_CONFIG_VALUE_HPP

#include <tao/json.hpp>

#include "traits.hpp"

namespace tao::config
{
   using value = json::basic_value< traits >;

}  // namespace tao::config

#endif
