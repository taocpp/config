// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include "test.hpp"

#include <tao/config/internal/action.hpp>
#include <tao/config/internal/grammar.hpp>
#include <tao/config/internal/state.hpp>

namespace tao::config
{
   void unit_test()
   {
      const std::string data = "dummy = 1";
      internal::state st;
      internal::pegtl_input_t in( data, __FUNCTION__ );
      const bool result = internal::pegtl::parse< internal::rules::config_file, internal::action >( in, st );
      std::cerr << result << std::endl;
   }

}  // tao::config

#include "main.hpp"
