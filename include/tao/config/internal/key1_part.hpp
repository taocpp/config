// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY1_PART_HPP
#define TAO_CONFIG_INTERNAL_KEY1_PART_HPP

#include <cassert>
#include <string>
#include <variant>

#include "constants.hpp"
#include "key1_kind.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   struct key1_part
   {
      using data_t = std::variant< std::string, std::size_t, part_star_t, std::shared_ptr< std::uint64_t > >;

      key1_part( const part_star_t t, const pegtl::position& p )
         : position( p ),
           data( t )
      {}

      // key1_part( const char, const pegtl::position& ) = delete;
      // key1_part( const signed char, const pegtl::position& ) = delete;
      // key1_part( const unsigned char, const pegtl::position& ) = delete;

      key1_part( const std::size_t i, const pegtl::position& p )
         : position( p ),
           data( i )
      {}

      key1_part( const pegtl::position& p, const std::uint64_t g )
         : position( p ),
           data( std::make_shared< std::uint64_t >( g ) )
      {}

      key1_part( const std::string& n, const pegtl::position& p )
         : position( p ),
           data( n )
      {}

      [[nodiscard]] key1_kind kind() const noexcept
      {
         return key1_kind( data.index() );
      }

      [[nodiscard]] std::size_t get_index() const noexcept
      {
         const auto* s = std::get_if< std::size_t >( &data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] const std::string& get_name() const noexcept
      {
         const auto* s = std::get_if< std::string >( &data );
         assert( s != nullptr );
         return *s;
      }

      void set_generation( const std::uint64_t g ) noexcept
      {
         assert( g > 0 );

         if( const auto* s = std::get_if< std::shared_ptr< std::uint64_t > >( &data ) ) {
            assert( s != nullptr );
            assert( s->get() != nullptr );
            assert( g > **s );
            **s = g;
         }
      }

      [[nodiscard]] std::uint64_t get_generation() const noexcept
      {
         const auto* s = std::get_if< std::shared_ptr< std::uint64_t > >( &data );
         assert( s != nullptr );
         assert( s->get() != nullptr );
         assert( **s > 0 );
         return **s;
      }

      pegtl::position position;
      data_t data;
   };

}  // namespace tao::config::internal

#endif
