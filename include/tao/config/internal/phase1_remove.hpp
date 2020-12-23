// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_REMOVE_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_REMOVE_HPP

#include <cassert>
#include <string>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "key1.hpp"
#include "object.hpp"
#include "reverse.hpp"

namespace tao::config::internal
{
   inline void phase1_remove( concat& c, const key1& path );

   inline void phase1_remove_star( concat& c, const key1& path )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               continue;  // TODO: Error or ignore? -- Probably ignore!
            case entry_kind::reference:
               continue;  // TODO: Error or ignore? -- Probably ignore!
            case entry_kind::array:
               for( auto& d : e.get_array().array ) {
                  phase1_remove( d, path );
               }
               continue;
            case entry_kind::object:
               for( auto& p : e.get_object().object ) {
                  phase1_remove( p.second, path );
               }
               continue;
            case entry_kind::remove:
               continue;  // TODO: Error or ignore? -- Probably ignore!
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_remove_minus( concat& c, const key1& path )
   {
      for( auto& e : reverse( c.concat ) ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "type error" ) + std::to_string( __LINE__ );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore?
            case entry_kind::array:
               if( !e.get_array().array.empty() ) {
                  phase1_remove( e.get_array().array.back(), path );
                  return;
               }
               continue;
            case entry_kind::object:
               throw std::string( "type error" ) + std::to_string( __LINE__ );
            case entry_kind::remove:
               throw std::string( "could not delete last element across delete" );
         }
         assert( false );  // UNREACHABLE
      }
      throw std::string( "could not delete last element" );
   }

   inline void phase1_remove_name( concat& c, const std::string& k, const key1& path )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "type error" ) + std::to_string( __LINE__ );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore?
            case entry_kind::array:
               throw std::string( "type error" ) + std::to_string( __LINE__ );
            case entry_kind::object:
               if( auto* p = e.get_object().find( k ) ) {
                  phase1_remove( p->second, path );
               }
               continue;
            case entry_kind::remove:
               continue;
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_remove_index( concat& c, const std::size_t n, const key1& path )
   {
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   inline void phase1_remove_append( concat& c, const bool r, const key1& path )
   {
      if( !r ) {
         phase1_remove_minus( c, path );
      }
   }

   inline void phase1_remove( concat& c, const key1_part& part, const key1& path )
   {
      switch( part.kind() ) {
         case key1_kind::star:
            phase1_remove_star( c, path );
            return;
         case key1_kind::minus:
            phase1_remove_minus( c, path );
            return;
         case key1_kind::name:
            phase1_remove_name( c, part.get_name(), path );
            return;
         case key1_kind::index:
            phase1_remove_index( c, part.get_index(), path );
            return;
         case key1_kind::append:
            phase1_remove_append( c, part.get_append_flag(), path );
            return;
      }
      assert( false );  // UNREACHABLE
   }

   inline void phase1_remove( concat& c, const key1& path )
   {
      if( path.empty() ) {
         c.concat.clear();
      }
      else {
         phase1_remove( c, path.front(), pop_front( path ) );
      }
   }

   inline void phase1_remove( object& o, const key1& path )
   {
      assert( !path.empty() );

      if( auto* p = o.find( path.at( 0 ).get_name() ) ) {
         phase1_remove( p->second, pop_front( path ) );
      }
   }

}  // namespace tao::config::internal

#endif
