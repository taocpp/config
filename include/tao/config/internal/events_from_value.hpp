// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EVENTS_FROM_VALUE_HPP
#define TAO_CONFIG_INTERNAL_EVENTS_FROM_VALUE_HPP

#include "debug_traits.hpp"
#include "json.hpp"

namespace tao::config::internal
{
   template< typename Consumer, template< typename... > class Traits >
   void events_from_value( Consumer& c, const json::basic_value< Traits >& v )
   {
      c.begin_object( 2 );
      c.key( "meta" );
      json::events::produce< debug_traits >( c, v.public_base() );
      c.member();
      c.key( "data" );
      json::events::from_value< events_from_value< Consumer, Traits > >( c, v );
      c.member();
      c.end_object( 2 );
   }

}  // namespace tao::config::internal

#endif
