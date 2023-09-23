// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_TRY_CATCH_HPP
#define TAO_CONFIG_INTERNAL_TRY_CATCH_HPP

#include <iostream>
#include <stdexcept>

#include <tao/pegtl/contrib/nested_exceptions.hpp>

#include "pegtl.hpp"
#include "reverse.hpp"

namespace tao::config::internal
{
   template< typename F >
   void try_catch( const F& f )
   {
      try {
         f();
      }
      catch( const pegtl::parse_error& ) {
         std::cerr << "*** config parse error ***" << std::endl;
         for( const auto& e : pegtl::nested::flatten() ) {
            std::cerr << e.message() << ": " << e.position_string() << std::endl;
         }
      }
      catch( const std::exception& e ) {
         std::cerr << "*** config error ***" << std::endl;
         std::cerr << e.what() << std::endl;
      }
      catch( const std::string& s ) {
         std::cerr << "*** config error ***" << std::endl;
         std::cerr << s << std::endl;
      }
   }

}  // namespace tao::config::internal

#endif
