// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config/internal/reference2.hpp>

namespace tao::config
{
   void unit_test()
   {
      internal::reference2 k( "(foo.(bar.1).42)" );
      TAO_CONFIG_TEST_ASSERT( k.size() == 3 );
      TAO_CONFIG_TEST_ASSERT( k[ 0 ].kind() == internal::reference2_kind::name );
      TAO_CONFIG_TEST_ASSERT( k[ 0 ].get_name() == "foo" );
      TAO_CONFIG_TEST_ASSERT( k[ 1 ].kind() == internal::reference2_kind::vector );
      const auto& v = k[ 1 ].get_vector();
      TAO_CONFIG_TEST_ASSERT( v.size() == 2 );
      TAO_CONFIG_TEST_ASSERT( v[ 0 ].kind() == internal::reference2_kind::name );
      TAO_CONFIG_TEST_ASSERT( v[ 0 ].get_name() == "bar" );
      TAO_CONFIG_TEST_ASSERT( v[ 1 ].kind() == internal::reference2_kind::index );
      TAO_CONFIG_TEST_ASSERT( v[ 1 ].get_index() == 1 );
      TAO_CONFIG_TEST_ASSERT( k[ 2 ].kind() == internal::reference2_kind::index );
      TAO_CONFIG_TEST_ASSERT( k[ 2 ].get_index() == 42 );

      TAO_CONFIG_TEST_THROWS( internal::reference2( "foo.-" ) );
   }

}  // namespace tao::config

#include "main.hpp"
