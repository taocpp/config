// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ATOM_HPP
#define TAO_CONFIG_INTERNAL_ATOM_HPP

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "pegtl.hpp"

namespace tao::config::internal
{
   struct null
   {
      explicit null( const pegtl::position& pos )
         : position( pos )
      {}

      [[nodiscard]] const pegtl::position& get_position() const noexcept
      {
         return position;
      }

      pegtl::position position;
   };

   template< typename T >
   struct atom
   {
      template< typename V >
      atom( V&& v, const pegtl::position& pos )
         : value( std::forward< V >( v ) ),
           position( pos )
      {}

      [[nodiscard]] const pegtl::position& get_position() const noexcept
      {
         return position;
      }

      T value;
      pegtl::position position;
   };

   using boolean = atom< bool >;
   using string_t = atom< std::string >;
   using binary_t = atom< std::vector< std::byte > >;

   using signed_t = atom< std::int64_t >;
   using unsigned_t = atom< std::uint64_t >;
   using double_t = atom< double >;

}  // namespace tao::config::internal

#endif
