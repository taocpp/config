// Copyright (c) 2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STRING_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_STRING_UTILITY_HPP

#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#include "json.hpp"

namespace tao::config::internal
{
   inline std::string_view strcat_to_string( const json::type t )
   {
      return json::to_string( t );
   }

   template< typename T >
   auto strcat_to_string( const T& t )
   {
      std::ostringstream os;
      os << t;
      return os.str();
   }

   template< typename... Ts >
   [[nodiscard]] std::string strcat( Ts&&... ts )
   {
      return ( std::string() += ... += strcat_to_string( ts ) );
   }

}  // namespace tao::config::internal

#endif
