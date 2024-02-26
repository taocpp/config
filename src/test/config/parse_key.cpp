// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config/key.hpp>

namespace tao::config
{
   void unit_test()
   {
      key k( "foo.bar.42" );
      TAO_CONFIG_TEST_ASSERT( k.size() == 3 );
      TAO_CONFIG_TEST_ASSERT( k[ 0 ].kind() == key_kind::name );
      TAO_CONFIG_TEST_ASSERT( k[ 0 ].get_name() == "foo" );
      TAO_CONFIG_TEST_ASSERT( k[ 1 ].kind() == key_kind::name );
      TAO_CONFIG_TEST_ASSERT( k[ 1 ].get_name() == "bar" );
      TAO_CONFIG_TEST_ASSERT( k[ 2 ].kind() == key_kind::index );
      TAO_CONFIG_TEST_ASSERT( k[ 2 ].get_index() == 42 );

      TAO_CONFIG_TEST_THROWS( key( "foo.-" ) );
   }

}  // namespace tao::config

#include "main.hpp"
