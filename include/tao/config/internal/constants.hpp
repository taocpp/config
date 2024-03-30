// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONSTANTS_HPP
#define TAO_CONFIG_INTERNAL_CONSTANTS_HPP

#include "entry_kind.hpp"

namespace tao::config::internal
{
   struct part_asterisk_t
   {
      explicit constexpr part_asterisk_t( int /*unused*/ ) {}
   };

   struct part_vector_t
   {
      explicit constexpr part_vector_t( int /*unused*/ ) {}
   };

   constexpr part_asterisk_t part_asterisk{ 0 };
   constexpr part_vector_t part_vector{ 0 };

   [[nodiscard]] constexpr bool operator<( const part_asterisk_t, const part_asterisk_t ) noexcept
   {
      return false;
   }

   struct array_init_t
   {
      explicit constexpr array_init_t( int /*unused*/ ) {}

      static constexpr entry_kind kind = entry_kind::ARRAY;
   };

   struct object_init_t
   {
      explicit constexpr object_init_t( int /*unused*/ ) {}

      static constexpr entry_kind kind = entry_kind::OBJECT;
   };

   struct asterisk_init_t
   {
      explicit constexpr asterisk_init_t( int /*unused*/ ) {}

      static constexpr entry_kind kind = entry_kind::ASTERISK;
   };

   constexpr array_init_t array_init{ 0 };
   constexpr object_init_t object_init{ 0 };
   constexpr asterisk_init_t asterisk_init{ 0 };

}  // namespace tao::config::internal

#endif
