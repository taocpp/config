// Copyright (c) 2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_T_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_T_HPP

#include <functional>
#include <string>
#include <map>

#include "pegtl.hpp"

namespace tao::config::internal
{
   struct state;

   using extension_t = std::function< void( pegtl_input_t&, state& ) >;

   using extension_map_t = std::map< std::string, extension_t >;

}  // namespace tao::config::internal

#endif
