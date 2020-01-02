// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EVENTS_HPP
#define TAO_CONFIG_INTERNAL_EVENTS_HPP

#include "../external/json.hpp"

namespace tao::config::internal
{
   template< typename Consumer, template< typename... > class Traits >
   void events_from_value( Consumer& c, const json::basic_value< Traits >& v )
   {
      c.begin_object( 2 );
      c.key( "meta" );
      json::events::produce< Traits >( c, v.public_base() );
      c.member();
      c.key( "data" );
      json::events::from_value< events_from_value< Consumer, Traits > >( c, v );
      c.member();
      c.end_object( 2 );
   }

}  // namespace tao::config::internal

#endif
