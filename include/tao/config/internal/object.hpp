// Copyright (c) 2019-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_OBJECT_HPP
#define TAO_CONFIG_INTERNAL_OBJECT_HPP

#include <cassert>
#include <map>
#include <string>

#include "forward.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename C >
   struct basic_object
   {
      using data_t = std::map< std::string, C >;

      basic_object() = delete;

      explicit basic_object( const pegtl::position& p )
         : position( p )
      {}

      basic_object( basic_object&& ) = default;
      basic_object( const basic_object& ) = default;

      ~basic_object() = default;

      basic_object& operator=( basic_object&& ) = default;
      basic_object& operator=( const basic_object& ) = default;

      [[nodiscard]] std::pair< const std::string, C >* find( const std::string& k ) noexcept
      {
         const auto i = object.find( k );
         return ( i == object.end() ) ? nullptr : ( &*i );
      }

      [[nodiscard]] const std::pair< const std::string, C >* find( const std::string& k ) const noexcept
      {
         const auto i = object.find( k );
         return ( i == object.end() ) ? nullptr : ( &*i );
      }

      [[nodiscard]] std::size_t count_references_recursive() const noexcept
      {
         std::size_t result = 0;

         for( const auto& p : object ) {
            result += p.second.count_references_recursive();
         }
         return result;
      }

      std::map< std::string, C > object;
      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
