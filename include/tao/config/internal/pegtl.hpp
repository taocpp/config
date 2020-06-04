// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PEGTL_HPP
#define TAO_CONFIG_INTERNAL_PEGTL_HPP

#include "../external/pegtl.hpp"

namespace tao::config
{
   namespace pegtl = json::pegtl;

   using pegtl_input_t = pegtl::memory_input< pegtl::tracking_mode::eager, pegtl::eol::lf_crlf >;

}  // namespace tao::config

#endif
