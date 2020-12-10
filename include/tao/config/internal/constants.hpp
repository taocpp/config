// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONSTANTS_HPP
#define TAO_CONFIG_INTERNAL_CONSTANTS_HPP

namespace tao::config::internal
{
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

   constexpr part_star_t part_star{ 0 };
   constexpr part_minus_t part_minus{ 0 };
   constexpr part_vector_t part_vector{ 0 };

}  // namespace tao::config::internal

#endif
