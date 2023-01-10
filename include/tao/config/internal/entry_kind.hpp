// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_KIND_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_KIND_HPP

namespace tao::config::internal
{
   enum class entry_kind : char
   {
      value = 0,
      reference = 1,
      array = 2,
      object = 3,
      concat = 4
   };

}  // namespace tao::config::internal

#endif
