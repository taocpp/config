// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_HPP

#include <cassert>
#include <string>
#include <utility>
#include <variant>

#include "array.hpp"
#include "concat.hpp"
#include "entry_kind.hpp"
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

      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( entry_kind::value ), data_t >, json_t > );
      // TODO: All of these and everywhere?

      explicit entry( const ref2& r )
         : m_data( r )
      {
         assert( !r.empty() );
      }

      explicit entry( const json_t& j )
         : m_data( j )
      {
         expand();
      }

      explicit entry( const entry_kind k )
         : m_data( std::in_place_type_t< object >() )
      {
         switch( k ) {
            case entry_kind::value:
            case entry_kind::reference:
               assert( false );  // TODO!
            case entry_kind::array:
               set_array();
               break;
            case entry_kind::object:
               break;
         }
      }

      entry( entry&& ) = delete;
      entry( const entry& ) = delete;

      ~entry() = default;

      void operator=( entry&& ) = delete;
      void operator=( const entry& ) = delete;

      entry_kind kind() const noexcept
      {
         return entry_kind( m_data.index() );
      }

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

      void set_array()
      {
         m_data.emplace< std::size_t( entry_kind::array ) >();
      }

      void set_object()
      {
         m_data.emplace< std::size_t( entry_kind::array ) >();
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

      //      const pegtl::position position;

   private:
      std::variant< json_t, ref2, array, object > m_data;

      void expand()
      {
         if( get_value().is_array() ) {
            auto a = std::move( get_value().get_array() );
            set_array();
            for( auto& j : a ) {
               get_array().array.emplace_back().concat.emplace_back( std::move( j ) );
            }
            return;
         }
         if( get_value().is_object() ) {
            auto o = std::move( get_value().get_object() );
            set_object();
            for( auto& [ k, v ] : o ) {
               get_object().object.try_emplace( std::move( k ) ).first->second.concat.emplace_back( std::move( v ) );
            }
            return;
         }
      }
   };

}  // namespace tao::config::internal

#endif
