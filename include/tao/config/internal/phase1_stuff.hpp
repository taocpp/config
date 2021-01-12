// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_STUFF_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_STUFF_HPP

#include <cassert>
#include <type_traits>

namespace tao::config::internal
{
   enum class phase1_stuff : char
   {
      remove_all,
      ensure_array,
      ensure_object,
      make_permanent,
      make_temporary
   };

}  // namespace tao::config::internal

#endif
