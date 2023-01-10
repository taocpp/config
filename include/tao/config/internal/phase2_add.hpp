// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ADD_HH
#define TAO_CONFIG_INTERNAL_PHASE2_ADD_HH

#include <stdexcept>
#include <utility>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "pegtl.hpp"
#include "string_utility.hpp"

namespace tao::config::internal
{
   struct phase2_add_error
   {};

   // For numbers we currently use the following rules:
   // 1. Integers and floating-point can't be mixed, and:
   // 2. Mixed integers result in a signed value.

   inline void phase2_signed_add( const json_t& l, json_t& r )
   {
      if( r.is_signed() ) {
         r.assign( l.get_signed() + r.get_signed() );
         return;
      }
      if( r.is_unsigned() ) {
         r.assign( l.get_signed() + std::int64_t( r.get_unsigned() ) );
         return;
      }
      throw phase2_add_error();
   }

   inline void phase2_unsigned_add( const json_t& l, json_t& r )
   {
      if( r.is_signed() ) {
         r.assign( std::int64_t( l.get_unsigned() ) + r.get_signed() );
         return;
      }
      if( r.is_unsigned() ) {
         r.assign( l.get_unsigned() + r.get_unsigned() );
         return;
      }
      throw phase2_add_error();
   }

   inline void phase2_double_add( const json_t& l, json_t& r )
   {
      if( r.is_double() ) {
         r.assign( l.get_double() + r.get_double() );
         return;
      }
      throw phase2_add_error();
   }

   inline void phase2_string_add( const json_t& l, json_t& r )
   {
      if( r.type() == json::type::STRING ) {
         r.get_string() = l.get_string() + r.get_string();
         return;
      }
      throw phase2_add_error();
   }

   inline void phase2_binary_add( const json_t& l, json_t& r )
   {
      if( r.type() == json::type::BINARY ) {
         r.get_binary().insert( r.get_binary().begin(), l.get_binary().begin(), l.get_binary().end() );
         return;
      }
      throw phase2_add_error();
   }

   inline void phase2_value_add( json_t&& l, json_t& r )
   {
      switch( l.type() ) {
         case json::type::NULL_:
         case json::type::BOOLEAN:
            throw phase2_add_error();
         case json::type::SIGNED:
            phase2_signed_add( l, r );
            return;
         case json::type::UNSIGNED:
            phase2_unsigned_add( l, r );
            return;
         case json::type::DOUBLE:
            phase2_double_add( l, r );
            return;
         case json::type::STRING:
            phase2_string_add( l, r );
            return;
         case json::type::BINARY:
            phase2_binary_add( l, r );
            return;
         case json::type::STRING_VIEW:
         case json::type::BINARY_VIEW:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         case json::type::UNINITIALIZED:
         case json::type::ARRAY:
         case json::type::OBJECT:
         case json::type::VALUE_PTR:
         case json::type::OPAQUE_PTR:
         case json::type::VALUELESS_BY_EXCEPTION:
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
      throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
   }

   inline void phase2_add( json_t&& l, json_t& r )
   {
      try {
         phase2_value_add( std::move( l ), r );  // l is unchanged when a phase2_add_error is thrown.
      }
      catch( const phase2_add_error& /*unused*/ ) {
         pegtl::parse_error p( strcat( "incompatible types ", l.type(), "@", l.position, " and ", r.type(), "@", r.position ), pegtl::position( 0, 1, 1, "(todo) location of '+'" ) );
         throw p;
      }
   }

}  // namespace tao::config::internal

#endif
