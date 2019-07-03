// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_ACCESS_HPP

#include <iterator>
#include <stdexcept>

#include "../key.hpp"

#include "entry.hpp"
#include "format.hpp"
#include "state.hpp"
#include "utility.hpp"

namespace tao::config::internal
{
   inline const concat& access_impl( const position& pos, const concat& l, const key& p );

   inline const concat& access_name( const position& pos, const concat& l, const std::string& k, const key& p )
   {
      for( const auto& i : reverse( l.entries() ) ) {
         if( !i.is_object() ) {
            throw pegtl::parse_error( format( "attempt to index non-object with string", { { "string", k }, { "non-object", { &i.position(), i.type() } } } ), pos );
         }
         const auto j = i.get_object().find( k );

         if( j != i.get_object().end() ) {
            return access_impl( pos, j->second, p );
         }
      }
      throw pegtl::parse_error( format( "object index not found", { { "string", k }, { "object", { &l.p } } } ), pos );
   }

   inline const concat& access_index( const position& pos, const concat& l, std::size_t n, const key& p )
   {
      for( const auto& i : l.entries() ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( "attempt to index non-array with integer", { { "integer", n }, { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         const auto s = i.get_array().size();

         if( n < s ) {
            auto j = i.get_array().begin();
            std::advance( j, n );
            return access_impl( pos, *j, p );
         }
         n -= s;
      }
      throw pegtl::parse_error( format( "array index out of range", { { "integer", n }, { "array", { &l.p } } } ), pos );
   }

   inline const concat& access_minus( const position& pos, const concat& l, const key& p )
   {
      for( const auto& i : reverse( l.entries() ) ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( "attempt to access last element in non-array", { { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         if( !i.get_array().empty() ) {
            return access_impl( pos, i.get_array().back(), p );
         }
      }
      throw pegtl::parse_error( format( "array has no last element to access", { { "array", { &l.p } } } ), pos );
   }

   inline const concat& access_impl( const position& pos, const concat& l, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return access_name( pos, l, t.get_name(), p );
         case part::index:
            return access_index( pos, l, t.get_index(), p );
         case part::star:
            throw pegtl::parse_error( "attempt to access star", pos );
         case part::minus:
            return access_minus( pos, l, p );
      }
      assert( false );
   }

   inline const concat& access_impl( const position& pos, const concat& l, const key& p )
   {
      if( p.empty() ) {
         return l;
      }
      return access_impl( pos, l, p.front(), pop_front( p ) );
   }

   inline const concat* access( const position& pos, const object_t& o, const std::string& k, const key& p )
   {
      const auto i = o.find( k );

      if( i != o.end() ) {
         return &access_impl( pos, i->second, p );
      }
      return nullptr;
   }

   inline const concat& access( const position& pos, const entry& e, const std::string& k, const key& p );

   inline const concat& access( const position& pos, const concat& l, const std::string& k, const key& p )
   {
      for( const auto& i : reverse( l.entries() ) ) {
         switch( i.type() ) {
            case entry::atom:
               throw pegtl::parse_error( "addition of atom and object detected", pos );
            case entry::array:
               throw pegtl::parse_error( "addition of array and object detected", pos );
            case entry::object:
               if( const auto* c = access( pos, i.get_object(), k, p ) ) {
                  return *c;
               }
               break;
            case entry::reference:
               throw pegtl::parse_error( format( "object index access across reference", { { "string", k } } ), pos );  // Alternatively print a warning and continue?
            case entry::nothing:
               assert( false );
               break;
         }
      }
      return access( pos, l.parent(), k, p );
   }

   inline const concat& access( const position& pos, const entry& e, const std::string& k, const key& p )
   {
      switch( e.type() ) {
         case entry::atom:
            assert( false );
            break;
         case entry::array:
            return access( pos, e.parent()->parent(), k, p );  // Top-level is an object, so e.parent() is always non-null for arrays.
         case entry::object:
            if( e.parent() ) {
               return access( pos, *e.parent(), k, p );
            }
            else if( const auto* c = access( pos, e.get_object(), k, p ) ) {
               return *c;
            }
            break;
         case entry::reference:
            assert( false );
            break;
         case entry::nothing:
            assert( false );
            break;
      }
      throw pegtl::parse_error( format( "object index not found", { { "string", k }, e.type() } ), pos );
   }

   inline const concat& access( const position& pos, const entry& e, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return access( pos, e, t.get_name(), p );
         case part::index:
            assert( false );
            break;
         case part::star:
            assert( false );
            break;
         case part::minus:
            assert( false );
            break;
      }
      assert( false );
   }

   inline const concat& access( const position& pos, const entry& e, const key& p )
   {
      assert( !p.empty() );

      return access( pos, e, p.front(), pop_front( p ) );
   }

}  // namespace tao::config::internal

#endif
