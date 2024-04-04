// Copyright (c) 2021-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <sstream>
#include <string>
#include <string_view>

#include "test.hpp"

#include <tao/config.hpp>
#include <tao/config/internal/to_stream.hpp>

namespace tao::config
{
   void unit_test()
   {
      internal::config_parser cp;
      cp.parse( std::filesystem::path( "tests/debug_traits.config" ) );
      json::events::to_value consumer;
      json::events::produce< internal::debug_traits >( consumer, cp.st.root );
      const json::value config = std::move( consumer.value );
      const json::value output = json::jaxn::from_file( std::filesystem::path( "tests/debug_traits.output" ) );
      TAO_CONFIG_TEST_ASSERT( output == config );
      {
         std::ostringstream os1;
         internal::to_stream( os1, cp.st.root );
         TAO_CONFIG_TEST_ASSERT( json::jaxn::from_string( os1.str() ) == output );
      }
      std::ostringstream os2;
      internal::to_stream( os2, cp.st.root, 3 );
      TAO_CONFIG_TEST_ASSERT( json::jaxn::from_string( os2.str() ) == output );
   }

}  // namespace tao::config

#include "main.hpp"
