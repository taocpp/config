// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_RESULT_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_RESULT_TRAITS_HPP

#include <string>
#include <string_view>

#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename T >
   struct result_traits
   {
      static json_t convert( const pegtl::position& p, const T& t )
      {
         return json_t( t, p );
      }
   };

   template<>
   struct result_traits< json_t >
   {
      static json_t convert( const pegtl::position& /*unused*/, json_t&& t )
      {
         return std::move( t );
      }

      static json_t convert( const pegtl::position& /*unused*/, const json_t& t )
      {
         return t;
      }
   };

   template<>
   struct result_traits< std::string >
   {
      static json_t convert( const pegtl::position& p, const std::string& s )
      {
         if( !json::internal::validate_utf8_nothrow( s ) ) {
            throw pegtl::parse_error( "invalid utf-8 in extension result", p );  // TODO: Name of extension?
         }
         return json_t( s, p );
      }
   };

   template<>
   struct result_traits< std::string_view >
   {
      static json_t convert( const pegtl::position& p, const std::string_view sv )
      {
         return result_traits< std::string >::convert( p, std::string( sv ) );
      }
   };

}  // namespace tao::config::internal

#endif
