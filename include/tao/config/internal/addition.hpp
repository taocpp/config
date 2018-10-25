// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ADDITION_HH
#define TAO_CONFIG_INTERNAL_ADDITION_HH

#include "concat.hpp"
#include "entry.hpp"
#include "format.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         struct addition_error
         {
            json::type l;
            json::type r;
         };

         template< template< typename... > class Traits >
         void boolean_addition( json::basic_value< Traits >& l, json::basic_value< Traits >& r )
         {
            switch( r.type() ) {
               case json::type::BOOLEAN:
                  l = l.unsafe_get_boolean() | r.unsafe_get_boolean();
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void signed_addition( json::basic_value< Traits >& l, json::basic_value< Traits >& r )
         {
            switch( r.type() ) {
               case json::type::SIGNED:
                  l = l.unsafe_get_signed() + r.unsafe_get_signed();  // TODO: Check for overflow - and adapt target type?
                  break;
               case json::type::UNSIGNED:
                  l = l.unsafe_get_signed() + r.unsafe_get_unsigned();  // TODO: Check for overflow - and adapt target type?
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void unsigned_addition( json::basic_value< Traits >& l, json::basic_value< Traits >& r )
         {
            switch( r.type() ) {
               case json::type::SIGNED:
                  l = l.unsafe_get_unsigned() + r.unsafe_get_signed();  // TODO: Check for overflow - and adapt target type?
                  break;
               case json::type::UNSIGNED:
                  l = l.unsafe_get_unsigned() + r.unsafe_get_unsigned();  // TODO: Check for overflow - and adapt target type?
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void double_addition( json::basic_value< Traits >& l, json::basic_value< Traits >& r )
         {
            switch( r.type() ) {
               case json::type::DOUBLE:
                  l = l.unsafe_get_double() + r.unsafe_get_double();
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void string_addition( json::basic_value< Traits >& l, json::basic_value< Traits >& r )
         {
            switch( r.type() ) {
               case json::type::STRING:
                  l.unsafe_get_string() += r.unsafe_get_string();
                  break;
               case json::type::STRING_VIEW:
                  l.unsafe_get_string() += r.unsafe_get_string_view();
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void binary_addition( json::basic_value< Traits >& l, json::basic_value< Traits >& r )
         {
            switch( r.type() ) {
               case json::type::BINARY:
                  l.unsafe_get_binary().insert( l.unsafe_get_binary().end(), r.unsafe_get_binary().begin(), r.unsafe_get_binary().end() );
                  break;
               case json::type::BINARY_VIEW:
                  l.unsafe_get_binary().insert( l.unsafe_get_binary().end(), r.unsafe_get_binary_view().begin(), r.unsafe_get_binary_view().end() );
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void array_addition( json::basic_value< Traits >& l, json::basic_value< Traits >&& r )
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

         template< template< typename... > class Traits >
         void object_addition( json::basic_value< Traits >& l, json::basic_value< Traits >&& r )
         {
            switch( r.type() ) {
               case json::type::OBJECT:
                  for( auto&& i : r.unsafe_get_object() ) {
                     l.emplace( i.first, std::move( i.second ) );
                  }
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void addition( json::basic_value< Traits >& l, json::basic_value< Traits >&& r, const position& pos )
         {
            if( r.is_null() ) {
               return;
            }
            if( l.is_null() ) {
               l = std::move( r );
               return;
            }
            try {
               switch( l.type() ) {
                  case json::type::NULL_:
                     assert( false );
                  case json::type::BOOLEAN:
                     return boolean_addition( l, r );
                  case json::type::SIGNED:
                     return signed_addition( l, r );
                  case json::type::UNSIGNED:
                     return unsigned_addition( l, r );
                  case json::type::DOUBLE:
                     return double_addition( l, r );
                  case json::type::STRING:
                  case json::type::STRING_VIEW:
                     return string_addition( l, r );
                  case json::type::BINARY:
                  case json::type::BINARY_VIEW:
                     return binary_addition( l, r );
                  case json::type::ARRAY:
                     return array_addition( l, std::move( r ) );
                  case json::type::OBJECT:
                     return object_addition( l, std::move( r ) );
                  default:
                     assert( false );
               }
            }
            catch( const addition_error& e ) {
               throw std::runtime_error( format( "inconsistent types", { &pos, { "old_type", e.l }, { "new_type", e.r } } ) );
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
