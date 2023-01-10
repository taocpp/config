// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_JSON_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_JSON_TRAITS_HPP

#include "json.hpp"
#include "json_base.hpp"

namespace tao::config::internal
{
   template< typename T >
   struct json_traits
      : json::traits< T >
   {};

   template<>
   struct json_traits< void >
      : json::traits< void >
   {
      static constexpr const bool enable_implicit_constructor = false;

      template< typename Value >
      using public_base = json_base;
   };

}  // namespace tao::config::internal

#endif
