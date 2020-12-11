// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <list>

#include "forward.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "ref2.hpp"

namespace tao::config::internal
{
   template< typename E >
   struct basic_concat
   {
      using data_t = std::list< E >;

      explicit basic_concat( const pegtl::position& p )
         : position( p )
      {}

      basic_concat( basic_concat&& ) = delete;
      basic_concat( const basic_concat& ) = delete;

      ~basic_concat() = default;

      void operator=( basic_concat&& ) = delete;
      void operator=( const basic_concat& ) = delete;

      std::list< E > concat;
      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
