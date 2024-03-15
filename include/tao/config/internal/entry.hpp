// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ENTRY_HPP
#define TAO_CONFIG_INTERNAL_ENTRY_HPP

#include <cassert>
#include <cstddef>
#include <stdexcept>
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
      using data_t = std::variant< json_t, reference2, array, object, concat >;

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

      entry( const std::string& n, const pegtl::position& p )
         : m_data( std::in_place_type_t< array >(), n, p )
      {}

      entry( const entry_kind k, const pegtl::position& p )
         : m_data( std::in_place_type_t< object >(), p )
      {
         switch( k ) {
            case entry_kind::value:
            case entry_kind::reference:
               throw std::string( "this should never happen" );
            case entry_kind::array:
               set_array( p );
               return;
            case entry_kind::object:
               return;
            case entry_kind::concat:
               set_concat( p );
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }

      entry( entry&& ) = default;
      entry( const entry& ) = default;

      ~entry() = default;

      entry& operator=( entry&& ) = default;
      entry& operator=( const entry& ) = default;

      entry_kind kind() const noexcept
      {
         return entry_kind( m_data.index() );
      }

      [[nodiscard]] bool is_value() const noexcept
      {
         return std::holds_alternative< json_t >( m_data );
      }

      [[nodiscard]] bool is_reference() const noexcept
      {
         return std::holds_alternative< reference2 >( m_data );
      }

      [[nodiscard]] bool is_array() const noexcept
      {
         return std::holds_alternative< array >( m_data );
      }

      [[nodiscard]] bool is_object() const noexcept
      {
         return std::holds_alternative< object >( m_data );
      }

      [[nodiscard]] bool is_concat() const noexcept
      {
         return std::holds_alternative< concat >( m_data );
      }

      void set_value( json_t&& t )
      {
         m_data = std::move( t );
         expand();
      }

      void set_value( const json_t& t )
      {
         m_data = t;
         expand();
      }

      void set_array( pegtl::position p )
      {
         m_data.emplace< std::size_t( entry_kind::array ) >( std::move( p ) );
      }

      void set_object( pegtl::position p )
      {
         m_data.emplace< std::size_t( entry_kind::object ) >( std::move( p ) );
      }

      void set_concat( pegtl::position p )
      {
         m_data.emplace< std::size_t( entry_kind::concat ) >( std::move( p ) );
      }

      [[nodiscard]] json_t& get_value() noexcept
      {
         auto* s = std::get_if< json_t >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] reference2& get_reference() noexcept
      {
         auto* s = std::get_if< reference2 >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] array& get_array() noexcept
      {
         auto* s = std::get_if< array >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] object& get_object() noexcept
      {
         auto* s = std::get_if< object >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] concat& get_concat() noexcept
      {
         auto* s = std::get_if< concat >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] const json_t& get_value() const noexcept
      {
         const auto* s = std::get_if< json_t >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] const reference2& get_reference() const noexcept
      {
         const auto* s = std::get_if< reference2 >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] const array& get_array() const noexcept
      {
         const auto* s = std::get_if< array >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] const object& get_object() const noexcept
      {
         const auto* s = std::get_if< object >( &m_data );
         assert( s );
         return *s;
      }

      [[nodiscard]] const concat& get_concat() const noexcept
      {
         const auto* s = std::get_if< concat >( &m_data );
         assert( s );
         return *s;
      }

   private:
      data_t m_data;

      void expand()
      {
         if( get_value().is_array() ) {
            json_t::array_t a = std::move( get_value().get_array() );
            set_array( get_value().position );
            for( json_t& j : a ) {
               concat& d = get_array().array.emplace_back( j.position );
               d.concat.emplace_back( std::move( j ) );
               d.remove = true;  // TODO: Make configurable?
            }
            return;
         }
         if( get_value().is_object() ) {
            json_t::object_t o = std::move( get_value().get_object() );
            set_object( get_value().position );
            for( auto& [ k, j ] : o ) {
               const auto p = get_object().object.try_emplace( std::move( k ), j.position );
               p.first->second.concat.emplace_back( std::move( j ) );
               p.first->second.remove = true;  // TODO: Make configurable?
            }
            return;
         }
      }
   };

}  // namespace tao::config::internal

#endif
