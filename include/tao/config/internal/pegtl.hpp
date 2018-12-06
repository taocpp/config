// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PEGTL_HPP
#define TAO_CONFIG_INTERNAL_PEGTL_HPP

#include "../external/pegtl.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         namespace pegtl = json_pegtl;

         using position = pegtl::position;

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
