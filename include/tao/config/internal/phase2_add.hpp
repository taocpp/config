// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ADD_HH
#define TAO_CONFIG_INTERNAL_PHASE2_ADD_HH

#include <utility>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   struct phase2_add_error
   {
      json::type l;
      json::type r;
   };

   // For numbers we currently follow two rules in order:
   // 1. Integers and floating-point can't be mixed, and
   // 2. Mixed integers are handled like in the C language.

   inline void phase2_number_add( const json_t& l, json_t& r )
   {
      if( l.is_double() || r.is_double() ) {
         if( l.is_double() && r.is_double() ) {
            r.assign( l.get_double() + r.get_double() );
            return;
         }
      }
      else if( l.is_signed() ) {
         if( r.is_signed() ) {
            r.assign( l.get_signed() + r.get_signed() );
            return;
         }
         if( r.is_unsigned() ) {
            r.assign( l.get_signed() + std::int64_t( r.get_unsigned() ) );
            return;
         }
      }
      else if( l.is_unsigned() ) {
         if( r.is_signed() ) {
            r.assign( std::int64_t( l.get_unsigned() ) + r.get_signed() );
            return;
         }
         if( r.is_unsigned() ) {
            r.assign( l.get_unsigned() + r.get_unsigned() );
            return;
         }
      }
      throw phase2_add_error{ l.type(), r.type() };
   }

   inline void phase2_string_add( const json_t& l, json_t& r )
   {
      switch( r.type() ) {
         case json::type::STRING:
            r.get_string() = l.get_string() + r.get_string();
            break;
         default:
            throw phase2_add_error{ l.type(), r.type() };
      }
   }

   inline void phase2_binary_add( const json_t& l, json_t& r )
   {
      switch( r.type() ) {
         case json::type::BINARY:
            r.get_binary().insert( r.get_binary().begin(), l.get_binary().begin(), l.get_binary().end() );
            break;
         default:
            throw phase2_add_error{ l.type(), r.type() };
      }
   }

   inline void phase2_value_add( json_t&& l, json_t& r )
   {
      if( r.is_null() ) {
         r = std::move( l );
         return;
      }
      switch( l.type() ) {
         case json::type::UNINITIALIZED:
            throw phase2_add_error{ l.type(), r.type() };
         case json::type::NULL_:
            return;
         case json::type::BOOLEAN:
            throw phase2_add_error{ l.type(), r.type() };
         case json::type::SIGNED:
         case json::type::UNSIGNED:
         case json::type::DOUBLE:
            phase2_number_add( l, r );
            return;
         case json::type::STRING:
            phase2_string_add( l, r );
            return;
         case json::type::STRING_VIEW:
            assert( false );  // UNREACHABLE
         case json::type::BINARY:
            phase2_binary_add( l, r );
            return;
         case json::type::BINARY_VIEW:
            assert( false );  // UNREACHABLE
         case json::type::ARRAY:
         case json::type::OBJECT:
         case json::type::VALUE_PTR:
         case json::type::OPAQUE_PTR:
            assert( false );  // UNREACHABLE
         case json::type::VALUELESS_BY_EXCEPTION:
            throw std::string( "something went wrong -- including the quality of this temporary error message -- TODO: improve it" );
      }
      assert( false );  // UNREACHABLE
   }

   inline void phase2_add( json_t&& l, json_t& r )
   {
      try {
         phase2_value_add( std::move( l ), r );
      }
      catch( const phase2_add_error& e ) {
         throw pegtl::parse_error( "inconsistent json types", r.position );  // TODO: Add l.position to the parse_error, too.
      }
   }

}  // namespace tao::config::internal

#endif
