// Copyright (c) 2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STRING_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_STRING_UTILITY_HPP

#include <string>
#include <utility>

#include "json.hpp"

namespace tao::config::internal
{
   inline std::string_view strcat_to_string( const json::type t )
   {
      return json::to_string( t );
   }

   inline std::string_view strcat_to_string( const std::string_view t )
   {
      return t;
   }

   inline char strcat_to_string( const char c )
   {
      return c;
   }

   template< typename... Ts >
   [[nodiscard]] std::string strcat( Ts&&... ts )
   {
      return ( std::string() += ... += strcat_to_string( ts ) );
   }

}  // namespace tao::config::internal

#endif
