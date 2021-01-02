// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP

#include <cassert>
#include <string>

#include "array.hpp"
#include "concat.hpp"
#include "constants.hpp"
#include "entry.hpp"
#include "key1.hpp"
#include "object.hpp"
#include "reverse.hpp"

namespace tao::config::internal
{
   // Suffix access phase, back-tracking allowed for first part.

   [[nodiscard]] inline const concat* phase2_suffix_access( const concat& c, const key1& suffix, const bool first );

   [[nodiscard]] inline const concat* phase2_suffix_access_minus( const concat& c, const key1& suffix )
   {
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   [[nodiscard]] inline const concat* phase2_suffix_access_name( const concat& c, const std::string& name, const key1& suffix, const bool first )
   {
      bool done = false;  // TODO: Refactor this away.
      const concat* found = nullptr;

      for( const auto& e : reverse( c.concat ) ) {
         if( done ) {
            break;
         }
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "access name in value" );
            case entry_kind::reference:
               throw better_luck_next_time;
            case entry_kind::array:
               throw std::string( "access name in array" );
            case entry_kind::object:
               if( const auto i = e.get_object().object.find( name ); i != e.get_object().object.end() ) {
                  if( found != nullptr ) {
                     throw better_luck_next_time;
                  }
                  found = &i->second;
               }
               continue;
            case entry_kind::remove:
               done = true;
               continue;
         }
         assert( false );  // UNREACHABLE
      }
      if( found == nullptr ) {
         if( first ) {
            return nullptr;
         }
         throw std::string( "name not found" );  // NOTE: Just like all other throw std::string( ... ) this is just a temporary placeholder during refactoring.
      }
      return phase2_suffix_access( *found, suffix, false );
   }


   [[nodiscard]] inline const concat* phase2_suffix_access_index( const concat& c, const std::size_t index, const key1& suffix, const bool first )
   {
      std::size_t n = index;

      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "cannot index (across) value" );
            case entry_kind::reference:
               throw std::string( "cannot index (across) reference" );
            case entry_kind::array:
               if( e.get_array().array.size() > n ) {
                  auto i = e.get_array().array.begin();
                  std::advance( i, n );
                  return &*i;
               }
               n -= e.get_array().array.size();
               continue;
            case entry_kind::object:
               throw std::string( "cannot index (across) object" );
            case entry_kind::remove:
               n = index;
               continue;  // TODO: Skip to after the remove before iterating...
         }
      }
      throw std::string( "index out of range" );
   }

   [[nodiscard]] inline const concat* phase2_suffix_access( const concat& c, const key1_part& p, const key1& suffix, const bool first )
   {
      switch( p.kind() ) {
         case key1_kind::star:
            throw pegtl::parse_error( "unable to access star", p.position );
         case key1_kind::minus:
            return phase2_suffix_access_minus( c, suffix );
         case key1_kind::name:
            return phase2_suffix_access_name( c, p.get_name(), suffix, first );
         case key1_kind::index:
            return phase2_suffix_access_index( c, p.get_index(), suffix, first );
         case key1_kind::append:
            throw pegtl::parse_error( "this should be impossible", p.position );
      }
      assert( false );  // UNREACHABLE
   }

   [[nodiscard]] inline const concat* phase2_suffix_access( const concat& c, const key1& suffix, const bool first )
   {
      if( suffix.empty() ) {
         if( c.all_references() == 0 ) {
            return &c;
         }
         throw better_luck_next_time;
      }
      return phase2_suffix_access( c, suffix.at( 0 ), pop_front( suffix ), first );
   }

   // Prefix access phase, back-tracking allowed.

   [[nodiscard]] inline const concat* phase2_prefix_access( const concat& c, const key1& prefix, const key1& suffix );

   [[nodiscard]] inline const concat* phase2_prefix_access_minus( const concat& c, const key1& prefix, const key1& suffix )
   {
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   [[nodiscard]] inline const concat* phase2_prefix_access_name( const concat& c, const std::string& name, const key1& prefix, const key1& suffix )
   {
      bool done = false;  // TODO: Refactor this away.
      const concat* found = nullptr;

      for( const auto& e : reverse( c.concat ) ) {
         if( done ) {
            break;
         }
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "access name in value" );
            case entry_kind::reference:
               throw better_luck_next_time;
            case entry_kind::array:
               throw std::string( "access name in array" );
            case entry_kind::object:
               if( const auto i = e.get_object().object.find( name ); i != e.get_object().object.end() ) {
                  if( found != nullptr ) {
                     throw better_luck_next_time;
                  }
                  found = &i->second;
               }
               continue;
            case entry_kind::remove:
               done = true;
               continue;
         }
         assert( false );  // UNREACHABLE
      }
      if( found == nullptr ) {
         throw std::string( "name not found" );
      }
      return phase2_prefix_access( *found, prefix, suffix );
   }

   [[nodiscard]] inline const concat* phase2_prefix_access_index( const concat& c, const std::size_t index, const key1& prefix, const key1& suffix )
   {
      std::size_t n = index;

      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "cannot index (across) value" );
            case entry_kind::reference:
               throw std::string( "cannot index (across) reference" );
            case entry_kind::array:
               if( e.get_array().array.size() > n ) {
                  auto i = e.get_array().array.begin();
                  std::advance( i, n );
                  return phase2_prefix_access( *i, prefix, suffix );
               }
               n -= e.get_array().array.size();
               continue;
            case entry_kind::object:
               throw std::string( "cannot index (across) object" );
            case entry_kind::remove:
               n = index;
               continue;  // TODO: Skip to after the remove before iterating...
         }
      }
      throw std::string( "index out of range" );
   }

   [[nodiscard]] inline const concat* phase2_prefix_access( const concat& c, const key1_part& p, const key1& prefix, const key1& suffix )
   {
      switch( p.kind() ) {
         case key1_kind::star:
            throw pegtl::parse_error( "unable to access star", p.position );
         case key1_kind::minus:
            return phase2_prefix_access_minus( c, prefix, suffix );
         case key1_kind::name:
            return phase2_prefix_access_name( c, p.get_name(), prefix, suffix );
         case key1_kind::index:
            return phase2_prefix_access_index( c, p.get_index(), prefix, suffix );
         case key1_kind::append:
            throw pegtl::parse_error( "this should be impossible", p.position );
      }
      assert( false );  // UNREACHABLE
   }

   [[nodiscard]] inline const concat* phase2_prefix_access( const concat& c, const key1& prefix, const key1& suffix )
   {
      if( prefix.empty() ) {
         return phase2_suffix_access( c, suffix, true );
      }
      if( const concat* d =  phase2_prefix_access( c, prefix.at( 0 ), pop_front( prefix ), suffix ) ) {
         return d;
      }
      return phase2_suffix_access( c, suffix, true );
   }

   [[nodiscard]] inline const concat* phase2_access( const object& o, const key1& prefix, const key1& suffix )
   {
      assert( !suffix.empty() );

      try {
         if( prefix.empty() ) {
            return phase2_suffix_access( o.object.at( suffix.front().get_name() ), pop_front( suffix ), false );
         }
         return phase2_prefix_access( o.object.at( prefix.front().get_name() ), pop_front( prefix ), suffix );
      }
      catch( const better_luck_next_time_t& /*unused*/ ) {
         return nullptr;
      }
   }

}  // namespace tao::config::internal

#endif
