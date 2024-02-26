// Copyright (c) 2022-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config.hpp>
#include <tao/config/contrib/rot13.hpp>

namespace tao::config
{
   void unit_test()
   {
      parser p;
      p.set_inner_extension( "rot13", rot13 );
      p.parse( "a = (rot13 \"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@[]{}`\")", __FUNCTION__ );
      const auto j = p.result< json::traits >();

      TAO_CONFIG_TEST_ASSERT( j.at( "a" ) == "0123456789nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM@[]{}`" );
   }

}  // namespace tao::config

#include "main.hpp"
