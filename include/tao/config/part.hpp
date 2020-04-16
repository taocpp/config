// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PART_HPP
#define TAO_CONFIG_PART_HPP

#include <cassert>
#include <ostream>
#include <string>
#include <variant>

#include "internal/grammar.hpp"

namespace tao::config
{
   class part
   {
   public:
      enum kind : char
      {
         star = 0,
         minus = 1,
         name = 2,
         index = 3
      };

      struct star_t
      {};

      struct minus_t
      {};

      explicit part( const star_t t ) noexcept
         : m_data( t )
      {}

      explicit part( const minus_t t ) noexcept
         : m_data( t )
      {}

      explicit part( const std::uint64_t i ) noexcept
         : m_data( i )
      {}

      explicit part( const std::string& n )
         : m_data( n )
      {}

      explicit part( std::string&& n ) noexcept
         : m_data( std::move( n ) )
      {}

      kind type() const noexcept
      {
         return kind( m_data.index() );
      }

      std::uint64_t get_index() const noexcept
      {
         const std::uint64_t* s = std::get_if< std::uint64_t >( &m_data );
         assert( s != nullptr );
         return *s;
      }

      const std::string& get_name() const noexcept
      {
         const std::string* s = std::get_if< std::string >( &m_data );
         assert( s != nullptr );
         return *s;
      }

   private:
      std::variant< star_t, minus_t, std::string, std::uint64_t > m_data;
   };

   constexpr bool is_alpha( const int c ) noexcept
   {
      return ( ( 'a' <= c ) && ( c <= 'z' ) ) || ( ( 'A' <= c ) && ( c <= 'Z' ) );
   }

   constexpr bool is_first( const int c ) noexcept
   {
      return is_alpha( c ) || ( c == '_' );
   }

   inline bool is_identifier( const std::string& n )
   {
      using grammar = pegtl::seq< internal::rules::identifier, pegtl::eof >;
      pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl::eol::lf_crlf, const char* > in( n, __FUNCTION__ );
      return pegtl::parse< grammar >( in );
   }

   inline std::string name_to_string( const std::string& n )
   {
      return is_identifier( n ) ? n : ( '"' + json::internal::escape( n ) + '"' );
   }

   inline std::string to_string( const part& t )
   {
      switch( t.type() ) {
         case part::name:
            return name_to_string( t.get_name() );
         case part::index:
            return std::to_string( t.get_index() );
         case part::star:
            return "*";
         case part::minus:
            return "-";
      }
      assert( false );
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

   inline void to_stream( std::ostream& o, const part& t )
   {
      switch( t.type() ) {
         case part::name:
            name_to_stream( o, t.get_name() );
            return;
         case part::index:
            o << t.get_index();
            return;
         case part::star:
            o << '*';
            return;
         case part::minus:
            o << '-';
            return;
      }
      assert( false );
   }

}  // namespace tao::config

#endif
