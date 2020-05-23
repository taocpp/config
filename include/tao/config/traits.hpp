// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_TRAITS_HPP
#define TAO_CONFIG_TRAITS_HPP

#include "external/json.hpp"

#include "annotation.hpp"

namespace tao::config
{
   template< typename T >
   struct traits
      : public json::traits< T >
   {
   };

   template<>
   struct traits< void >
      : public json::traits< void >
   {
      static constexpr const bool enable_implicit_constructor = false;

      template< typename Value >
      using public_base = annotation;
   };

   template<>
   struct traits< part >
   {
      template< template< typename... > class, typename Consumer >
      static void produce( Consumer& c, const part& t )
      {
         switch( t.type() ) {
            case part::name:
               c.string( t.get_name() );
               return;
            case part::index:
               c.number( std::uint64_t( t.get_index() ) );
               return;
            case part::star:
               c.string( "*" );
               return;
            case part::minus:
               c.string( "-" );
               return;
         }
         assert( false );
      }
   };

   template<>
   struct traits< key >
   {
      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const key& p )
      {
         c.string( to_string( p ) );
      }
   };

   template<>
   struct traits< const key* >
   {
      TAO_JSON_DEFAULT_KEY( "key" );

      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const key* p )
      {
         v.set_opaque_ptr( p );
      }
   };

   template<>
   struct traits< key* >
      : public traits< const key* >
   {
   };

   template<>
   struct traits< json::position >
   {
      template< template< typename... > class Traits, typename Consumer >
      static void produce( Consumer& c, const json::position& p )
      {
         c.string( p.source() + ':' + std::to_string( p.line() ) + ':' + std::to_string( p.column() ) );
      }
   };

   template<>
   struct traits< const json::position* >
   {
      TAO_JSON_DEFAULT_KEY( "position" );

      template< template< typename... > class Traits >
      static void assign( json::basic_value< Traits >& v, const json::position* p )
      {
         v.set_opaque_ptr( p );
      }
   };

   template<>
   struct traits< json::position* >
      : public traits< const json::position* >
   {
   };

   template<>
   struct traits< annotation >
      : public json::binding::object< TAO_JSON_BIND_REQUIRED( "key", &annotation::key ),
                                      TAO_JSON_BIND_REQUIRED( "position", &annotation::position ) >
   {
      TAO_JSON_DEFAULT_KEY( "meta" );
   };

}  // namespace tao::config

#endif
