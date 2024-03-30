// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ARRAY_HPP
#define TAO_CONFIG_INTERNAL_ARRAY_HPP

#include <list>
#include <string>

#include "forward.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename C >
   struct basic_array
   {
      using data_t = std::list< C >;

      basic_array() = delete;

      explicit basic_array( const pegtl::position& p )
         : position( p )
      {}

      explicit basic_array( const std::string& f, const pegtl::position& p )
         : function( f ),
           position( p )
      {}

      basic_array( basic_array&& ) = default;
      basic_array( const basic_array& ) = default;

      ~basic_array() = default;

      basic_array& operator=( basic_array&& ) = default;
      basic_array& operator=( const basic_array& ) = default;

      [[nodiscard]] const pegtl::position& get_position() const noexcept
      {
         return position;
      }

      std::string function;
      std::list< C > array;
      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
