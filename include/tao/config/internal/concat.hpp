// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <list>

#include "entry_kind.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "pegtl.hpp"
#include "reverse.hpp"

namespace tao::config::internal
{
   template< typename E >
   struct basic_concat
   {
      using data_t = std::list< E >;

      basic_concat() = default;
      // basic_concat() = delete;

      // explicit basic_concat( const pegtl::position& p )
      //    : position( p )
      // {}

      basic_concat( basic_concat&& ) = delete;
      basic_concat( const basic_concat& ) = delete;

      ~basic_concat() = default;

      void operator=( basic_concat&& ) = delete;
      void operator=( const basic_concat& ) = delete;

      void back_ensure_kind( const entry_kind k )
      {
         if( concat.empty() || ( concat.back().kind() != k ) ) {
            concat.emplace_back( k );
         }
      }

      std::list< E > concat;
      //      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
