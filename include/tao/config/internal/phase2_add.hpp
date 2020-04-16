// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ADD_HH
#define TAO_CONFIG_INTERNAL_PHASE2_ADD_HH

#include <utility>

#include "concat.hpp"
#include "entry.hpp"
#include "format.hpp"
#include "value_traits.hpp"

namespace tao::config::internal
{
   struct addition_error
   {
      json::type l;
      json::type r;
   };

   inline void phase2_boolean_add( json::basic_value< value_traits >& l, const json::basic_value< value_traits >& r )
   {
      switch( r.type() ) {
         case json::type::BOOLEAN:
            l.assign( l.get_boolean() || r.get_boolean() );
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_number_add( json::basic_value< value_traits >& l, const json::basic_value< value_traits >& r )
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
      throw addition_error{ l.type(), r.type() };
   }

   inline void phase2_string_add( json::basic_value< value_traits >& l, const json::basic_value< value_traits >& r )
   {
      switch( r.type() ) {
         case json::type::STRING:
            l.get_string() += r.get_string();
            break;
         case json::type::STRING_VIEW:
            l.get_string() += r.get_string_view();
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_binary_add( json::basic_value< value_traits >& l, const json::basic_value< value_traits >& r )
   {
      switch( r.type() ) {
         case json::type::BINARY:
            l.get_binary().insert( l.get_binary().end(), r.get_binary().begin(), r.get_binary().end() );
            break;
         case json::type::BINARY_VIEW:
            l.get_binary().insert( l.get_binary().end(), r.get_binary_view().begin(), r.get_binary_view().end() );
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_array_add( json::basic_value< value_traits >& l, json::basic_value< value_traits >&& r )
   {
      switch( r.type() ) {
         case json::type::ARRAY:
            l.get_array().reserve( l.get_array().size() + r.get_array().size() );
            for( auto&& i : r.get_array() ) {
               l.emplace_back( std::move( i ) );
            }
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_value_add( json::basic_value< value_traits >& l, json::basic_value< value_traits >&& r );

   inline void phase2_object_add( json::basic_value< value_traits >& l, json::basic_value< value_traits >&& r )
   {
      switch( r.type() ) {
         case json::type::OBJECT:
            for( auto&& i : r.get_object() ) {
               if( const auto p = l.get_object().try_emplace( i.first, std::move( i.second ) ); !p.second ) {
                  phase2_value_add( p.first->second, std::move( i.second ) );
               }
            }
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_value_add( json::basic_value< value_traits >& l, json::basic_value< value_traits >&& r )
   {
      if( r.is_null() ) {
         return;
      }
      if( l.is_null() || r.clear ) {
         l = std::move( r );
         return;
      }
      switch( l.type() ) {
         case json::type::UNINITIALIZED:
            throw addition_error{ l.type(), r.type() };
         case json::type::NULL_:
            assert( false );
         case json::type::BOOLEAN:
            phase2_boolean_add( l, r );
            break;
         case json::type::SIGNED:
         case json::type::UNSIGNED:
         case json::type::DOUBLE:
            phase2_number_add( l, r );
            break;
         case json::type::STRING:
         case json::type::STRING_VIEW:
            phase2_string_add( l, r );
            break;
         case json::type::BINARY:
         case json::type::BINARY_VIEW:
            phase2_binary_add( l, r );
            break;
         case json::type::ARRAY:
            phase2_array_add( l, std::move( r ) );
            break;
         case json::type::OBJECT:
            phase2_object_add( l, std::move( r ) );
            break;
         default:
            assert( false );
      }
   }

   inline void phase2_add( json::basic_value< value_traits >& l, json::basic_value< value_traits >&& r )
   {
      try {
         phase2_value_add( l, std::move( r ) );
      }
      catch( const addition_error& e ) {
         assert( r.position );
         throw pegtl::parse_error( format( __FILE__, __LINE__, "inconsistent types in addition", { { "left", e.l }, { "right", e.r } } ), *r.position );  // l.position?
      }
   }

}  // namespace tao::config::internal

#endif
