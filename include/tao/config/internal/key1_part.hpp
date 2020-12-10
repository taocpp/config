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

      pegtl::position position;

   private:
      std::variant< part_star_t, part_minus_t, std::string, std::uint64_t > m_data;
   };

   // [[nodiscard]] inline std::string to_string( const key1_part& t )
   // {
   //    switch( t.type() ) {
   //       case key1_part::star:
   //          return "*";
   //       case key1_part::minus:
   //          return "-";
   //       case key1_part::name:
   //          return name_to_string( t.get_name() );
   //       case key1_part::index:
   //          return std::to_string( t.get_index() );
   //    }
   //    assert( false );
   // }

   // inline void name_to_stream( std::ostream& o, const std::string& name )
   // {
   //    if( is_ident( name ) ) {
   //       o << name;
   //    }
   //    else {
   //       o << '"' << json::internal::escape( name ) << '"';
   //    }
   // }

   // inline void to_stream( std::ostream& o, const key1_part& t )
   // {
   //    switch( t.type() ) {
   //       case key1_part::name:
   //          name_to_stream( o, t.get_name() );
   //          return;
   //       case key1_part::index:
   //          o << t.get_index();
   //          return;
   //       case key1_part::star:
   //          o << '*';
   //          return;
   //       case key1_part::minus:
   //          o << '-';
   //          return;
   //    }
   //    assert( false );
   // }

}  // namespace tao::config

#endif
