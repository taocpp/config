// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP

#include <stdexcept>

#include "../access.hpp"
#include "../key.hpp"

#include "entry.hpp"
#include "format.hpp"
#include "forward.hpp"
#include "pegtl.hpp"
#include "reverse.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   struct temporary_non_local_return_hack
   {};  // TODO: Ensure this doesn't survive the first wave of refactoring after stabilising the semantics.

   inline const json_t& phase2_access_down( const pegtl::position& pos, const concat& l, const key& p );

   inline const json_t& phase2_access_down( const pegtl::position& pos, const entry_array& a, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::star:
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index array with star", { { "array", &a.position() } } ), pos );
         case part::minus:
            if( !a.empty() ) {
               return phase2_access_down( pos, a.back(), p );
            }
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to reference last entry of empty array", { { "array", &a.position() } } ), pos );
         case part::name:
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to reference string in array", { { "array", &a.position() }, { "string", t.get_name() } } ), pos );
         case part::index:
            if( a.size() > t.get_index() ) {
               return phase2_access_down( pos, a[ t.get_index() ], p );
            }
            throw pegtl::parse_error( format( __FILE__, __LINE__, "array reference out of bounds", { { "array", &a.position() }, { "integer", t.get_index() }, { "size", a.size() } } ), pos );
      }
      assert( false );
   }

   inline const json_t& phase2_access_down( const pegtl::position& pos, const entry_object& o, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::star:
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to index object with star", { { "object", &o.position() } } ), pos );
         case part::minus:
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to reference last entry in object", { { "object", &o.position() } } ), pos );
         case part::name:
            if( const auto* i = o.find( t.get_name() ) ) {
               return phase2_access_down( pos, i->second, p );
            }
            throw pegtl::parse_error( format( __FILE__, __LINE__, "object reference not found", { { "object", &o.position() }, { "string", t.get_name() } } ), pos );
         case part::index:
            throw pegtl::parse_error( format( __FILE__, __LINE__, "attempt to reference integer in object", { { "object", &o.position() }, { "integer", t.get_index() } } ), pos );
      }
      assert( false );
   }

   inline const json_t& phase2_access_down( const pegtl::position& pos, const concat& l, const key& p )
   {
      if( l.size() == 1 ) {
         const entry& e = l.entries().front();
         switch( e.type() ) {
            case entry::atom:
               if( p.empty() ) {
                  return e.get_atom();
               }
               return config::access( e.get_atom(), p );
            case entry::array:
               if( !p.empty() ) {
                  return phase2_access_down( pos, e.get_array(), p.front(), pop_front( p ) );
               }
               break;
            case entry::object:
               if( !p.empty() ) {
                  return phase2_access_down( pos, e.get_object(), p.front(), pop_front( p ) );
               }
               break;
            case entry::reference:
               break;
            case entry::nothing:
               assert( false );
         }
      }
      throw temporary_non_local_return_hack();  // Unsuitable concat found -- abort search and try again next iteration.
   }

   inline const json_t* phase2_access_up( const pegtl::position& pos, const entry& e, const std::string& k, const key& p );

   inline const json_t* phase2_access_up( const pegtl::position& pos, const concat& l, const std::string& k, const key& p )
   {
      std::vector< const json_t* > j;
      std::vector< const concat* > r;

      for( const auto& i : reverse( l.entries() ) ) {
         switch( i.type() ) {
            case entry::atom:
               if( const json_t& a = i.get_atom(); a.is_object() ) {
                  if( const json_t* b = a.find( k ) ) {
                     j.emplace_back( b );
                  }
                  if( i.clear() ) {
                     break;
                  }
                  continue;
               }
               throw pegtl::parse_error( format( __FILE__, __LINE__, "addition of object and json detected", { { "json", &i.get_atom().position }, { "type", i.get_atom().type() } } ), l.p );
            case entry::array:
               throw pegtl::parse_error( format( __FILE__, __LINE__, "addition of object and array detected", { { "array", &i.get_array().position() } } ), l.p );
            case entry::object:
               if( const auto* j = i.get_object().find( k ) ) {
                  r.emplace_back( &( j->second ) );
               }
               if( i.clear() ) {
                  break;
               }
               continue;
            case entry::reference:
               return nullptr;
            case entry::nothing:
               assert( false );
               break;
         }
         break;
      }
      if( j.size() + r.size() == 1 ) {
         if( j.empty() ) {
            return &phase2_access_down( pos, *r[ 0 ], p );
         }
         return &config::access( *j[ 0 ], p );
      }
      if( j.size() + r.size() >= 2 ) {
         return nullptr;  // TODO - what?
      }
      // Nothing found, look further up the alternating entry-concat-tree.
      return phase2_access_up( pos, l.parent(), k, p );
   }

   inline const json_t* phase2_access_up( const pegtl::position& pos, const entry& e, const std::string& k, const key& p )
   {
      switch( e.type() ) {
         case entry::array:
            // Skip array because the first part of the key is (always) a string.
            // Top-level is an object, so e.parent() is always non-null for both
            // arrays and concats. We can skip the parent concat and jump straight
            // to the parent entry because we don't care about the siblings of our
            // array; if any of them happen to be end up as objects in any form
            // then that will be flagged as error later.
            return phase2_access_up( pos, e.parent()->parent(), k, p );
         case entry::object:
            if( e.parent() ) {
               // We are NOT at top-level, and want to take into consideration
               // the concat siblings of our object entry e when looking for k.
               return phase2_access_up( pos, *e.parent(), k, p );
            }
            // We are at top-level, take the last chance to find k right here.
            if( const auto* i = e.get_object().find( k ) ) {
               // No backtracking once we change direction.
               return &phase2_access_down( pos, i->second, p );
            }
            break;
         case entry::atom:
         case entry::reference:
         case entry::nothing:
            assert( false );
            break;
      }
      return nullptr;
   }

   inline const json_t* phase2_access( const pegtl::position& pos, const entry& e, const key& p )
   {
      assert( !p.empty() );

      try {
         return phase2_access_up( pos, e, p.front().get_name(), pop_front( p ) );
      }
      catch( const temporary_non_local_return_hack& ) {
         return nullptr;
      }
   }

}  // namespace tao::config::internal

#endif
