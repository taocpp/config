// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSION_MAPS_HPP
#define TAO_CONFIG_INTERNAL_EXTENSION_MAPS_HPP

#include <utility>

#include "extension_types.hpp"

namespace tao::config::internal
{
   struct extension_maps
   {
      extension_maps() = delete;

      extension_maps( value_extension_map&& inner, member_extension_map&& member, member_extension_map&& value )
         : inner( std::move( inner ) ),
           member( std::move( member ) ),
           value( std::move( value ) )
      {}

      extension_maps( extension_maps&& ) = delete;
      extension_maps( const extension_maps& ) = delete;

      ~extension_maps() = default;

      void operator=( extension_maps&& ) = delete;
      void operator=( const extension_maps& ) = delete;

      value_extension_map inner;
      member_extension_map member;
      member_extension_map value;
   };

}  // namespace tao::config::internal

#endif
