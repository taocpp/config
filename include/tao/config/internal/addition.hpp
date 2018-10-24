// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ADDITION_HH
#define TAO_CONFIG_INTERNAL_ADDITION_HH

#include "value.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< template< typename... > class Traits >
         json::null_t null_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::NULL_:
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for null addition" );
               }
            }
            return json::null;
         }

         template< template< typename... > class Traits >
         bool boolean_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            bool result = false;

            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::BOOLEAN:
                     result |= e.unsafe_get_boolean();
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for boolean addition" );
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         double double_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            double result = 0.0;

            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::DOUBLE:
                     result += e.unsafe_get_double();
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for double addition" );
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         __int128_t integer_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            __int128_t result = 0;

            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::SIGNED:
                     result += e.unsafe_get_signed();
                     break;
                  case json::type::UNSIGNED:
                     result += e.unsafe_get_unsigned();
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for integer addition" );
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         std::string string_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::ostringstream oss;

            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::STRING:
                     oss << e.unsafe_get_string();
                     continue;
                  case json::type::STRING_VIEW:
                     oss << e.unsafe_get_string_view();
                     continue;
                  default:
                     throw std::runtime_error( "inconsistent json types for string addition" );
               }
            }
            return oss.str();
         }

         template< template< typename... > class Traits >
         std::vector< std::byte > binary_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::vector< std::byte > result;

            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::BINARY:
                     result.insert( result.end(), e.unsafe_get_binary().begin(), e.unsafe_get_binary().end() );
                     continue;
                  case json::type::BINARY_VIEW:
                     result.insert( result.end(), e.unsafe_get_binary_view().begin(), e.unsafe_get_binary_view().end() );
                     continue;
                  default:
                     throw std::runtime_error( "inconsistent json types for binary addition" );
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         std::vector< json::basic_value< Traits > > array_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::vector< json::basic_value< Traits > > result;

            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::ARRAY:
                     result.insert( result.end(), e.unsafe_get_array().begin(), e.unsafe_get_array().end() );
                     continue;
                  default:
                     throw std::runtime_error( "inconsistent json types for array addition" );
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         std::map< std::string, json::basic_value< Traits > > object_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::map< std::string, json::basic_value< Traits > > result;

            for( const auto& e : a ) {
               switch( e.type() ) {
                  case json::type::OBJECT:
                     for( const auto& i : e.unsafe_get_object() ) {
                        result.insert_or_assign( i.first, i.second );  // TODO: We can do better than this for performance, but not with the same simplicity?
                     }
                     break;
                  default:
                     throw std::runtime_error( "inconsistent json types for object addition" );
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         json::basic_value< Traits > integer_addition_value( const std::vector< json::basic_value< Traits > >& a )
         {
            const auto t = integer_addition( a );
            return ( t < 0 ) ? json::basic_value< Traits >( std::int64_t( t ) ) : json::basic_value< Traits >( std::uint64_t( t ) );  // TODO: Check for overflow etc.
         }

         template< template< typename... > class Traits >
         json::basic_value< Traits > value_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            assert( !a.empty() );

            switch( a[ 0 ].type() ) {
               case json::type::NULL_:
                  return json::basic_value< Traits >( null_addition( a ) );
               case json::type::ARRAY:
                  return json::basic_value< Traits >( array_addition( a ) );
               case json::type::OBJECT:
                  return json::basic_value< Traits >( object_addition( a ) );
               case json::type::BOOLEAN:
                  return json::basic_value< Traits >( boolean_addition( a ) );
               case json::type::SIGNED:
               case json::type::UNSIGNED:
                  return integer_addition_value( a );
               case json::type::STRING:
               case json::type::STRING_VIEW:
                  return json::basic_value< Traits >( string_addition( a ) );
               case json::type::BINARY:
               case json::type::BINARY_VIEW:
                  return json::basic_value< Traits >( binary_addition( a ) );
               default:
                  std::cerr << json::to_string( a[ 0 ].type() ) << std::endl;
                  throw std::runtime_error( "invalid json type for addition" );
            }
         }

         // template< template< typename... > class Traits >
         // json::basic_value< Traits > value_addition( const json::basic_value< Traits >& v )
         // {
         //    return value_addition< Traits >( v.unsafe_get_array() );
         // }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
