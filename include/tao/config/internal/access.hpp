// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_ACCESS_HPP

#include <stdexcept>

#include "format.hpp"
#include "pointer.hpp"
#include "utility.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline const concat& access( const position& pos, const concat& l, const pointer& p );

         inline const concat& access_name( const position& pos, const concat& l, const std::string& k, const pointer& p )
         {
            for( const auto& i : reverse( l.v ) ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( format( "attempt to index non-object with string", { &pos, { "string", k }, { "non-object", { &i.position(), i.type() } } } ) );
               }
               const auto j = i.get_object().find( k );

               if( j != i.get_object().end() ) {
                  return access( pos, j->second, p );
               }
            }
            throw std::runtime_error( format( "object index not found", { &pos, { "string", k }, { "object", { &l.p } } } ) );
         }

         inline const concat& access_index( const position& pos, const concat& l, std::size_t n, const pointer& p )
         {
            for( const auto& i : l.v ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( format( "attempt to index non-array with integer", { &pos, { "integer", n }, { "non-array", { &i.position(), i.type() } } } ) );
               }
               const auto s = i.get_array().size();

               if( n < s ) {
                  return access( pos, i.get_array()[ n ], p );
               }
               n -= s;
            }
            throw std::runtime_error( format( "array index out of range", { &pos, { "integer", n }, { "array", { &l.p } } } ) );
         }

         inline const concat& access_minus( const position& pos, const concat& l, const pointer& p )
         {
            for( const auto& i : reverse( l.v ) ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( format( "attempt to access last element in non-array", { &pos, { "non-array", { &i.position(), i.type() } } } ) );
               }
               if( !i.get_array().empty() ) {
                  return access( pos, i.get_array().back(), p );
               }
            }
            throw std::runtime_error( format( "array has no last element to access", { &pos, { "array", { &l.p } } } ) );
         }

         inline const concat& access( const position& pos, const concat& l, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::name:
                  return access_name( pos, l, t.get_name(), p );
               case token::index:
                  return access_index( pos, l, t.get_index(), p );
               case token::star:
                  throw std::runtime_error( format( "attempt to access star", { &pos } ) );
               case token::minus:
                  return access_minus( pos, l, p );
            }
            assert( false );
         }

         inline const concat& access( const position& pos, const concat& l, const pointer& p )
         {
            if( p.empty() ) {
               return l;
            }
            return access( pos, l, p.front(), pop_front( p ) );
         }

         inline const concat& access( const position& pos, const object_t& o, const std::string& k, const pointer& p )
         {
            const auto i = o.find( k );

            if( i != o.end() ) {
               return access( pos, i->second, p );
            }
            throw std::runtime_error( format( "object index not found at top-level", { &pos, { "string", k } } ) );
         }

         inline const concat& access( const position& pos, const object_t& o, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::name:
                  return access( pos, o, t.get_name(), p );
               case token::index:
                  assert( false );
               case token::star:
                  assert( false );
               case token::minus:
                  assert( false );
            }
            assert( false );
         }

         inline const concat& access( const position& pos, const object_t& o, const pointer& p )
         {
            assert( !p.empty() );

            return access( pos, o, p.front(), pop_front( p ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
