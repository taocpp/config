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
         struct overflow_error
         {
         };

         struct addition_error
         {
            json::type l;
            json::type r;
         };

         template< template< typename... > class Traits >
         void boolean_addition( json::basic_value< Traits >& l, const json::basic_value< Traits >& r )
         {
            switch( r.type() ) {
               case json::type::BOOLEAN:
                  l = l.unsafe_get_boolean() || r.unsafe_get_boolean();
                  break;
               default:
                  throw addition_error{ l.type(), r.type() };
            }
         }

         template< template< typename... > class Traits >
         void number_addition( json::basic_value< Traits >& l, const json::basic_value< Traits >& r )
         {
            if( l.type() == json::type::DOUBLE ) {
               if( !r.is_number() ) {
                  throw addition_error{ l.type(), r.type() };
               }
               l = l.unsafe_get_double() + r.template as< double >();
               return;
            }
            if( r.type() == json::type::DOUBLE ) {
               if( !l.is_number() ) {
                  assert( false );
               }
               l = l.template as< double >() + r.unsafe_get_double();
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
               l = std::uint64_t( t );
            }
            else {
               if( t != __int128_t( std::int64_t( t ) ) ) {
                  throw overflow_error();
               }
               l = std::int64_t( t );
            }
         }

         template< template< typename... > class Traits >
         void string_addition( json::basic_value< Traits >& l, const json::basic_value< Traits >& r )
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
         void binary_addition( json::basic_value< Traits >& l, const json::basic_value< Traits >& r )
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
                  case json::type::UNSIGNED:
                  case json::type::DOUBLE:
                     return number_addition( l, r );
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
               throw std::runtime_error( format( "inconsistent types in addition", { &pos, { "old_type", e.l }, { "new_type", e.r } } ) );
            }
            catch( const overflow_error& ) {
               throw std::runtime_error( format( "numeric overflow in addition", { &pos } ) );
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
