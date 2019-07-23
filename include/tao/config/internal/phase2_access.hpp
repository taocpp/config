// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP

#include <stdexcept>

#include "../access.hpp"
#include "../key.hpp"

#include "entry.hpp"
#include "format.hpp"
#include "forward.hpp"
#include "reverse.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   struct temporary_non_local_return_hack {};  // TODO: Ensure this doesn't survive the first wave of refactoring after stabilising the semantics.

   inline const json_t& phase2_access_impl( const concat& l, const key& p );

   inline const json_t& phase2_access_impl( const entry_array& a, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::star:
            throw std::runtime_error( "attempt to reference star" );
         case part::minus:
            if( !a.empty() ) {
               return phase2_access_impl( a.back(), p );
            }
            throw std::runtime_error( "attempt to reference last entry of empty array" );
         case part::name:
            throw std::runtime_error( "attempt to reference string in array" );
         case part::index:
            if( a.size() > t.get_index() ) {
               return phase2_access_impl( a[ t.get_index() ], p );
            }
            throw std::runtime_error( "array reference out of bounds" );
      }
      assert( false );
   }

   inline const json_t& phase2_access_impl( const entry_object& o, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::star:
            throw std::runtime_error( "attempt to reference star" );
         case part::minus:
            throw std::runtime_error( "attempt to reference last entry object" );
         case part::name:
            if( const auto* i = o.find( t.get_name() ) ) {
               return phase2_access_impl( i->second, p );
            }
            throw std::runtime_error( "object reference not found" );
         case part::index:
            throw std::runtime_error( "attempt to reference index in object" );
      }
      assert( false );
   }

   inline const json_t& phase2_access_impl( const concat& l, const key& p )
   {
      if( l.size() == 1 ) {
         auto& e = l.entries().front();
         switch( e.type() ) {
            case entry::atom:
               if( p.empty() ) {
                  return e.get_atom();
               }
               return config::access( e.get_atom(), p );
            case entry::array:
               if( !p.empty() ) {
                  return phase2_access_impl( e.get_array(), p.front(), pop_front( p ) );
               }
               break;
            case entry::object:
               if( !p.empty() ) {
                  return phase2_access_impl( e.get_object(), p.front(), pop_front( p ) );
               }
               break;
            case entry::reference:
               break;
            case entry::nothing:
               assert( false );
         }
      }
      throw temporary_non_local_return_hack();  // Unsuitable thing found -- abort search and try again next iteration.
   }

   inline const json_t* phase2_access( const entry_object& o, const std::string& k, const key& p )
   {
      if( const auto* i = o.find( k ) ) {
         return &phase2_access_impl( i->second, p );  // No backtracking once we enter _impl.
      }
      return nullptr;  // Nothing found -- backtrack search in entry/concat tree.
   }

   inline const json_t* phase2_access( const pegtl::position& pos, const entry& e, const std::string& k, const key& p );

   inline const json_t* phase2_access( const pegtl::position& pos, const concat& l, const std::string& k, const key& p )
   {
      for( const auto& i : reverse( l.entries() ) ) {
         switch( i.type() ) {
            case entry::atom:
               throw pegtl::parse_error( "addition of atom and object detected", pos );
            case entry::array:
               throw pegtl::parse_error( "addition of array and object detected", pos );
            case entry::object:
               if( const json_t* c = phase2_access( i.get_object(), k, p ) ) {
                  return c;
               }
               break;
            case entry::reference:
               return nullptr;
            case entry::nothing:
               assert( false );
         }
      }
      return phase2_access( pos, l.parent(), k, p );
   }

   inline const json_t* phase2_access( const pegtl::position& pos, const entry& e, const std::string& k, const key& p )
   {
      switch( e.type() ) {
         case entry::atom:
            return &config::access( e.get_atom(), k + p );
         case entry::array:
            return phase2_access( pos, e.parent()->parent(), k, p );  // Top-level is an object, so e.parent() is always non-null for arrays.
         case entry::object:
            if( e.parent() ) {
               return phase2_access( pos, *e.parent(), k, p );
            }
            if( const json_t* c = phase2_access( e.get_object(), k, p ) ) {
               return c;
            }
            break;
         case entry::reference:
            assert( false );
            break;
         case entry::nothing:
            assert( false );
            break;
      }
      return nullptr;
   }

   inline const json_t* phase2_access( const pegtl::position& pos, const entry& e, const part& t, const key& p )
   {
      switch( t.type() ) {
         case part::name:
            return phase2_access( pos, e, t.get_name(), p );
         case part::index:
            assert( false );
         case part::star:
            assert( false );
         case part::minus:
            assert( false );
      }
      assert( false );
   }

   inline const json_t* phase2_access( const entry& e, const key& p )
   {
      assert( !p.empty() );

      try {
         return phase2_access( e.position(), e, p.front(), pop_front( p ) );
      }
      catch( const temporary_non_local_return_hack& ) {
         return nullptr;
      }
   }

}  // namespace tao::config::internal

#endif
