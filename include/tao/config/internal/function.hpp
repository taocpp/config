// Copyright (c) 2019-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_FUNCTION_HPP
#define TAO_CONFIG_INTERNAL_FUNCTION_HPP

#include <cstddef>
#include <list>
#include <string>

#include "forward.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename C >
   struct basic_function
   {
      using data_t = std::list< C >;

      basic_function() = delete;

      basic_function( const std::string& n, const pegtl::position& p )
         : name( n ),
           position( p )
      {}

      basic_function( basic_function&& ) = default;
      basic_function( const basic_function& ) = default;

      ~basic_function() = default;

      basic_function& operator=( basic_function&& ) = default;
      basic_function& operator=( const basic_function& ) = default;

      [[nodiscard]] std::size_t count_references_recursive() const noexcept
      {
         std::size_t result = 0;

         for( const auto& c : array ) {
            result += c.count_references_recursive();
         }
         return result;
      }

      std::string name;
      std::list< C > array;
      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
