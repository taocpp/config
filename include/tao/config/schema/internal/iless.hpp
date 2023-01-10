// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_SCHEMA_INTERNAL_ILESS_HPP
#define TAO_CONFIG_SCHEMA_INTERNAL_ILESS_HPP

#include <string_view>

#if defined( _MSC_VER )
#include <string.h>
#define strncasecmp _strnicmp
#else
#include <strings.h>
#endif

namespace tao::config::schema::internal
{
   struct iless
   {
      // TODO: Compare international strings (Unicode, using ICU)
      bool operator()( const std::string_view lhs, const std::string_view rhs ) const noexcept
      {
         const auto d = strncasecmp( &( *lhs.begin() ), &( *rhs.begin() ), ( std::min )( lhs.size(), rhs.size() ) );
         return ( d < 0 ) || ( ( d == 0 ) && ( lhs.size() < rhs.size() ) );
      }
   };

}  // namespace tao::config::schema::internal

#endif
