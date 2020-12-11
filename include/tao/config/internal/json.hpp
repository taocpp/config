// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_JSON_HPP
#define TAO_CONFIG_INTERNAL_JSON_HPP

#include <tao/json.hpp>

#include <tao/json/contrib/position.hpp>
#include <tao/json/contrib/traits.hpp>  // TODO: This might be a problem...

namespace tao::config::internal
{
   template< typename T >
   struct json_traits;

   using json_t = tao::json::basic_value< json_traits >;

}  // namespace tao::config::internal

#endif
