// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_KEY_PART_HPP
#define TAO_CONFIG_KEY_PART_HPP

#include <cassert>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>

#include "internal/constants.hpp"
#include "internal/json.hpp"
#include "internal/key_grammar.hpp"
#include "internal/pegtl.hpp"

#include "key_kind.hpp"

namespace tao::config
{
   struct key_part
   {
      using data_t = std::variant< std::string, std::size_t >;

      explicit key_part( const std::size_t i ) noexcept
         : data( i )
      {}

      explicit key_part( std::string&& n ) noexcept
         : data( std::move( n ) )
      {}

      explicit key_part( const std::string& n )
         : data( n )
      {}

      [[nodiscard]] key_kind kind() const noexcept
      {
         return key_kind( data.index() );
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

      data_t data;
   };

   [[nodiscard]] inline bool operator<( const key_part& l, const key_part& r ) noexcept
   {
      return l.data < r.data;
   }

   [[nodiscard]] inline bool operator==( const key_part& l, const key_part& r ) noexcept
   {
      return l.data == r.data;
   }

   [[nodiscard]] constexpr bool is_alpha( const int c ) noexcept
   {
      return ( ( 'a' <= c ) && ( c <= 'z' ) ) || ( ( 'A' <= c ) && ( c <= 'Z' ) );
   }

   [[nodiscard]] constexpr bool is_first( const int c ) noexcept
   {
      return is_alpha( c ) || ( c == '_' );
   }

   [[nodiscard]] inline bool is_identifier( const std::string& n )
   {
      using grammar = pegtl::seq< internal::rules::ident, pegtl::eof >;
      pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl::eol::lf_crlf, const char* > in( n, __FUNCTION__ );
      return pegtl::parse< grammar >( in );
   }

   [[nodiscard]] inline std::string name_to_string( const std::string& n )
   {
      return is_identifier( n ) ? n : ( '"' + json::internal::escape( n ) + '"' );
   }

   [[nodiscard]] inline std::string to_string( const key_part& t )
   {
      switch( t.kind() ) {
         case key_kind::name:
            return name_to_string( t.get_name() );
         case key_kind::index:
            return std::to_string( t.get_index() );
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   inline void name_to_stream( std::ostream& o, const std::string& n )
   {
      if( is_identifier( n ) ) {
         o << n;
      }
      else {
         o << '"' << json::internal::escape( n ) << '"';
      }
   }

   inline void to_stream( std::ostream& o, const key_part& t )
   {
      switch( t.kind() ) {
         case key_kind::name:
            name_to_stream( o, t.get_name() );
            return;
         case key_kind::index:
            o << t.get_index();
            return;
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

}  // namespace tao::config

#endif
