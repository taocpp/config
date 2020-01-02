// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_ACCESS_HPP

#include <stdexcept>

#include "../key.hpp"

#include "entry.hpp"
#include "format.hpp"
#include "reverse.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   inline const concat& phase1_access_impl( const pegtl::position& pos, const concat& l, const key& p );

   inline const concat& phase1_access_name( const pegtl::position& pos, const concat& l, const std::string& k, const key& p )
   {
      for( const auto& i : reverse( l.entries() ) ) {
         if( !i.is_object() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index non-object with string", { { "string", k }, { "non-object", { &i.position(), i.type() } } } ), pos );
         }
         if( const auto* j = i.get_object().find( k ) ) {
            return phase1_access_impl( pos, j->second, p );
         }
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "object index not found", { { "string", k }, { "object", { &l.p } } } ), pos );
   }

   inline const concat& phase1_access_index( const pegtl::position& pos, const concat& l, std::size_t n, const key& p )
   {
      for( const auto& i : l.entries() ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index non-array with integer", { { "integer", n }, { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         const auto s = i.get_array().size();

         if( n < s ) {
            const auto& j = i.get_array()[ n ];
            return phase1_access_impl( pos, j, p );
         }
         n -= s;
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "array index out of range", { { "integer", n }, { "array", { &l.p } } } ), pos );
   }

   inline const concat& phase1_access_minus( const pegtl::position& pos, const concat& l, const key& p )
   {
      for( const auto& i : reverse( l.entries() ) ) {
         if( !i.is_array() ) {
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to access last element in non-array", { { "non-array", { &i.position(), i.type() } } } ), pos );
         }
         if( !i.get_array().empty() ) {
            return phase1_access_impl( pos, i.get_array().back(), p );
         }
      }
      throw pegtl::parse_error( format( __FILE__, __LINE__, "array has no last element to access", { { "array", { &l.p } } } ), pos );
   }

   inline const concat& phase1_access_impl( const pegtl::position& pos, const concat& l, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return phase1_access_name( pos, l, t.get_name(), p );
         case part::index:
            return phase1_access_index( pos, l, t.get_index(), p );
         case part::star:
            throw pegtl::parse_error( "attempt to access star", pos );
         case part::minus:
            return phase1_access_minus( pos, l, p );
      }
      assert( false );
   }

   inline const concat& phase1_access_impl( const pegtl::position& pos, const concat& l, const key& p )
   {
      if( p.empty() ) {
         return l;
      }
      return phase1_access_impl( pos, l, p.front(), pop_front( p ) );
   }

   inline const concat* phase1_access( const pegtl::position& pos, const entry_object& o, const std::string& k, const key& p )
   {
      if( const auto* i = o.find( k ) ) {
         return &phase1_access_impl( pos, i->second, p );
      }
      return nullptr;
   }

   inline const concat& phase1_access( const pegtl::position& pos, const entry& e, const std::string& k, const key& p );

   inline const concat& phase1_access( const pegtl::position& pos, const concat& l, const std::string& k, const key& p )
   {
      for( const auto& i : reverse( l.entries() ) ) {
         switch( i.type() ) {
            case entry::atom:
               throw pegtl::parse_error( "addition of atom and object detected", pos );
            case entry::array:
               throw pegtl::parse_error( "addition of array and object detected", pos );
            case entry::object:
               if( const auto* c = phase1_access( pos, i.get_object(), k, p ) ) {
                  return *c;
               }
               break;
            case entry::reference:
               // TODO: Print a warning and continue instead?
               throw pegtl::parse_error( format( __FILE__, __LINE__, "object index access across reference", { { "string", k } } ), pos );
            case entry::nothing:
               assert( false );
               break;
         }
      }
      return phase1_access( pos, l.parent(), k, p );
   }

   inline const concat& phase1_access( const pegtl::position& pos, const entry& e, const std::string& k, const key& p )
   {
      switch( e.type() ) {
         case entry::atom:
            assert( false );
            break;
         case entry::array:
            return phase1_access( pos, e.parent()->parent(), k, p );  // Top-level is an object, so e.parent() is always non-null for arrays.
         case entry::object:
            if( e.parent() ) {
               return phase1_access( pos, *e.parent(), k, p );
            }
            if( const auto* c = phase1_access( pos, e.get_object(), k, p ) ) {
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
      throw pegtl::parse_error( format( __FILE__, __LINE__, "object index not found", { { "string", k }, e.type() } ), pos );
   }

   inline const concat& phase1_access( const pegtl::position& pos, const entry& e, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return phase1_access( pos, e, t.get_name(), p );
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

   inline const concat& phase1_access( const pegtl::position& pos, const entry& e, const key& p )
   {
      assert( !p.empty() );

      return phase1_access( pos, e, p.front(), pop_front( p ) );
   }

}  // namespace tao::config::internal

#endif
