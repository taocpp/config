// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_ACCESS_HPP

#include <stdexcept>

#include "pointer.hpp"
#include "utility.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline const concat& access( const concat& l, const pointer& p );

         inline const concat& access_name( const concat& l, const std::string& k, const pointer& p )
         {
            for( const auto& i : reverse( l.v ) ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto j = i.get_object().find( k );

               if( j != i.get_object().end() ) {
                  return access( j->second, p );
               }
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline const concat& access_index( const concat& l, std::size_t n, const pointer& p )
         {
            for( const auto& i : l.v ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               const auto s = i.get_array().size();

               if( n < s ) {
                  return access( i.get_array()[ n ], p );
               }
               n -= s;
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline const concat& access_minus( const concat& l, const pointer& p )
         {
            for( const auto& i : reverse( l.v ) ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
               }
               if( !i.get_array().empty() ) {
                  return access( i.get_array().back(), p );
               }
            }
            throw std::runtime_error( std::string( __FILE__ ) + ":" + std::to_string( __LINE__ ) );
         }

         inline const concat& access( const concat& l, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::NAME:
                  return access_name( l, t.name(), p );
               case token::INDEX:
                  return access_index( l, t.index(), p );
               case token::STAR:
                  assert( false );
               case token::MINUS:
                  return access_minus( l, p );
            }
            assert( false );
         }

         inline const concat& access( const concat& l, const pointer& p )
         {
            if( p.empty() ) {
               return l;
            }
            return access( l, p.front(), pop_front( p ) );
         }

         inline const concat& access( const object_t& o, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::NAME:
                  return access( o.at( t.name() ), p );  // TODO: Proper exception message on key-not-found.
               case token::INDEX:
                  assert( false );
               case token::STAR:
                  assert( false );
               case token::MINUS:
                  assert( false );
            }
            assert( false );
         }

         inline const concat& access( const object_t& o, const pointer& p )
         {
            assert( !p.empty() );

            return access( o, p.front(), pop_front( p ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
