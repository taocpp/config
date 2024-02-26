// Copyright (c) 2023-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <cassert>

#include <tao/config.hpp>

namespace tao
{
   void test1()
   {
      const std::string input = "foo = '''a\nb\nc\n'''";
      const auto config = config::from_string( input, __FUNCTION__ );
      const auto string = config.get_object().at( "foo" );
      assert( string.position.line() == 1 );
      assert( string.get_string() == "a\nb\nc\n" );
   }

   void test2()
   {
      const std::string input = "foo = '''\na\nb\nc\n'''";
      const auto config = config::from_string( input, __FUNCTION__ );
      const auto string = config.get_object().at( "foo" );
      assert( string.position.line() == 1 );  // Should this be 2?
      assert( string.get_string() == "a\nb\nc\n" );
   }

   void test3()
   {
      const std::string input = "\n\n\nfoo = '''a\nb\nc\n'''";
      const auto config = config::from_string( input, __FUNCTION__ );
      const auto string = config.get_object().at( "foo" );
      assert( string.position.line() == 4 );
      assert( string.get_string() == "a\nb\nc\n" );
   }

   void test4()
   {
      const std::string input = "\n\n\nfoo = '''\na\nb\nc\n'''";
      const auto config = config::from_string( input, __FUNCTION__ );
      const auto string = config.get_object().at( "foo" );
      assert( string.position.line() == 4 );  // Should this be 5?
      assert( string.get_string() == "a\nb\nc\n" );
   }

}  // namespace tao

int main()
{
   tao::test1();
   tao::test2();
   tao::test3();
   tao::test4();
   return 0;
}
