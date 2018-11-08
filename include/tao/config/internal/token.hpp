// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TOKEN_HPP
#define TAO_CONFIG_INTERNAL_TOKEN_HPP

#include <cassert>
#include <ostream>
#include <string>

#include "grammar.hpp"
#include "traits.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         class token
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
            token( const kind t )
               : m_type( t )
            {
               assert( ( t == star ) || ( t == minus ) );
            }

            explicit
            token( const std::size_t i )
               : m_type( kind::index ),
                 m_index( i )
            {
            }

            explicit
            token( const std::string& n )
               : m_type( name ),
                 m_name( n )
            {
            }

            kind type() const noexcept
            {
               return m_type;
            }

            std::size_t get_index() const noexcept
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

            // Only one of the following is valid, but that is accepatble for
            // the moment because this is a config reader, not HPC, and it is
            // only in the internal representation, not exposed to the user.
            std::size_t m_index;
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
            json_pegtl::memory_input< json_pegtl::tracking_mode::lazy, json_pegtl::eol::lf_crlf, const char* > in( n, __FUNCTION__ );
            return json_pegtl::parse< rules::ident >( in );
         }

         inline std::string name_to_string( const std::string& n )
         {
            if( is_identifier( n ) ) {
               return n;
            }
            return '"' + json::internal::escape( n ) + '"';
         }

         inline std::string token_to_string( const token& t )
         {
            switch( t.type() ) {
               case token::name:
                  return name_to_string( t.get_name() );
               case token::index:
                  return std::to_string( t.get_index() );
               case token::star:
                  return "*";
               case token::minus:
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

         inline void token_to_stream( std::ostream& o, const token& t )
         {
            switch( t.type() ) {
               case token::name:
                  name_to_stream( o, t.get_name() );
                  return;
               case token::index:
                  o << t.get_index();
                  return;
               case token::star:
                  o << '*';
                  return;
               case token::minus:
                  o << '-';
                  return;
            }
            assert( false );
         }

         template<>
         struct traits< token >
         {
            template< template< typename... > class, typename Consumer >
            static void produce( Consumer& c, const token& t )
            {
               switch( t.type() ) {
                  case token::name:
                     c.string( t.get_name() );
                     return;
                  case token::index:
                     c.number( std::uint64_t( t.get_index() ) );
                     return;
                  case token::star:
                     c.string( "*" );
                     return;
                  case token::minus:
                     c.string( "-" );
                     return;
               }
               assert( false );
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
