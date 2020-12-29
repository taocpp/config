// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config/internal/config_action.hpp>
#include <tao/config/internal/config_grammar.hpp>
#include <tao/config/internal/config_parser.hpp>
#include <tao/config/internal/to_stream.hpp>

namespace tao::config
{
   void unit_test()
   {
      const std::string data = "foo = 1, bar = [ 0, 1, 2 ]";
      internal::config_parser cfg;
      internal::pegtl_input_t in( data, __FUNCTION__ );
      const bool result = pegtl::parse< internal::rules::config_file, internal::config_action >( in, cfg.st, cfg.em );
      TAO_CONFIG_TEST_ASSERT( result );
   }

}  // namespace tao::config

#include "main.hpp"
