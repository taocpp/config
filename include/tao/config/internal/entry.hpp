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
#include "atom.hpp"
#include "concat.hpp"
#include "constants.hpp"
#include "entry_kind.hpp"
#include "forward.hpp"
#include "object.hpp"
#include "pegtl.hpp"
#include "reference2.hpp"

namespace tao::config::internal
{
   struct entry
   {
      using data_t = std::variant< null, boolean, string_t, binary_t, signed_t, unsigned_t, double_t, array, object, concat, reference2 >;

      explicit entry( const null& n )
         : m_data( n )
      {}

      explicit entry( const boolean& b )
         : m_data( b )
      {}

      explicit entry( const string_t& s )
         : m_data( s )
      {}

      explicit entry( const binary_t& b )
         : m_data( b )
      {}

      explicit entry( const signed_t& s )
         : m_data( s )
      {}

      explicit entry( const unsigned_t& u )
         : m_data( u )
      {}

      explicit entry( const double_t& d )
         : m_data( d )
      {}

      explicit entry( const reference2& r )
         : m_data( r )
      {
         assert( !r.empty() );
      }

      entry( const std::string& n, const pegtl::position& p )
         : m_data( std::in_place_type_t< array >(), n, p )
      {}

      entry( const array_init_t /*unused*/, const pegtl::position& p )
         : m_data( std::in_place_type_t< array >(), p )
      {}

      entry( const object_init_t /*unused*/, const pegtl::position& p )
         : m_data( std::in_place_type_t< object >(), p )
      {}

      entry( const asterisk_init_t /*unused*/, const pegtl::position& p )
         : m_data( std::in_place_type_t< concat >(), p )
      {}

      entry( entry&& ) noexcept = default;
      entry( const entry& ) = default;

      ~entry() = default;

      entry& operator=( entry&& ) noexcept = default;
      entry& operator=( const entry& ) = default;

      [[nodiscard]] entry_kind kind() const noexcept
      {
         return entry_kind( m_data.index() );
      }

      [[nodiscard]] bool is_null() const noexcept
      {
         return std::holds_alternative< null >( m_data );
      }

      [[nodiscard]] bool is_string() const noexcept
      {
         return std::holds_alternative< string_t >( m_data );
      }

      [[nodiscard]] bool is_binary() const noexcept
      {
         return std::holds_alternative< binary_t >( m_data );
      }

      [[nodiscard]] bool is_array() const noexcept
      {
         return std::holds_alternative< array >( m_data );
      }

      [[nodiscard]] bool is_object() const noexcept
      {
         return std::holds_alternative< object >( m_data );
      }

      [[nodiscard]] bool is_asterisk() const noexcept
      {
         return std::holds_alternative< concat >( m_data );
      }

      [[nodiscard]] bool is_reference() const noexcept
      {
         return std::holds_alternative< reference2 >( m_data );
      }

      template< typename T >
      void set_value( atom< T >&& t )
      {
         m_data.emplace< atom< T > >( std::move( t ) );
      }

      void set_array( pegtl::position p )
      {
         m_data.emplace< std::size_t( entry_kind::ARRAY ) >( std::move( p ) );
      }

      void set_object( pegtl::position p )
      {
         m_data.emplace< std::size_t( entry_kind::OBJECT ) >( std::move( p ) );
      }

      [[nodiscard]] bool get_boolean() const noexcept
      {
         return std::get< boolean >( m_data ).value;
      }

      [[nodiscard]] boolean& get_boolean_atom() noexcept
      {
         return std::get< boolean >( m_data );
      }

      [[nodiscard]] const boolean& get_boolean_atom() const noexcept
      {
         return std::get< boolean >( m_data );
      }

      [[nodiscard]] std::string& get_string() noexcept
      {
         return std::get< string_t >( m_data ).value;
      }

      [[nodiscard]] const std::string& get_string() const noexcept
      {
         return std::get< string_t >( m_data ).value;
      }

      [[nodiscard]] string_t& get_string_atom() noexcept
      {
         return std::get< string_t >( m_data );
      }

      [[nodiscard]] const string_t& get_string_atom() const noexcept
      {
         return std::get< string_t >( m_data );
      }

      [[nodiscard]] std::vector< std::byte >& get_binary() noexcept
      {
         return std::get< binary_t >( m_data ).value;
      }

      [[nodiscard]] const std::vector< std::byte >& get_binary() const noexcept
      {
         return std::get< binary_t >( m_data ).value;
      }

      [[nodiscard]] binary_t& get_binary_atom() noexcept
      {
         return std::get< binary_t >( m_data );
      }

      [[nodiscard]] const binary_t& get_binary_atom() const noexcept
      {
         return std::get< binary_t >( m_data );
      }

      [[nodiscard]] std::int64_t get_signed() const noexcept
      {
         return std::get< signed_t >( m_data ).value;
      }

      [[nodiscard]] signed_t& get_signed_atom() noexcept
      {
         return std::get< signed_t >( m_data );
      }

      [[nodiscard]] const signed_t& get_signed_atom() const noexcept
      {
         return std::get< signed_t >( m_data );
      }

      [[nodiscard]] std::uint64_t get_unsigned() const noexcept
      {
         return std::get< unsigned_t >( m_data ).value;
      }

      [[nodiscard]] unsigned_t& get_unsigned_atom() noexcept
      {
         return std::get< unsigned_t >( m_data );
      }

      [[nodiscard]] const unsigned_t& get_unsigned_atom() const noexcept
      {
         return std::get< unsigned_t >( m_data );
      }

      [[nodiscard]] double get_double() const noexcept
      {
         return std::get< double_t >( m_data ).value;
      }

      [[nodiscard]] double_t& get_double_atom() noexcept
      {
         return std::get< double_t >( m_data );
      }

      [[nodiscard]] const double_t& get_double_atom() const noexcept
      {
         return std::get< double_t >( m_data );
      }

      [[nodiscard]] array& get_array() noexcept
      {
         return std::get< array >( m_data );
      }

      [[nodiscard]] const array& get_array() const noexcept
      {
         return std::get< array >( m_data );
      }

      [[nodiscard]] object& get_object() noexcept
      {
         return std::get< object >( m_data );
      }

      [[nodiscard]] const object& get_object() const noexcept
      {
         return std::get< object >( m_data );
      }

      [[nodiscard]] concat& get_asterisk() noexcept
      {
         return std::get< concat >( m_data );
      }

      [[nodiscard]] const concat& get_asterisk() const noexcept
      {
         return std::get< concat >( m_data );
      }

      [[nodiscard]] reference2& get_reference() noexcept
      {
         return std::get< reference2 >( m_data );
      }

      [[nodiscard]] const reference2& get_reference() const noexcept
      {
         return std::get< reference2 >( m_data );
      }

      [[nodiscard]] const pegtl::position& get_position() const noexcept
      {
         return std::visit( []( const auto& v ) -> const pegtl::position& { return v.get_position(); }, m_data );
      }

   private:
      data_t m_data;
   };

}  // namespace tao::config::internal

#endif
