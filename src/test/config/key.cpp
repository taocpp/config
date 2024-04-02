// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <sstream>
#include <string>

#include "test.hpp"

#include <tao/config/key.hpp>

namespace tao::config
{
   void unit_test()
   {
      const std::string s0 = "foo.bar.42.\" \"";
      const std::vector< key_part > kpv = { key_part( "foo" ), key_part( "bar" ), key_part( 42 ), key_part( " " ) };
      const key k1( kpv.begin(), kpv.end() );
      const key k2( "foo.bar.42.' '" );
      const key k3 = { key_part( "foo" ), key_part( "bar" ), key_part( 42 ), key_part( " " ) };
      key k4 = k3;
      TAO_CONFIG_TEST_ASSERT( k1.size() == 4 );
      TAO_CONFIG_TEST_ASSERT( k1 == k2 );
      TAO_CONFIG_TEST_ASSERT( k1 == k3 );
      TAO_CONFIG_TEST_ASSERT( k1.vector() == k2.vector() );
      TAO_CONFIG_TEST_ASSERT( k1.vector() == k4.vector() );
      const auto k5 = pop_front( k1 );
      const auto k6 = pop_back( k1 );
      TAO_CONFIG_TEST_ASSERT( k5.size() == 3 );
      TAO_CONFIG_TEST_ASSERT( k6.size() == 3 );
      TAO_CONFIG_TEST_ASSERT( k5 == key( "bar.42.' '" ) );
      TAO_CONFIG_TEST_ASSERT( k6 == key( "foo.bar.42" ) );
      const std::string s1 = to_string( k1 );
      TAO_CONFIG_TEST_ASSERT( s1 == s0 );
      std::ostringstream oss;
      to_stream( oss, k1 );
      TAO_CONFIG_TEST_ASSERT( oss.str() == s0 );
      k4 = pop_back( k1 );
      TAO_CONFIG_TEST_ASSERT( k4 + " " == k1 );
      TAO_CONFIG_TEST_ASSERT( k4 + key_part( " " ) == k1 );
      TAO_CONFIG_TEST_ASSERT( k4 + key( "\" \"" ) == k1 );
      TAO_CONFIG_TEST_ASSERT( k1 + 123 == key( s0 + ".123" ) );
   }

}  // namespace tao::config

#include "main.hpp"
