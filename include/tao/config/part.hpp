// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_PART_HPP
#define TAO_CONFIG_PART_HPP

#include <cassert>
#include <ostream>
#include <string>

#include "internal/grammar.hpp"

namespace tao
{
   namespace config
   {
      class part
      {
      public:
         enum kind : char
         {
            name,
            index,
            star,
            minus
         };

         explicit
         part( const kind t )
            : m_type( t )
         {
            assert( ( t == star ) || ( t == minus ) );
         }

         explicit
         part( const std::uint64_t i )
            : m_type( kind::index ),
              m_index( i )
         {
         }

         explicit
         part( const std::string& n )
            : m_type( name ),
              m_name( n )
         {
         }

         kind type() const noexcept
         {
            return m_type;
         }

         std::uint64_t get_index() const noexcept
         {
            assert( m_type == index );

            return m_index;
         }

         const std::string& get_name() const noexcept
         {
            assert( m_type == name );

            return m_name;
         }

      private:
         kind m_type;
         // TODO: Change m_index to signed type (change semantics of minus).
         // TODO: Use std::variant or union for the following data members.
         std::uint64_t m_index;
         std::string m_name;
      };

      inline bool is_alpha( const int c )
      {
         return ( ( 'a' <= c ) && ( c <= 'z' ) ) || ( ( 'A' <= c ) && ( c <= 'Z' ) );
      }

      inline bool is_first( const int c )
      {
         return is_alpha( c ) || ( c == '_' );
      }

      inline bool is_identifier( const std::string& n )
      {
         using grammar = json_pegtl::seq< internal::rules::identifier, json_pegtl::eof >;
         json_pegtl::memory_input< json_pegtl::tracking_mode::lazy, json_pegtl::eol::lf_crlf, const char* > in( n, __FUNCTION__ );
         return json_pegtl::parse< grammar >( in );
      }

      inline std::string name_to_string( const std::string& n )
      {
         if( is_identifier( n ) ) {
            return n;
         }
         return '"' + json::internal::escape( n ) + '"';
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

   }  // namespace config

}  // namespace tao

#endif
