// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_KEY_PART_HPP
#define TAO_CONFIG_KEY_PART_HPP

#include <cassert>
#include <string>
#include <variant>

#include "internal/constants.hpp"

#include "key_kind.hpp"

namespace tao::config
{
   struct key_part
   {
      explicit key_part( const internal::part_minus_t t )
         : data( t )
      {}

      explicit key_part( const std::size_t i )
         : data( i )
      {}

      explicit key_part( const std::string& n )
         : data( n )
      {}

      [[nodiscard]] key_kind kind() const noexcept
      {
         return key_kind( data.index() );
      }

      [[nodiscard]] std::size_t get_index() const noexcept
      {
         const auto* s = std::get_if< std::size_t >( &data );
         assert( s != nullptr );
         return *s;
      }

      [[nodiscard]] const std::string& get_name() const noexcept
      {
         const auto* s = std::get_if< std::string >( &data );
         assert( s != nullptr );
         return *s;
      }

      std::variant< internal::part_minus_t, std::string, std::size_t > data;
   };

   inline bool operator<( const key_part& l, const key_part& r ) noexcept
   {
      return l.data < r.data;
   }

}  // namespace tao::config

#endif
