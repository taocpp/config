// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_HPP

#include <cassert>
#include <string>
#include <variant>

#include "array.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "pegtl.hpp"
#include "ref2.hpp"

namespace tao::config::internal
{
   struct entry
   {
      using data_t = std::variant< json_t, ref2, array, object >;

      entry( const pegtl::position& p, const ref2& r )
         : position( p ),
           m_data( r )
      {}

      entry( const pegtl::position& p, const json_t& j )
         : position( p ),
           m_data( j )
      {}

      entry( const pegtl::position& p, const json::empty_array_t /*unused*/ )
         : position( p ),
           m_data( std::in_place_type_t< array >(), p )
      {}

      entry( const pegtl::position& p, const json::empty_object_t /*unused*/ )
         : position( p ),
           m_data( std::in_place_type_t< object >(), p )
      {}

      entry( entry&& ) = delete;
      entry( const entry& ) = delete;

      ~entry() = default;

      void operator=( entry&& ) = delete;
      void operator=( const entry& ) = delete;

      bool is_value() const noexcept
      {
         return std::holds_alternative< json_t >( m_data );
      }

      bool is_reference() const noexcept
      {
         return std::holds_alternative< ref2 >( m_data );
      }

      bool is_array() const noexcept
      {
         return std::holds_alternative< array >( m_data );
      }

      bool is_object() const noexcept
      {
         return std::holds_alternative< object >( m_data );
      }

      json_t& get_value() noexcept
      {
         auto* s = std::get_if< json_t >( &m_data );
         assert( s );
         //  assert( s->clear == m_clear );  // TODO: Where, and why exactly, is this violated?
         return *s;
      }

      ref2& get_reference() noexcept
      {
         auto* s = std::get_if< ref2 >( &m_data );
         assert( s );
         return *s;
      }

      array& get_array() noexcept
      {
         auto* s = std::get_if< array >( &m_data );
         assert( s );
         return *s;
      }

      object& get_object() noexcept
      {
         auto* s = std::get_if< object >( &m_data );
         assert( s );
         return *s;
      }

      const json_t& get_value() const noexcept
      {
         const auto* s = std::get_if< json_t >( &m_data );
         assert( s );
         return *s;
      }

      const ref2& get_reference() const noexcept
      {
         const auto* s = std::get_if< ref2 >( &m_data );
         assert( s );
         return *s;
      }

      const array& get_array() const noexcept
      {
         const auto* s = std::get_if< array >( &m_data );
         assert( s );
         return *s;
      }

      const object& get_object() const noexcept
      {
         const auto* s = std::get_if< object >( &m_data );
         assert( s );
         return *s;
      }

      const pegtl::position position;

   private:
      std::variant< json_t, ref2, array, object > m_data;
   };

}  // namespace tao::config::internal

#endif
