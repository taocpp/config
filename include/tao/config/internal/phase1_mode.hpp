// Copyright (c) 2021-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_MODE_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_MODE_HPP

namespace tao::config::internal
{
   enum class phase1_mode : bool
   {
      implicit = true,
      manifest = false  // Can't use explicit as enum label.
   };

}  // namespace tao::config::internal

#endif
