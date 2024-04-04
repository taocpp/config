// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FUNCTION_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_FUNCTION_TRAITS_HPP

#include <cassert>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

#include "array.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   struct arguments_unready
   {};

   template< typename, typename = void >
   struct function_traits;

   template<>
   struct function_traits< entry >
   {
      static void put( entry& e, entry&& f )
      {
         e = std::move( f );
      }

      static void put( entry& e, const entry& f )
      {
         e = f;
      }
   };

   template< typename T >
   struct function_traits< atom< T > >
   {
      static void put( entry& e, atom< T >&& v )
      {
         e.set_value( std::move( v ) );
      }

      static void put( entry& e, const atom< T >& v )
      {
         e.set_value( v );
      }
   };

   [[nodiscard]] inline const entry& function_traits_entry( array& f, const std::size_t i )
   {
      assert( f.array.size() > i );

      auto p = f.array.begin();
      std::advance( p, i );

      assert( !p->concat.empty() );

      if( p->concat.size() != 1 ) {
         throw arguments_unready();
      }
      return p->concat.front();
   }

   template<>
   struct function_traits< std::string >
   {
      [[nodiscard]] static std::string get( array& f, const std::size_t i )
      {
         const entry& e = function_traits_entry( f, i );

         if( e.is_string() ) {
            return e.get_string();
         }
         if( e.is_binary() ) {
            const std::vector< std::byte >& b = e.get_binary();
            const std::string s( reinterpret_cast< const char* >( b.data() ), b.size() );
            if( !json::internal::validate_utf8_nothrow( s ) ) {
               throw pegtl::parse_error( "invalid utf-8 in binary data used as string", e.get_position() );
            }
            return s;
         }
         throw pegtl::parse_error( "invalid type for string argument", e.get_position() );
      }
   };

}  // namespace tao::config::internal

#endif
