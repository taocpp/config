// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <sstream>
#include <string>

#include "test.hpp"

#include <tao/config/key_part.hpp>

namespace tao::config
{
   void test_name1()
   {
      const std::string str = "foo";
      const key_part p( str );
      TAO_CONFIG_TEST_ASSERT( p.kind() == key_kind::name );
      TAO_CONFIG_TEST_ASSERT( p.get_name() == str );
      TAO_CONFIG_TEST_ASSERT( to_string( p ) == str );
      std::ostringstream oss;
      to_stream( oss, p );
      TAO_CONFIG_TEST_ASSERT( oss.str() == str );
   }

   void test_name2()
   {
      const std::string str = "+- ";
      const key_part p( str );
      TAO_CONFIG_TEST_ASSERT( p.kind() == key_kind::name );
      TAO_CONFIG_TEST_ASSERT( p.get_name() == str );
      TAO_CONFIG_TEST_ASSERT( to_string( p ) == '"' + str + '"' );
      std::ostringstream oss;
      to_stream( oss, p );
      TAO_CONFIG_TEST_ASSERT( oss.str() == '"' + str + '"' );
   }

   void test_index()
   {
      const std::size_t ind = 42;
      const key_part p( ind );
      TAO_CONFIG_TEST_ASSERT( p.kind() == key_kind::index );
      TAO_CONFIG_TEST_ASSERT( p.get_index() == ind );
      TAO_CONFIG_TEST_ASSERT( to_string( p ) == "42" );
      std::ostringstream oss;
      to_stream( oss, p );
      TAO_CONFIG_TEST_ASSERT( oss.str() == "42" );
   }

   void unit_test()
   {
      test_name1();
      test_name2();
      test_index();
   }

}  // namespace tao::config

#include "main.hpp"
