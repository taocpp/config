// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ASSIGN_HPP
#define TAO_CONFIG_INTERNAL_ASSIGN_HPP

#include <iterator>
#include <stdexcept>

#include "../key.hpp"

#include "entry.hpp"
#include "format.hpp"
#include "pegtl.hpp"
#include "utility.hpp"

namespace tao::config::internal
{
   inline concat& assign( const position& pos, concat& l, const key& p );

   inline concat& assign_name( const position& pos, concat& l, const std::string& k, const key& p )
   {
      for( auto& i : reverse( l.private_entries() ) ) {
         if( !i.is_object() ) {
            throw pegtl::parse_error( format( "attempt to index non-object with string", { { "string", k }, { "non-object", { &i.position(), i.type() } } } ), pos );
         }
         const auto j = i.get_object().find( k );

         if( j != i.get_object().end() ) {
            return assign( pos, j->second, p );
         }
      }
      if( l.entries().empty() ) {
         l.emplace_back_object( pos );
      }
      return assign( pos, l.private_entries().back().emplace( k, pos ), p );
   }

   inline concat& assign_minus( const position& pos, concat& l, const key& p )
   {
      if( l.entries().empty() ) {
         l.emplace_back_array( pos );
      }
      else if( !l.entries().back().is_array() ) {
         throw pegtl::parse_error( format( "attempt to append to non-array", { { "non-array", { &l.entries().back().position(), l.entries().back().type() } } } ), pos );
      }
      return assign( pos, l.private_entries().back().emplace_back( pos ), p );
   }

   inline concat& assign_index( const position& pos, concat& l, const std::size_t m, const key& p )
   {
      std::size_t n = m;

      for( auto& i : l.private_entries() ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( "attempt to index non-array with integer", { { "integer", m }, { "partial", n }, { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         const auto s = i.get_array().size();

         if( n < s ) {
            auto j = i.get_array().begin();
            std::advance( j, n );
            return assign( pos, *j, p );
         }
         n -= s;
      }
      if( n == 0 ) {
         return assign_minus( pos, l, p );
      }
      throw pegtl::parse_error( format( "array index out of range", { { "integer", m }, { "partial", n } } ), pos );
   }

   inline concat& assign( const position& pos, concat& l, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return assign_name( pos, l, t.get_name(), p );
         case part::index:
            return assign_index( pos, l, t.get_index(), p );
         case part::star:
            throw pegtl::parse_error( "attempt to assign to star", pos );
         case part::minus:
            return assign_minus( pos, l, p );
      }
      assert( false );
   }

   inline concat& assign( const position& pos, concat& l, const key& p )
   {
      if( p.empty() ) {
         l.p = pos;
         return l;
      }
      return assign( pos, l, p.front(), pop_front( p ) );
   }

   inline concat& assign( const position& pos, entry& e, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return assign( pos, e.emplace( t.get_name(), pos ), p );
         case part::index:
            assert( false );
         case part::star:
            assert( false );
         case part::minus:
            assert( false );
      }
      assert( false );
   }

   inline concat& assign( const position& pos, entry& e, const key& p )
   {
      assert( !p.empty() );

      return assign( pos, e, p.front(), pop_front( p ) );
   }

}  // namespace tao::config::internal

#endif
