// Copyright (c) 2020-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <type_traits>

#include "test.hpp"

#include <tao/config/internal/array.hpp>
#include <tao/config/internal/entry.hpp>
#include <tao/config/internal/forward.hpp>
#include <tao/config/internal/json.hpp>
#include <tao/config/internal/key1.hpp>
#include <tao/config/internal/object.hpp>
#include <tao/config/internal/reference2.hpp>

#include <tao/config/key.hpp>

namespace tao::config
{
   void unit_test()
   {
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( key_kind::name ), key_part::data_t >, std::string > );
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( key_kind::index ), key_part::data_t >, std::size_t > );

      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::entry_kind::ARRAY ), internal::entry::data_t >, internal::array > );
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::entry_kind::OBJECT ), internal::entry::data_t >, internal::object > );
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::entry_kind::REFERENCE ), internal::entry::data_t >, internal::reference2 > );

      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::key1_kind::name ), internal::key1_part::data_t >, std::string > );
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::key1_kind::index ), internal::key1_part::data_t >, std::size_t > );
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::key1_kind::asterisk ), internal::key1_part::data_t >, internal::part_asterisk_t > );

      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::reference2_kind::name ), internal::reference2_part::data_t >, std::string > );
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::reference2_kind::index ), internal::reference2_part::data_t >, std::size_t > );
      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( internal::reference2_kind::vector ), internal::reference2_part::data_t >, std::vector< internal::reference2_part > > );
   }

}  // namespace tao::config

#include "main.hpp"
