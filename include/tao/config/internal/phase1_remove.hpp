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

namespace tao::config::internal
{
   inline void phase1_remove_star( concat& c )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "TODO" );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore? -- Probably ignore!
            case entry_kind::array:
               e.get_array().array.clear();
               continue;
            case entry_kind::object:
               e.get_object().object.clear();
               continue;
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_remove_minus( concat& c )
   {
      assert( false );  // UNREACHABLE
   }

   inline void phase1_remove_name( concat& c, const std::string& k )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "TODO" );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore?
            case entry_kind::array:
               throw std::string( "TODO" );
            case entry_kind::object:
               e.get_object().object.erase( k );
               continue;
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_remove_index( concat& c, const std::size_t n )
   {
      assert( false );  // UNREACHABLE
   }

   inline void phase1_remove( concat& c, const key1_part& part )
   {
      switch( part.kind() ) {
         case key1_kind::star:
            phase1_remove_star( c );
            return;
         case key1_kind::minus:
            phase1_remove_minus( c );
            return;
         case key1_kind::name:
            phase1_remove_name( c, part.get_name() );
            return;
         case key1_kind::index:
            phase1_remove_index( c, part.get_index() );
            return;
         case key1_kind::append:
            throw std::string( "TODO -- What?" );
      }
      assert( false );  // UNREACHABLE
   }

   inline void phase1_remove( concat& c, const key1& path );
   inline void phase1_remove( concat& c, const key1_part& part, const key1& path );

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
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_remove_minus( concat& c, const key1& path )
   {
      assert( false );
   }

   inline void phase1_remove_name( concat& c, const std::string& k, const key1& path )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "TODO" );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore?
            case entry_kind::array:
               throw std::string( "TODO" );
            case entry_kind::object:
               if( auto* p = e.get_object().find( k ) ) {
                  phase1_remove( p->second, path );
               }
               continue;
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_remove_index( concat& c, const std::size_t n, const key1& path )
   {
      assert( false );
   }

   inline void phase1_remove( concat& c, const key1_part& part, const key1& path )
   {
      assert( !path.empty() );

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
            throw std::string( "TODO -- What?" );
      }
      assert( false );  // UNREACHABLE
   }

   inline void phase1_remove( concat& c, const key1& path )
   {
      assert( !path.empty() );

      if( path.size() == 1 ) {
         phase1_remove( c, path.at( 0 ) );
      }
      else {
         phase1_remove( c, path.front(), pop_front( path ) );
      }
   }

   inline void phase1_remove( object& o, const key1& path )
   {
      assert( !path.empty() );

      if( path.size() == 1 ) {
         o.object.erase( path.at( 0 ).get_name() );
      }
      else if( auto* p = o.find( path.at( 0 ).get_name() ) ) {
         phase1_remove( p->second, pop_front( path ) );
      }
   }

}  // namespace tao::config::internal

#endif
