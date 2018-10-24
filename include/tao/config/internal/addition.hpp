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
            json::type type;
            std::size_t index;
         };

         template< template< typename... > class Traits >
         json::null_t null_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::NULL_:
                     break;
                  default:
                     throw addition_error{ json::type::NULL_, i };
               }
            }
            return json::null;
         }

         template< template< typename... > class Traits >
         bool boolean_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            bool result = false;

            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::BOOLEAN:
                     result |= a[ i ].unsafe_get_boolean();
                     break;
                  default:
                     throw addition_error{ json::type::BOOLEAN, i };
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         double double_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            double result = 0.0;

            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::DOUBLE:
                     result += a[ i ].unsafe_get_double();
                     break;
                  default:
                     throw addition_error{ json::type::DOUBLE, i };
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         __int128_t integer_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            __int128_t result = 0;

            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::SIGNED:
                     result += a[ i ].unsafe_get_signed();
                     break;
                  case json::type::UNSIGNED:
                     result += a[ i ].unsafe_get_unsigned();
                     break;
                  default:
                     throw addition_error{ json::type::SIGNED, i };  // TODO: Signed?
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         std::string string_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::ostringstream oss;

            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::STRING:
                     oss << a[ i ].unsafe_get_string();
                     continue;
                  case json::type::STRING_VIEW:
                     oss << a[ i ].unsafe_get_string_view();
                     continue;
                  default:
                     throw addition_error{ json::type::STRING, i };
               }
            }
            return oss.str();
         }

         template< template< typename... > class Traits >
         std::vector< std::byte > binary_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::vector< std::byte > result;

            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::BINARY:
                     result.insert( result.end(), a[ i ].unsafe_get_binary().begin(), a[ i ].unsafe_get_binary().end() );
                     continue;
                  case json::type::BINARY_VIEW:
                     result.insert( result.end(), a[ i ].unsafe_get_binary_view().begin(), a[ i ].unsafe_get_binary_view().end() );
                     continue;
                  default:
                     throw addition_error{ json::type::BINARY, i };
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         std::vector< json::basic_value< Traits > > array_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::vector< json::basic_value< Traits > > result;

            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::ARRAY:
                     result.insert( result.end(), a[ i ].unsafe_get_array().begin(), a[ i ].unsafe_get_array().end() );
                     continue;
                  default:
                     throw addition_error{ json::type::ARRAY, i };
               }
            }
            return result;
         }

         template< template< typename... > class Traits >
         std::map< std::string, json::basic_value< Traits > > object_addition( const std::vector< json::basic_value< Traits > >& a )
         {
            std::map< std::string, json::basic_value< Traits > > result;

            for( std::size_t i = 0; i < a.size(); ++i ) {
               switch( a[ i ].type() ) {
                  case json::type::OBJECT:
                     for( const auto& j : a[ i ].unsafe_get_object() ) {
                        result.insert_or_assign( j.first, j.second );
                     }
                     break;
                  default:
                     throw addition_error{ json::type::OBJECT, i };
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
         json::basic_value< Traits > value_addition( const concat& c, const std::vector< json::basic_value< Traits > >& a )
         {
            assert( !a.empty() );
            assert( a.size() == c.v.size() );

            try {
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
                     throw std::runtime_error( format( "invalid addition type", { &c.v[ 0 ].position(), a[ 0 ].type() } ) );
               }
            }
            catch( const addition_error& e ) {
               throw std::runtime_error( format( "inconsistent addition types", { { "first", { &c.v[ 0 ].position(), a[ 0 ].type() } }, { "later", { &c.v[ e.index ].position(), a[ e.index ].type() } } } ) );
            }
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
