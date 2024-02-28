// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_TYPES_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_TYPES_HPP

#include <functional>
#include <map>
#include <string>

#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   using inner_function = std::function< bool( entry& ) >;
   using inner_function_map = std::map< std::string, inner_function >;

   using inner_extension = std::function< json_t( pegtl_input_t&, state&, const extension_maps& ) >;
   using inner_extension_map = std::map< std::string, inner_extension >;

   using outer_extension = std::function< void( pegtl_input_t&, state&, const extension_maps& ) >;
   using outer_extension_map = std::map< std::string, outer_extension >;

}  // namespace tao::config::internal

#endif
