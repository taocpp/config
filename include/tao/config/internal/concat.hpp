// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <cassert>
#include <list>

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

      explicit basic_concat( const pegtl::position& p )
         : position( p )
      {}

      basic_concat( basic_concat&& ) = delete;
      basic_concat( const basic_concat& ) = delete;

      ~basic_concat() = default;

      void operator=( basic_concat&& ) = delete;
      void operator=( const basic_concat& ) = delete;

      array& ensure_array()
      {
         if( concat.empty() || ( !concat.back().is_array() ) ) {
            concat.emplace_back( json::empty_array );
         }
         return concat.back().get_array();
      }

      object& ensure_object()
      {
         if( concat.empty() || ( !concat.back().is_object() ) ) {
            concat.emplace_back( json::empty_object );
         }
         return concat.back().get_object();
      }

      std::list< E > concat;
      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
