// Copyright (c) 2016-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config

#ifndef TAO_CONFIG_INTERNAL_JAXN_TO_ENTRY_HPP
#define TAO_CONFIG_INTERNAL_JAXN_TO_ENTRY_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "array.hpp"
#include "concat.hpp"
#include "constants.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "object.hpp"

namespace tao::config::internal
{
   struct jaxn_to_entry
   {
      jaxn_to_entry() noexcept = default;

      std::vector< entry > stack_;
      std::vector< std::string > keys_;
      std::optional< entry > value;

      void null( const pegtl::position& p )
      {
         value.emplace( internal::null( p ) );
      }

      void boolean( const bool v, const pegtl::position& p )
      {
         value.emplace( internal::boolean( v, p ) );
      }

      void number( const std::int64_t v, const pegtl::position& p )
      {
         value.emplace( internal::signed_t( v, p ) );
      }

      void number( const std::uint64_t v, const pegtl::position& p )
      {
         value.emplace( internal::unsigned_t( v, p ) );
      }

      void number( const double v, const pegtl::position& p )
      {
         value.emplace( internal::double_t( v, p ) );
      }

      void string( const std::string_view v, const pegtl::position& p )
      {
         value.emplace( internal::string_t( v, p ) );
      }

      void string( const char* v, const pegtl::position& p )
      {
         value.emplace( internal::string_t( v, p ) );
      }

      void string( std::string&& v, const pegtl::position& p )
      {
         value.emplace( internal::string_t( std::move( v ), p ) );
      }

      void binary( const tao::binary_view v, const pegtl::position& p )
      {
         binary( std::vector< std::byte >( v.data(), v.data() + v.size() ), p );
      }

      void binary( std::vector< std::byte >&& v, const pegtl::position& p )
      {
         value.emplace( internal::binary_t( std::move( v ), p ) );
      }

      void begin_array( const pegtl::position& p )
      {
         stack_.emplace_back( array_init, p );
      }

      void begin_array( const std::size_t /*unused*/, const pegtl::position& p )
      {
         begin_array( p );
      }

      void element( const pegtl::position& p )
      {
         concat& c = stack_.back().get_array().array.emplace_back( p );
         c.remove = true;
         c.concat.emplace_back( std::move( value ).value() );
      }

      void end_array( const pegtl::position& /*unused*/ )
      {
         value = std::move( stack_.back() );
         stack_.pop_back();
      }

      void end_array( const std::size_t /*unused*/, const pegtl::position& p )
      {
         end_array( p );
      }

      void begin_object( const pegtl::position& p )
      {
         stack_.emplace_back( object_init, p );
      }

      void begin_object( const std::size_t /*unused*/, const pegtl::position& p )
      {
         begin_object( p );
      }

      void key( const std::string_view v, const pegtl::position& /*unused*/ )
      {
         keys_.emplace_back( v );
      }

      void key( const char* v, const pegtl::position& /*unused*/ )
      {
         keys_.emplace_back( v );
      }

      void key( std::string&& v, const pegtl::position& /*unused*/ )
      {
         keys_.emplace_back( std::move( v ) );
      }

      void member( const pegtl::position& p )
      {
         auto [ i, b ] = stack_.back().get_object().object.try_emplace( std::move( keys_.back() ), p );
         (void)b;
         keys_.pop_back();
         i->second.remove = true;
         i->second.concat.emplace_back( std::move( value ).value() );
      }

      void end_object( const pegtl::position& /*unused*/ )
      {
         value = std::move( stack_.back() );
         stack_.pop_back();
      }

      void end_object( const std::size_t /*unused*/, const pegtl::position& p )
      {
         end_object( p );
      }
   };

}  // namespace tao::config::internal

#endif
