// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_VALUE_HPP
#define TAO_CONFIG_VALUE_HPP

#include "external/json.hpp"

#include "traits.hpp"

namespace tao
{
   namespace config
   {
      using value = json::basic_value< traits >;

   }  // namespace config

}  // namespace tao

#endif
