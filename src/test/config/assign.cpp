// Copyright (c) 2021 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config.hpp>

namespace tao::config
{
   void unit_test()
   {
      auto v = from_string( "a = 1, b = 2, c = { d = { e = [ 6 7 ] } }", __FUNCTION__ );

      TAO_CONFIG_TEST_ASSERT( assign( v, key( "a" ) ) == value( 1 ) );
      TAO_CONFIG_TEST_ASSERT( assign( v, key( "b" ) ) == value( 2 ) );

      TAO_CONFIG_TEST_ASSERT( assign( v, key( "a" ) ).key == key( "a" ) );
      TAO_CONFIG_TEST_ASSERT( assign( v, key( "b" ) ).key == key( "b" ) );

      TAO_CONFIG_TEST_ASSERT( assign( v, key( "c.d.e.0" ) ) == value( 6 ) );
      TAO_CONFIG_TEST_ASSERT( assign( v, key( "c.d.e.1" ) ) == value( 7 ) );

      TAO_CONFIG_TEST_ASSERT( assign( v, key( "c.d.e.0" ) ).key == key( "c.d.e.0" ) );
      TAO_CONFIG_TEST_ASSERT( assign( v, key( "c.d.e.1" ) ).key == key( "c.d.e.1" ) );

      TAO_CONFIG_TEST_THROWS( (void)assign( v, key( "0" ) ) );
      TAO_CONFIG_TEST_THROWS( (void)assign( v, key( "c.d.e.f" ) ) );
      TAO_CONFIG_TEST_THROWS( (void)assign( v, key( "c.d.e.2" ) ) );

      TAO_CONFIG_TEST_ASSERT( assign( v, key( "r" ) ) == value( json::empty_object ) );
      TAO_CONFIG_TEST_ASSERT( assign( v, key( "r.s.t" ) ) == value( json::empty_object ) );
   }

}  // namespace tao::config

#include "main.hpp"
