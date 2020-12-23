// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_ACCESS_HPP

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
   [[nodiscard]] inline const concat& phase2_access( const concat& c, const key1& suffix );

   [[nodiscard]] inline const concat& phase2_access_minus( const concat& c, const key1& suffix )
   {
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   [[nodiscard]] inline const concat& phase2_access_name( const concat& c, const std::string& name, const key1& suffix )
   {
      for( const auto& e : reverse( c.concat ) ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "access name in value" );
            case entry_kind::reference:
               throw std::string( "access name in reference" );
            case entry_kind::array:
               throw std::string( "access name in array" );
            case entry_kind::object:
               if( const auto i = e.get_object().object.find( name ); i != e.get_object().object.end() ) {
                  return phase2_access( i->second, suffix );
               }
               continue;
         }
         assert( false );  // UNREACHABLE
      }
      throw std::string( "name not found" );  // NOTE: Just like all other throw std::string( ... ) this is just a temporary placeholder during refactoring.
   }


   [[nodiscard]] inline const concat& phase2_access_index( const concat& c, const std::size_t index, const key1& suffix )
   {
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   [[nodiscard]] inline const concat& phase2_access( const concat& c, const key1& suffix )
   {
      if( suffix.empty() ) {
         return c;
      }
      switch( suffix.at( 0 ).kind() ) {
         case key1_kind::star:
            throw pegtl::parse_error( "unable to access star", suffix.at( 0 ).position );
         case key1_kind::minus:
            return phase2_access_minus( c, pop_front( suffix ) );
         case key1_kind::name:
            return phase2_access_name( c, suffix.at( 0 ).get_name(), pop_front( suffix ) );
         case key1_kind::index:
            return phase2_access_index( c, suffix.at( 0 ).get_index(), pop_front( suffix ) );
         case key1_kind::append:
            throw pegtl::parse_error( "this should be impossible", suffix.at( 0 ).position );
      }
      assert( false );  // UNREACHABLE
   }

   [[nodiscard]] inline const concat& phase2_access( const concat& c, const key1& prefix, const key1& suffix );

   [[nodiscard]] inline const concat& phase2_access_minus( const concat& c, const key1& prefix, const key1& suffix )
   {
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   [[nodiscard]] inline const concat& phase2_access_name( const concat& c, const std::string& name, const key1& prefix, const key1& suffix )
   {
      for( const auto& e : reverse( c.concat ) ) {
         switch( e.kind() ) {
            case entry_kind::value:
               throw std::string( "access name in value" );
            case entry_kind::reference:
               throw std::string( "access name in reference" );
            case entry_kind::array:
               throw std::string( "access name in array" );
            case entry_kind::object:
               if( const auto i = e.get_object().object.find( name ); i != e.get_object().object.end() ) {
                  return phase2_access( i->second, prefix, suffix );
               }
               continue;
         }
         assert( false );  // UNREACHABLE
      }
      throw std::string( "name not found" );
   }

   [[nodiscard]] inline const concat& phase2_access_index( const concat& c, const std::size_t index, const key1& prefix, const key1& suffix )
   {
      throw std::string( "TODO: " ) + __FUNCTION__;
   }

   [[nodiscard]] inline const concat& phase2_access( const concat& c, const key1& prefix, const key1& suffix )
   {
      if( prefix.empty() ) {
         return phase2_access( c, suffix );
      }
      switch( prefix.at( 0 ).kind() ) {
         case key1_kind::star:
            throw pegtl::parse_error( "unable to access star", prefix.at( 0 ).position );
         case key1_kind::minus:
            return phase2_access_minus( c, pop_front( prefix ), suffix );
         case key1_kind::name:
            return phase2_access_name( c, prefix.at( 0 ).get_name(), pop_front( prefix ), suffix );
         case key1_kind::index:
            return phase2_access_index( c, prefix.at( 0 ).get_index(), pop_front( prefix ), suffix );
         case key1_kind::append:
            throw pegtl::parse_error( "this should be impossible", prefix.at( 0 ).position );
      }
      assert( false );  // UNREACHABLE
   }

   [[nodiscard]] inline const concat& phase2_access( const object& o, const key1& prefix, const key1& suffix )
   {
      assert( !suffix.empty() );

      if( prefix.empty() ) {
         return phase2_access( o.object.at( suffix.front().get_name() ), pop_front( suffix ) );
      }
      return phase2_access( o.object.at( prefix.front().get_name() ), pop_front( prefix ), suffix );
   }

}  // namespace tao::config::internal

#endif
