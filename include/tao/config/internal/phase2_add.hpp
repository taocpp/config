// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_ADD_HH
#define TAO_CONFIG_INTERNAL_PHASE2_ADD_HH

#include <utility>

#include "concat.hpp"
#include "entry.hpp"
#include "format.hpp"
#include "phase2_traits.hpp"

namespace tao::config::internal
{
   struct overflow_error
   {
   };

   struct addition_error
   {
      json::type l;
      json::type r;
   };

   inline void phase2_boolean_add( json::basic_value< phase2_traits >& l, const json::basic_value< phase2_traits >& r )
   {
      switch( r.type() ) {
         case json::type::BOOLEAN:
            l = json::basic_value< phase2_traits >( l.unsafe_get_boolean() || r.unsafe_get_boolean() );
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_number_add( json::basic_value< phase2_traits >& l, const json::basic_value< phase2_traits >& r )
   {
      if( l.type() == json::type::DOUBLE ) {
         if( !r.is_number() ) {
            throw addition_error{ l.type(), r.type() };
         }
         l = json::basic_value< phase2_traits >( l.unsafe_get_double() + r.template as< double >() );
         return;
      }
      if( r.type() == json::type::DOUBLE ) {
         if( !l.is_number() ) {
            assert( false );
         }
         l = json::basic_value< phase2_traits >( l.template as< double >() + r.unsafe_get_double() );
         return;
      }
      __int128_t t = 0;

      if( l.is_signed() ) {
         t += l.unsafe_get_signed();
      }
      else if( l.is_unsigned() ) {
         t += l.unsafe_get_unsigned();
      }
      else {
         assert( false );
      }
      if( r.is_signed() ) {
         t += r.unsafe_get_signed();
      }
      else if( r.is_unsigned() ) {
         t += r.unsafe_get_unsigned();
      }
      else {
         throw addition_error{ l.type(), r.type() };
      }
      if( t >= 0 ) {
         if( t != __int128_t( std::uint64_t( t ) ) ) {
            throw overflow_error();
         }
         l = json::basic_value< phase2_traits >( std::uint64_t( t ) );
      }
      else {
         if( t != __int128_t( std::int64_t( t ) ) ) {
            throw overflow_error();
         }
         l = json::basic_value< phase2_traits >( std::int64_t( t ) );
      }
   }

   inline void phase2_string_add( json::basic_value< phase2_traits >& l, const json::basic_value< phase2_traits >& r )
   {
      switch( r.type() ) {
         case json::type::STRING:
            l.get_string() += r.unsafe_get_string();
            break;
         case json::type::STRING_VIEW:
            l.get_string() += r.unsafe_get_string_view();
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_binary_add( json::basic_value< phase2_traits >& l, const json::basic_value< phase2_traits >& r )
   {
      switch( r.type() ) {
         case json::type::BINARY:
            l.get_binary().insert( l.unsafe_get_binary().end(), r.unsafe_get_binary().begin(), r.unsafe_get_binary().end() );
            break;
         case json::type::BINARY_VIEW:
            l.get_binary().insert( l.unsafe_get_binary().end(), r.unsafe_get_binary_view().begin(), r.unsafe_get_binary_view().end() );
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_array_add( json::basic_value< phase2_traits >& l, json::basic_value< phase2_traits >&& r )
   {
      switch( r.type() ) {
         case json::type::ARRAY:
            l.unsafe_get_array().reserve( l.unsafe_get_array().size() + r.unsafe_get_array().size() );
            for( auto&& i : r.unsafe_get_array() ) {
               l.emplace_back( std::move( i ) );
            }
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_value_add( json::basic_value< phase2_traits >& l, json::basic_value< phase2_traits >&& r );

   inline void phase2_object_add( json::basic_value< phase2_traits >& l, json::basic_value< phase2_traits >&& r )
   {
      switch( r.type() ) {
         case json::type::OBJECT:
            for( auto&& i : r.unsafe_get_object() ) {
               if( const auto p = l.unsafe_get_object().try_emplace( i.first, std::move( i.second ) ); !p.second ) {
                  phase2_value_add( p.first->second, std::move( i.second ) );
               }
            }
            break;
         default:
            throw addition_error{ l.type(), r.type() };
      }
   }

   inline void phase2_value_add( json::basic_value< phase2_traits >& l, json::basic_value< phase2_traits >&& r )
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

   inline void phase2_add( json::basic_value< phase2_traits >& l, json::basic_value< phase2_traits >&& r, const position& pos )
   {
      try {
         phase2_value_add( l, std::move( r ) );
      }
      catch( const addition_error& e ) {
         throw pegtl::parse_error( format( __FILE__, __LINE__, "inconsistent types in addition", { { "left", e.l }, { "right", e.r } } ), pos );
      }
      catch( const overflow_error& ) {
         throw pegtl::parse_error( "numeric overflow in addition", pos );
      }
   }

}  // namespace tao::config::internal

#endif
