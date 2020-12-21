// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_DELETE_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_DELETE_HPP

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
   inline void phase1_delete_star( concat& c )
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

   inline void phase1_delete_minus( concat& c )
   {
      for( auto& e : reverse( c.concat ) ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "type error" );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore?
            case entry_kind::array:
               if( !e.get_array().array.empty() ) {
                  e.get_array().array.pop_back();
                  return;
               }
               continue;
            case entry_kind::object:
               throw std::string( "type error" );
         }
         assert( false );  // UNREACHABLE
      }
      throw std::string( "could not delete last element" );
   }

   inline void phase1_delete_name( concat& c, const std::string& k )
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

   inline void phase1_delete_index( concat& c, const std::size_t n )
   {
      assert( false );  // TODO
   }

   inline void phase1_delete_append( concat& c, const bool r )
   {
      if( !r ) {
         phase1_delete_minus( c );
      }
   }

   inline void phase1_delete( concat& c, const key1_part& part )
   {
      switch( part.kind() ) {
         case key1_kind::star:
            phase1_delete_star( c );
            return;
         case key1_kind::minus:
            phase1_delete_minus( c );
            return;
         case key1_kind::name:
            phase1_delete_name( c, part.get_name() );
            return;
         case key1_kind::index:
            phase1_delete_index( c, part.get_index() );
            return;
         case key1_kind::append:
            phase1_delete_append( c, part.get_append_flag() );
            return;
      }
      assert( false );  // UNREACHABLE
   }

   inline void phase1_delete( concat& c, const key1& path );
   inline void phase1_delete( concat& c, const key1_part& part, const key1& path );

   inline void phase1_delete_star( concat& c, const key1& path )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               continue;  // TODO: Error or ignore? -- Probably ignore!
            case entry_kind::reference:
               continue;  // TODO: Error or ignore? -- Probably ignore!
            case entry_kind::array:
               for( auto& d : e.get_array().array ) {
                  phase1_delete( d, path );
               }
               continue;
            case entry_kind::object:
               for( auto& p : e.get_object().object ) {
                  phase1_delete( p.second, path );
               }
               continue;
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_delete_minus( concat& c, const key1& path )
   {
      for( auto& e : reverse( c.concat ) ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "type error" );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore?
            case entry_kind::array:
               if( !e.get_array().array.empty() ) {
                  phase1_delete( e.get_array().array.back(), path );
                  return;
               }
               continue;
            case entry_kind::object:
               throw std::string( "type error" );
         }
         assert( false );  // UNREACHABLE
      }
      throw std::string( "could not delete last element" );
   }

   inline void phase1_delete_name( concat& c, const std::string& k, const key1& path )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "type error" );
            case entry_kind::reference:
               continue;  // TODO: Error or ignore?
            case entry_kind::array:
               throw std::string( "type error" );
            case entry_kind::object:
               if( auto* p = e.get_object().find( k ) ) {
                  phase1_delete( p->second, path );
               }
               continue;
         }
         assert( false );  // UNREACHABLE
      }
   }

   inline void phase1_delete_index( concat& c, const std::size_t n, const key1& path )
   {
      assert( false );  // TODO
   }

   inline void phase1_delete_append( concat& c, const bool r, const key1& path )
   {
      if( !r ) {
         phase1_delete_minus( c, path );
      }
   }

   inline void phase1_delete( concat& c, const key1_part& part, const key1& path )
   {
      assert( !path.empty() );

      switch( part.kind() ) {
         case key1_kind::star:
            phase1_delete_star( c, path );
            return;
         case key1_kind::minus:
            phase1_delete_minus( c, path );
            return;
         case key1_kind::name:
            phase1_delete_name( c, part.get_name(), path );
            return;
         case key1_kind::index:
            phase1_delete_index( c, part.get_index(), path );
            return;
         case key1_kind::append:
            phase1_delete_append( c, part.get_append_flag(), path );
            return;
      }
      assert( false );  // UNREACHABLE
   }

   inline void phase1_delete( concat& c, const key1& path )
   {
      assert( !path.empty() );

      if( path.size() == 1 ) {
         phase1_delete( c, path.at( 0 ) );
      }
      else {
         phase1_delete( c, path.front(), pop_front( path ) );
      }
   }

   inline void phase1_delete( object& o, const key1& path )
   {
      assert( !path.empty() );

      if( path.size() == 1 ) {
         o.object.erase( path.at( 0 ).get_name() );
      }
      else if( auto* p = o.find( path.at( 0 ).get_name() ) ) {
         phase1_delete( p->second, pop_front( path ) );
      }
   }

}  // namespace tao::config::internal

#endif
