// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY1_PART_HPP
#define TAO_CONFIG_INTERNAL_KEY1_PART_HPP

#include <cassert>
#include <ostream>
#include <string>
#include <variant>

#include "constants.hpp"
#include "key1_kind.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   struct key1_part
   {
      key1_part( const part_star_t t, const pegtl::position& p )
         : position( p ),
           data( t )
      {}

      key1_part( const part_minus_t t, const pegtl::position& p )
         : position( p ),
           data( t )
      {}

      // key1_part( const char, const pegtl::position& ) = delete;
      // key1_part( const signed char, const pegtl::position& ) = delete;
      // key1_part( const unsigned char, const pegtl::position& ) = delete;

      key1_part( const std::uint64_t i, const pegtl::position& p )
         : position( p ),
           data( i )
      {}

      key1_part( const std::string& n, const pegtl::position& p )
         : position( p ),
           data( n )
      {}

      [[nodiscard]] key1_kind kind() const noexcept
      {
         return key1_kind( data.index() );
      }

      void inc_index()
      {
         auto* s = std::get_if< std::uint64_t >( &data );
         assert( s != nullptr );
         ++*s;
      }

      [[nodiscard]] std::uint64_t get_index() const noexcept
      {
         const auto* s = std::get_if< std::uint64_t >( &data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] const std::string& get_name() const noexcept
      {
         const auto* s = std::get_if< std::string >( &data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] bool is_implicit() const noexcept
      {
         return position.byte == std::size_t( -1 );
      }

      pegtl::position position;
      std::variant< part_star_t, part_minus_t, std::string, std::uint64_t > data;
   };

}  // namespace tao::config::internal

#endif
