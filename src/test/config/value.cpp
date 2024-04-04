// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <sstream>
#include <string>

#include "test.hpp"

#include <tao/config.hpp>

namespace tao::config
{
   const std::string s = "invalid json type 'object' for conversion to std::string  [(root):1:1]";  // TODO: Where does the second space come from?

   void unit_test()
   {
      const auto v = from_string( "foo = 42", __FUNCTION__ );

      try {
         (void)v.as< std::string >();
         ++failed;
      }
      catch( const std::exception& e ) {
         TAO_CONFIG_TEST_ASSERT( e.what() == s );
      }
   }

}  // namespace tao::config

#include "main.hpp"
