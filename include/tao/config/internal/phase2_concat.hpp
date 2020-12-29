// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_CONCAT_HH
#define TAO_CONFIG_INTERNAL_PHASE2_CONCAT_HH

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
   struct concat_error
   {
      json::type l;
      json::type r;
   };

   // For numbers we currently follow two rules in order:
   // 1. Integers and floating-point can't be mixed, and
   // 2. Mixed integers are handled like in the C language.

   inline void phase2_number_concat( json_t& l, const json_t& r )
   {
      if( l.is_double() || r.is_double() ) {
         if( l.is_double() && r.is_double() ) {
            l.assign( l.get_double() + r.get_double() );
            return;
         }
      }
      else if( l.is_signed() ) {
         if( r.is_signed() ) {
            l.assign( l.get_signed() + r.get_signed() );
            return;
         }
         if( r.is_unsigned() ) {
            l.assign( l.get_signed() + std::int64_t( r.get_unsigned() ) );
            return;
         }
      }
      else if( l.is_unsigned() ) {
         if( r.is_signed() ) {
            l.assign( std::int64_t( l.get_unsigned() ) + r.get_signed() );
            return;
         }
         if( r.is_unsigned() ) {
            l.assign( l.get_unsigned() + r.get_unsigned() );
            return;
         }
      }
      throw concat_error{ l.type(), r.type() };
   }

   inline void phase2_string_concat( json_t& l, const json_t& r )
   {
      switch( r.type() ) {
         case json::type::STRING:
            l.get_string() += r.get_string();
            break;
         default:
            throw concat_error{ l.type(), r.type() };
      }
   }

   inline void phase2_binary_concat( json_t& l, const json_t& r )
   {
      switch( r.type() ) {
         case json::type::BINARY:
            l.get_binary().insert( l.get_binary().end(), r.get_binary().begin(), r.get_binary().end() );
            break;
         default:
            throw concat_error{ l.type(), r.type() };
      }
   }

   inline void phase2_array_concat( json_t& l, json_t&& r )
   {
      switch( r.type() ) {
         case json::type::ARRAY:
            l.get_array().reserve( l.get_array().size() + r.get_array().size() );
            for( auto&& i : r.get_array() ) {
               l.emplace_back( std::move( i ) );
            }
            break;
         default:
            throw concat_error{ l.type(), r.type() };
      }
   }

   inline void phase2_value_concat( json_t& l, json_t&& r );

   inline void phase2_object_concat( json_t& l, json_t&& r )
   {
      switch( r.type() ) {
         case json::type::OBJECT:
            for( auto&& i : r.get_object() ) {
               const auto p = l.get_object().try_emplace( i.first, std::move( i.second ) );
               if( !p.second ) {
                  phase2_value_concat( p.first->second, std::move( i.second ) );  // TODO: Is this recursive call still correct for the intended semantics???
               }
            }
            break;
         default:
            throw concat_error{ l.type(), r.type() };
      }
   }

   inline void phase2_value_concat( json_t& l, json_t&& r )
   {
      if( r.is_null() ) {
         return;
      }
      switch( l.type() ) {
         case json::type::UNINITIALIZED:
            throw concat_error{ l.type(), r.type() };
         case json::type::NULL_:
            l = std::move( r );
            return;
         case json::type::BOOLEAN:
            throw concat_error{ l.type(), r.type() };
         case json::type::SIGNED:
         case json::type::UNSIGNED:
         case json::type::DOUBLE:
            phase2_number_concat( l, r );
            break;
         case json::type::STRING:
            phase2_string_concat( l, r );
            break;
         case json::type::STRING_VIEW:
            assert( false );  // UNREACHABLE
         case json::type::BINARY:
            phase2_binary_concat( l, r );
            break;
         case json::type::BINARY_VIEW:
            assert( false );  // UNREACHABLE
         case json::type::ARRAY:
            phase2_array_concat( l, std::move( r ) );
            break;
         case json::type::OBJECT:
            phase2_object_concat( l, std::move( r ) );
            break;
         default:
            assert( false );
      }
   }

   inline void phase2_concat( json_t& l, json_t&& r )
   {
      try {
         phase2_value_concat( l, std::move( r ) );
      }
      catch( const concat_error& e ) {
         throw pegtl::parse_error( "inconsistent json types", r.position );  // TODO: Add l.position to the parse_error, too.
      }
   }

}  // namespace tao::config::internal

#endif
