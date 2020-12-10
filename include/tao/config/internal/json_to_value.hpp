// Copyright (c) 2016-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/json/

#ifndef TAO_CONFIG_INTERNAL_TO_VALUE_HPP
#define TAO_CONFIG_INTERNAL_TO_VALUE_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "json.hpp"
#include "json_traits.hpp"

namespace tao::config::internal
{
   template< template< typename... > class Traits >
   struct json_to_basic_value
   {
      explicit json_to_basic_value( const pegtl::position& p )
         : value( json::uninitialized, p )
      {}

      std::vector< json::basic_value< Traits > > stack_;
      std::vector< std::string > keys_;
      json::basic_value< Traits > value;

      void null( const pegtl::position& p )
      {
         value.set_null();
         value.public_base().position = p;
      }

      void boolean( const pegtl::position& p, const bool v )
      {
         value.set_boolean( v );
         value.public_base().position = p;
      }

      void number( const std::int64_t v, const pegtl::position& p )
      {
         value.set_signed( v );
         value.public_base().position = p;
      }

      void number( const std::uint64_t v, const pegtl::position& p )
      {
         value.set_unsigned( v );
         value.public_base().position = p;
      }

      void number( const double v, const pegtl::position& p )
      {
         value.set_double( v );
         value.public_base().position = p;
      }

      void string( const pegtl::position& p, const std::string_view v )
      {
         value.emplace_string( v );
         value.public_base().position = p;
      }

      void string( const pegtl::position& p, const char* v )
      {
         value.emplace_string( v );
         value.public_base().position = p;
      }

      void string( const pegtl::position& p, std::string&& v )
      {
         value.emplace_string( std::move( v ) );
         value.public_base().position = p;
      }

      void binary( const pegtl::position& p, const tao::binary_view v )
      {
         value.emplace_binary( v.begin(), v.end() );
         value.public_base().position = p;
      }

      void binary( const pegtl::position& p, std::vector< std::byte >&& v )
      {
         value.emplace_binary( std::move( v ) );
         value.public_base().position = p;
      }

      void begin_array( const pegtl::position& p )
      {
         stack_.emplace_back( json::empty_array, p );
      }

      void begin_array( const pegtl::position& p, const std::size_t size )
      {
         begin_array( p );
         stack_.back().get_array().reserve( size );
      }

      void element( const pegtl::position& /*unused*/ )
      {
         stack_.back().emplace_back( std::move( value ) );
      }

      void end_array( const pegtl::position& /*unused*/, const std::size_t /*unused*/ = 0 )
      {
         value = std::move( stack_.back() );
         stack_.pop_back();
      }

      void begin_object( const pegtl::position& p, const std::size_t /*unused*/ = 0 )
      {
         stack_.emplace_back( json::empty_object, p );
      }

      void key( const pegtl::position& /*unused*/, const std::string_view v )
      {
         keys_.emplace_back( v );
      }

      void key( const pegtl::position& /*unused*/, const char* v )
      {
         keys_.emplace_back( v );
      }

      void key( const pegtl::position& /*unused*/, std::string&& v )
      {
         keys_.emplace_back( std::move( v ) );
      }

      void member( const pegtl::position& /*unused*/ )
      {
         stack_.back().try_emplace( std::move( keys_.back() ), std::move( value ) );
         keys_.pop_back();
      }

      void end_object( const pegtl::position& /*unused*/, const std::size_t /*unused*/ = 0 )
      {
         value = std::move( stack_.back() );
         stack_.pop_back();
      }
   };

   using json_to_value = json_to_basic_value< json_traits >;

}  // namespace tao::config::internal

#endif
