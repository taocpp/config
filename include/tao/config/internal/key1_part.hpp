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
   class key1_part
   {
   public:
      key1_part( const part_star_t t, const pegtl::position& p )
         : position( p ),
           m_data( t )
      {}

      key1_part( const part_minus_t t, const pegtl::position& p )
         : position( p ),
           m_data( t )
      {}

      // key1_part( const char, const pegtl::position& ) = delete;
      // key1_part( const signed char, const pegtl::position& ) = delete;
      // key1_part( const unsigned char, const pegtl::position& ) = delete;

      key1_part( const std::uint64_t i, const pegtl::position& p )
         : position( p ),
           m_data( i )
      {}

      key1_part( const std::string& n, const pegtl::position& p )
         : position( p ),
           m_data( n )
      {}

      [[nodiscard]] key1_kind kind() const noexcept
      {
         return key1_kind( m_data.index() );
      }

      void inc_index()
      {
         auto* s = std::get_if< std::uint64_t >( &m_data );
         assert( s != nullptr );
         ++*s;
      }

      [[nodiscard]] std::uint64_t get_index() const noexcept
      {
         const auto* s = std::get_if< std::uint64_t >( &m_data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] const std::string& get_name() const noexcept
      {
         const auto* s = std::get_if< std::string >( &m_data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] bool is_implicit() const noexcept
      {
         return ( position.line == 0 ) && ( position.column == 0 );
      }

      pegtl::position position;

   private:
      std::variant< part_star_t, part_minus_t, std::string, std::uint64_t > m_data;
   };

}  // namespace tao::config::internal

#endif
