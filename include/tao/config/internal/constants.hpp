// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONSTANTS_HPP
#define TAO_CONFIG_INTERNAL_CONSTANTS_HPP

#include <cstdint>

namespace tao::config::internal
{
   struct temporary_t
   {
      explicit constexpr temporary_t( int /*unused*/ ) {}
   };

   struct part_star_t
   {
      explicit constexpr part_star_t( int /*unused*/ ) {}
   };

   struct part_minus_t
   {
      explicit constexpr part_minus_t( int /*unused*/ ) {}
   };

   struct part_vector_t
   {
      explicit constexpr part_vector_t( int /*unused*/ ) {}
   };

   struct entry_remove_t
   {
      explicit constexpr entry_remove_t( int /*unused*/ ) {}
   };

   struct better_luck_next_time_t
   {
      explicit constexpr better_luck_next_time_t( int /*unused*/ ) {}
   };

   constexpr temporary_t temporary{ 0 };
   constexpr part_star_t part_star{ 0 };
   constexpr part_minus_t part_minus{ 0 };
   constexpr part_vector_t part_vector{ 0 };
   constexpr entry_remove_t entry_remove{ 0 };
   constexpr better_luck_next_time_t better_luck_next_time{ 0 };

   constexpr bool operator<( const part_star_t, const part_star_t ) noexcept
   {
      return false;
   }

   constexpr bool operator<( const part_minus_t, const part_minus_t ) noexcept
   {
      return false;
   }

   constexpr bool operator<( const entry_remove_t, const entry_remove_t ) noexcept
   {
      return false;
   }

}  // namespace tao::config::internal

#endif
