// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config/internal/key1.hpp>

namespace tao::config
{
   void unit_test()
   {
      internal::key1 k( "foo.bar.42.-.*" );
      TAO_CONFIG_TEST_ASSERT( k.size() == 5 );
      TAO_CONFIG_TEST_ASSERT( k[ 0 ].kind() == internal::key1_kind::name );
      TAO_CONFIG_TEST_ASSERT( k[ 0 ].get_name() == "foo" );
      TAO_CONFIG_TEST_ASSERT( k[ 1 ].kind() == internal::key1_kind::name );
      TAO_CONFIG_TEST_ASSERT( k[ 1 ].get_name() == "bar" );
      TAO_CONFIG_TEST_ASSERT( k[ 2 ].kind() == internal::key1_kind::index );
      TAO_CONFIG_TEST_ASSERT( k[ 2 ].get_index() == 42 );
      TAO_CONFIG_TEST_ASSERT( k[ 3 ].kind() == internal::key1_kind::minus );
      TAO_CONFIG_TEST_ASSERT( k[ 4 ].kind() == internal::key1_kind::star );
   }

}  // namespace tao::config

#include "main.hpp"
