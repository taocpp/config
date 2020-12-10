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
#include "ref2_kind.hpp"

namespace tao::config::internal
{
   class ref2_part
   {
   public:
      ref2_part( const part_minus_t t, const pegtl::position& p )
         : position( p ),
           m_data( t )
      {}

      ref2_part( const part_vector_t /*unused*/, const pegtl::position& p )
         : position( p ),
           m_data( std::vector< ref2_part >() )
      {}

      ref2_part( const std::uint64_t i, const pegtl::position& p )
         : position( p ),
           m_data( i )
      {}

      ref2_part( const std::string& n, const pegtl::position& p )
         : position( p ),
           m_data( n )
      {}

      [[nodiscard]] ref2_kind kind() const noexcept
      {
         return ref2_kind( m_data.index() + 1 );
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

      [[nodiscard]] std::vector< ref2_part >& get_vector() noexcept
      {
         auto* s = std::get_if< std::vector< ref2_part > >( &m_data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] const std::vector< ref2_part >& get_vector() const noexcept
      {
         const auto* s = std::get_if< std::vector< ref2_part > >( &m_data );
         assert( s != nullptr );
         return *s;
      }

      pegtl::position position;

   private:
      std::variant< part_minus_t, std::string, std::uint64_t, std::vector< ref2_part > > m_data;
   };

}  // namespace tao::config

#endif
