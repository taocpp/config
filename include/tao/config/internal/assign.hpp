// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ASSIGN_HPP
#define TAO_CONFIG_INTERNAL_ASSIGN_HPP

#include <stdexcept>

#include "pointer.hpp"
#include "utility.hpp"
#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Input >
         list_t& assign( const Input& in, list_t& l, const pointer& p );

         template< typename Input >
         list_t& assign_name( const Input& in, list_t& l, const std::string& k, const pointer& p )
         {
            for( auto& i : reverse( l ) ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto j = i.get_object().find( k );

               if( j != i.get_object().end() ) {
                  return assign( in, j->second, p );
               }
            }
            if( l.empty() ) {
               l.emplace_back( entry::object( in ) );
            }
            return assign( in, l.back().get_object()[ k ], p );
         }

         template< typename Input >
         list_t& assign_index( const Input& in, list_t& l, std::size_t n, const pointer& p )
         {
            for( auto& i : l ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto s = i.get_array().size();

               if( n < s ) {
                  return assign( in, i.get_array()[ n ], p );
               }
               n -= s;
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         template< typename Input >
         list_t& assign_minus( const Input& in, list_t& l, const pointer& p )
         {
            if( l.empty() ) {
               l.emplace_back( entry::array( in ) );
            }
            return assign( in, l.back().get_array().emplace_back(), p );
         }

         template< typename Input >
         list_t& assign( const Input& in, list_t& l, const token& t, const pointer& p )
         {
            switch( t.t ) {
               case token::NAME:
                  return assign_name( in, l, t.k, p );
               case token::INDEX:
                  return assign_index( in, l, t.i, p );
               case token::STAR:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               case token::MINUS:
                  return assign_minus( in, l, p );
            }
            assert( false );
         }

         template< typename Input >
         list_t& assign( const Input& in, list_t& l, const pointer& p )
         {
            if( p.empty() ) {
               return l;
            }
            return assign( in, l, p.front(), pop_front( p ) );
         }

         template< typename Input >
         list_t& assign( const Input& in, object_t& o, const token& t, const pointer& p )
         {
            switch( t.t ) {
               case token::NAME:
                  return assign( in, o[ t.k ], p );
               case token::INDEX:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               case token::STAR:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               case token::MINUS:
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
            }
            assert( false );
         }

         template< typename Input >
         list_t& assign( const Input& in, object_t& o, const pointer& p )
         {
            assert( !p.empty() );

            return assign( in, o, p.front(), pop_front( p ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
