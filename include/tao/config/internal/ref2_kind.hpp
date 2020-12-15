// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_REF2_KIND_HPP
#define TAO_CONFIG_INTERNAL_REF2_KIND_HPP

namespace tao::config::internal
{
   enum class ref2_kind : char
   {
      minus = 1,  // TODO: Remove?
      name = 2,
      index = 3,
      vector = 4
   };

}  // namespace tao::config::internal

#endif
