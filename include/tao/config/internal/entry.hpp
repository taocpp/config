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
#include "constants.hpp"
#include "entry_kind.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "object.hpp"
#include "pegtl.hpp"
#include "reference2.hpp"

namespace tao::config::internal
{
   struct entry
   {
      using data_t = std::variant< json_t, reference2, array, object, entry_remove_t >;

      static_assert( std::is_same_v< std::variant_alternative_t< std::size_t( entry_kind::value ), data_t >, json_t > );
      // TODO: All of these and everywhere?

      explicit entry( const json_t& j )
         : m_data( j )
      {
         expand();  // Recursively convert JSON Value class arrays and objects into config::internal arrays and objects with alternating concat/entry structure.
      }

      explicit entry( const reference2& r )
         : m_data( r )
      {
         assert( !r.empty() );
      }

      entry( const entry_kind k, const pegtl::position& p )
         : m_data( std::in_place_type_t< entry_remove_t >(), 42 )
      {
         switch( k ) {
            case entry_kind::value:
            case entry_kind::reference:
               throw std::string( "this should never happen" );
            case entry_kind::array:
               set_array( p );
               return;
            case entry_kind::object:
               set_object( p );
               return;
            case entry_kind::remove:
               return;
         }
         assert( false );  // UNREACHABLE
      }

      entry( entry&& ) = default;
      entry( const entry& ) = default;

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
         return std::holds_alternative< reference2 >( m_data );
      }

      bool is_array() const noexcept
      {
         return std::holds_alternative< array >( m_data );
      }

      bool is_object() const noexcept
      {
         return std::holds_alternative< object >( m_data );
      }

      void set_array( const pegtl::position& p )
      {
         m_data.emplace< std::size_t( entry_kind::array ) >( p );
      }

      void set_object( const pegtl::position& p )
      {
         m_data.emplace< std::size_t( entry_kind::object ) >( p );
      }

      void set_remove()
      {
         m_data.emplace< std::size_t( entry_kind::remove ) >( 42 );
      }

      json_t& get_value() noexcept
      {
         auto* s = std::get_if< json_t >( &m_data );
         assert( s );
         return *s;
      }

      reference2& get_reference() noexcept
      {
         auto* s = std::get_if< reference2 >( &m_data );
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

      const reference2& get_reference() const noexcept
      {
         const auto* s = std::get_if< reference2 >( &m_data );
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

      [[nodiscard]] std::size_t all_references() const noexcept
      {
         switch( kind() ) {
            case entry_kind::value:
               return 0;
            case entry_kind::reference:
               return 1;
            case entry_kind::array:
               return get_array().all_references();
            case entry_kind::object:
               return get_object().all_references();
            case entry_kind::remove:
               return 0;  // TODO: Do we need to exclude or eliminate things in a concat before a remove?
         }
         assert( false );  // UNREACHABLE
      }

      //      const pegtl::position position;

   private:
      std::variant< json_t, reference2, array, object, entry_remove_t > m_data;

      void expand()
      {
         if( get_value().is_array() ) {
            auto a = std::move( get_value().get_array() );
            set_array( get_value().position );
            for( auto& j : a ) {
               get_array().array.emplace_back( j.position ).concat.emplace_back( std::move( j ) );  // TODO: Add remove entry in front of every data entry?
            }
            return;
         }
         if( get_value().is_object() ) {
            auto o = std::move( get_value().get_object() );
            set_object( get_value().position );
            for( auto& [ k, v ] : o ) {
               get_object().object.try_emplace( std::move( k ), v.position ).first->second.concat.emplace_back( std::move( v ) );  // TODO: Add remove entry in front of every data entry?
            }
            return;
         }
      }
   };

}  // namespace tao::config::internal

#endif
