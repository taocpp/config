// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ASSIGN_HPP
#define TAO_CONFIG_INTERNAL_ASSIGN_HPP

#include <stdexcept>

#include "format.hpp"
#include "pegtl.hpp"
#include "pointer.hpp"
#include "utility.hpp"
#include "entry.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline concat& assign( const position& pos, concat& l, const pointer& p );

         inline concat& assign_name( const position& pos, concat& l, const std::string& k, const pointer& p )
         {
            for( auto& i : reverse( l.v ) ) {
               if( !i.is_object() ) {
                  throw std::runtime_error( format( "attempt to index non-object with string", { &pos, { "string", k }, { "non-object", { &i.position(), i.type() } } } ) );
               }
               const auto j = i.get_object().find( k );

               if( j != i.get_object().end() ) {
                  return assign( pos, j->second, p );
               }
            }
            if( l.v.empty() ) {
               l.v.emplace_back( entry::make_object( pos ) );
            }
            return assign( pos, l.v.back().get_object().try_emplace( k, pos ).first->second, p );
         }

         inline concat& assign_index( const position& pos, concat& l, std::size_t n, const pointer& p )
         {
            for( auto& i : l.v ) {
               if( !i.is_array() ) {
                  throw std::runtime_error( format( "attempt to index non-array with integer", { &pos, { "integer", n }, { "non-object", { &i.position(), i.type() } } } ) );
               }
               const auto s = i.get_array().size();

               if( n < s ) {
                  return assign( pos, i.get_array()[ n ], p );
               }
               n -= s;
            }
            throw std::runtime_error( format( "array index out of range", { &pos, { "integer", n } } ) );
         }

         inline concat& assign_minus( const position& pos, concat& l, const pointer& p )
         {
            if( l.v.empty() ) {
               l.v.emplace_back( entry::make_array( pos ) );
            }
            return assign( pos, l.v.back().get_array().emplace_back( pos ), p );
         }

         inline concat& assign( const position& pos, concat& l, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::name:
                  return assign_name( pos, l, t.get_name(), p );
               case token::index:
                  return assign_index( pos, l, t.get_index(), p );
               case token::star:
                  throw std::runtime_error( format( "attempt to assign to star", { &pos } ) );
               case token::minus:
                  return assign_minus( pos, l, p );
            }
            assert( false );
         }

         inline concat& assign( const position& pos, concat& l, const pointer& p )
         {
            if( p.empty() ) {
               l.p = pos;
               return l;
            }
            return assign( pos, l, p.front(), pop_front( p ) );
         }

         inline concat& assign( const position& pos, object_t& o, const token& t, const pointer& p )
         {
            switch( t.type() ) {
               case token::name:
                  return assign( pos, o.try_emplace( t.get_name(), pos ).first->second, p );
               case token::index:
                  assert( false );
               case token::star:
                  assert( false );
               case token::minus:
                  assert( false );
            }
            assert( false );
         }

         inline concat& assign( const position& pos, object_t& o, const pointer& p )
         {
            assert( !p.empty() );

            return assign( pos, o, p.front(), pop_front( p ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
