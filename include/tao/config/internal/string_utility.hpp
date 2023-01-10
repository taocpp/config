// Copyright (c) 2021-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STRING_UTILITY_HPP
#define TAO_CONFIG_INTERNAL_STRING_UTILITY_HPP

#include <sstream>
#include <string>
#include <utility>

#include "json.hpp"

namespace tao::config::internal
{
   struct strcat_stream
   {
      std::ostringstream oss;
   };

   template< typename T >
   strcat_stream& operator<<=( strcat_stream& ss, const T& t )
   {
      ss.oss << t;
      return ss;
   }

   inline strcat_stream& operator<<=( strcat_stream& ss, const json::type t )
   {
      ss.oss << json::to_string( t );
      return ss;
   }

   template< typename... Ts >
   [[nodiscard]] std::string strcat( const Ts&... ts )
   {
      strcat_stream ss;
      (void)( ss <<= ... <<= ts );
      return std::move( ss.oss ).str();
   }

}  // namespace tao::config::internal

#endif
