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
#if defined( __GNUC__ ) || defined( __clang__ )
   using max_int_t = __int128_t;
#else
   using max_int_t = std::int64_t;
#endif

   struct overflow_error
   {
   };

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
      if( l.type() == json::type::DOUBLE ) {
         if( !r.is_number() ) {
            throw addition_error{ l.type(), r.type() };
         }
         l.assign( l.get_double() + r.template as< double >() );
         return;
      }
      if( r.type() == json::type::DOUBLE ) {
         if( !l.is_number() ) {
            assert( false );
         }
         l.assign( l.template as< double >() + r.get_double() );
         return;
      }
      max_int_t t = 0;

      if( l.is_signed() ) {
         t += l.get_signed();
      }
      else if( l.is_unsigned() ) {
         t += l.get_unsigned();
      }
      else {
         assert( false );
      }
      if( r.is_signed() ) {
         t += r.get_signed();
      }
      else if( r.is_unsigned() ) {
         t += r.get_unsigned();
      }
      else {
         throw addition_error{ l.type(), r.type() };
      }
      if( t >= 0 ) {
         if( t != max_int_t( std::uint64_t( t ) ) ) {
            throw overflow_error();
         }
         l.assign( std::uint64_t( t ) );
      }
      else {
         if( t != max_int_t( std::int64_t( t ) ) ) {
            throw overflow_error();
         }
         l.assign( std::int64_t( t ) );
      }
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
      catch( const overflow_error& ) {
         assert( r.position );
         throw pegtl::parse_error( "numeric overflow in addition", *r.position );  // l.position?
      }
   }

}  // namespace tao::config::internal

#endif
