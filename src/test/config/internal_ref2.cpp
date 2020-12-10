// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <cassert>

#include <tao/config/internal/ref2.hpp>

namespace tao::config
{
   void unit_test()
   {
      internal::ref2 k( "(foo.(bar.1).42.-)" );
      assert( k.size() == 4 );
      assert( k[ 0 ].kind() == internal::ref2_kind::name );
      assert( k[ 0 ].get_name() == "foo" );
      assert( k[ 1 ].kind() == internal::ref2_kind::vector );
      const auto& v = k[ 1 ].get_vector();
      assert( v.size() == 2 );
      assert( v[ 0 ].kind() == internal::ref2_kind::name );
      assert( v[ 0 ].get_name() == "bar" );
      assert( v[ 1 ].kind() == internal::ref2_kind::index );
      assert( v[ 1 ].get_index() == 1 );
      assert( k[ 2 ].kind() == internal::ref2_kind::index );
      assert( k[ 2 ].get_index() == 42 );
      assert( k[ 3 ].kind() == internal::ref2_kind::minus );
   }

}  // tao::config

int main()
{
   tao::config::unit_test();
   return 0;
}
