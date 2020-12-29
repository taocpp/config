// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_KEY_KIND_HPP
#define TAO_CONFIG_KEY_KIND_HPP

namespace tao::config
{
   enum class key_kind : char
   {
      minus = 0,  // TODO: Remove?
      name = 1,
      index = 2
   };

}  // namespace tao::config

#endif
