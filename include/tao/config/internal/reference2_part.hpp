// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PART2_HPP
#define TAO_CONFIG_INTERNAL_PART2_HPP

#include <cassert>
#include <ostream>
#include <string>
#include <variant>
#include <vector>

#include "constants.hpp"
#include "pegtl.hpp"
#include "reference2_kind.hpp"

namespace tao::config::internal
{
   struct reference2_part
   {
      reference2_part( const part_minus_t t, const pegtl::position& p )
         : position( p ),
           data( t )
      {}

      reference2_part( const part_vector_t /*unused*/, const pegtl::position& p )
         : position( p ),
           data( std::vector< reference2_part >() )
      {}

      reference2_part( const std::uint64_t i, const pegtl::position& p )
         : position( p ),
           data( i )
      {}

      reference2_part( const std::string& n, const pegtl::position& p )
         : position( p ),
           data( n )
      {}

      [[nodiscard]] reference2_kind kind() const noexcept
      {
         return reference2_kind( data.index() + 1 );
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

      [[nodiscard]] std::vector< reference2_part >& get_vector() noexcept
      {
         auto* s = std::get_if< std::vector< reference2_part > >( &data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] const std::vector< reference2_part >& get_vector() const noexcept
      {
         const auto* s = std::get_if< std::vector< reference2_part > >( &data );
         assert( s != nullptr );
         return *s;
      }

      pegtl::position position;
      std::variant< part_minus_t, std::string, std::uint64_t, std::vector< reference2_part > > data;
   };

}  // namespace tao::config::internal

#endif