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
#include "json_traits.hpp"
#include "key1.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   struct arguments_unready
   {};

   template< typename, typename = void >
   struct function_traits;

   [[nodiscard]] inline decltype( auto ) function_traits_value( array& f, const std::size_t i )
   {
      assert( f.array.size() > i );
      auto p = f.array.begin();
      std::advance( p, i );
      if( p->concat.size() > 1 ) {
         throw arguments_unready();
      }
      const entry& e = p->concat.front();
      if( e.kind() != entry_kind::value ) {
         throw arguments_unready();
      }
      return p->concat.front().get_value();
   }

   template<>
   struct function_traits< json_t >
   {
      static void put( entry& e, const json_t& t )
      {
         e.set_value( t );
      }
   };

   template<>
   struct function_traits< std::string >
   {
      [[nodiscard]] static std::string get( array& f, std::size_t& i )
      {
         const json_t& j = function_traits_value( f, i++ );
         if( j.type() == json::type::STRING ) {
            return j.get_string();
         }
         if( j.type() == json::type::BINARY ) {
            const auto bv = j.as< tao::binary_view >();
            std::string s( reinterpret_cast< const char* >( bv.data() ), bv.size() );
            if( !json::internal::validate_utf8_nothrow( s ) ) {
               throw pegtl::parse_error( "invalid utf-8 in binary data used as string argument", f.position );
            }
            return s;
         }
         throw pegtl::parse_error( "invalid type for string argument", f.position );
      }

      static void put( entry& e, const std::string& s )
      {
         e.set_value( json_t( s ) );
      }
   };

   template<>
   struct function_traits< tao::binary_view >
   {
      [[nodiscard]] static tao::binary_view get( array& f, std::size_t& i )
      {
         const json_t& j = function_traits_value( f, i++ );
         if( j.is_binary_type() ) {
            return j.get_binary_type();
         }
         throw pegtl::parse_error( "invalid type for binary argument", f.position );
      }
   };

   template<>
   struct function_traits< std::vector< std::byte > >
   {
      static void put( entry& e, const std::vector< std::byte >& b )
      {
         e.set_value( json_t( b ) );
      }
   };

   template<>
   struct function_traits< std::vector< std::string > >
   {
      static void put( entry& e, const std::vector< std::string >& v )
      {
         e.set_value( json_t( v ) );
      }
   };

   template<>
   struct function_traits< pegtl::position >
   {
      [[nodiscard]] static pegtl::position get( array& f, const std::size_t /*unused*/ )
      {
         return f.position;
      }
   };

}  // namespace tao::config::internal

#endif
