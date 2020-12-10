// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config/internal/key1.hpp>

namespace tao::config
{
   void unit_test()
   {
      internal::key1 k( "foo.bar.42.-.*" );
      assert( k.size() == 5 );
      assert( k[ 0 ].kind() == internal::key1_kind::name );
      assert( k[ 0 ].get_name() == "foo" );
      assert( k[ 1 ].kind() == internal::key1_kind::name );
      assert( k[ 1 ].get_name() == "bar" );
      assert( k[ 2 ].kind() == internal::key1_kind::index );
      assert( k[ 2 ].get_index() == 42 );
      assert( k[ 3 ].kind() == internal::key1_kind::minus );
      assert( k[ 4 ].kind() == internal::key1_kind::star );
   }

}  // tao::config

#include "main.hpp"
