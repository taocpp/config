// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_STATISTICS_HPP
#define TAO_CONFIG_INTERNAL_STATISTICS_HPP

#include <cstddef>
#include <cstdlib>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "object.hpp"

namespace tao::config::internal
{
   struct statistics
   {
      statistics() noexcept = default;

      explicit statistics( const entry& e )
      {
         count( e );
      }

      explicit statistics( const concat& c )
      {
         count( c );
      }

      explicit statistics( const array& a )
      {
         count( a );
      }

      void count( const entry& e )
      {
         switch( e.kind() ) {
            case entry_kind::NULL_:
               ++m_nulls;
               return;
            case entry_kind::BOOLEAN:
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               ++m_atoms;
               return;
            case entry_kind::ARRAY:
               count( e.get_array() );
               return;
            case entry_kind::OBJECT:
               count( e.get_object() );
               return;
            case entry_kind::ASTERISK:
               ++m_asterisks;
               count( e.get_asterisk() );
               return;
            case entry_kind::REFERENCE:
               ++m_references;
               return;
         }
         std::abort();  // LCOV_EXCL_LINE
      }

      void count( const array& a )
      {
         if( a.function.empty() ) {
            ++m_arrays;
         }
         else {
            ++m_functions;
         }
         for( const auto& c : a.array ) {
            count( c );
         }
      }

      void count( const object& o )
      {
         for( const auto& p : o.object ) {
            count( p.second );
         }
         ++m_objects;
      }

      void count( const concat& c )
      {
         if( c.concat.size() > 1 ) {
            ++m_additions;
         }
         for( const auto& e : c.concat ) {
            count( e );
         }
      }

      [[nodiscard]] std::size_t nulls() const noexcept
      {
         return m_nulls;
      }

      [[nodiscard]] std::size_t references() const noexcept
      {
         return m_references;
      }

      [[nodiscard]] bool is_primitive() const noexcept
      {
         return ( m_functions == 0 ) && ( m_additions == 0 ) && ( m_asterisks == 0 ) && ( m_references == 0 );
      }

   private:
      std::size_t m_nulls = 0;
      std::size_t m_atoms = 0;
      std::size_t m_arrays = 0;
      std::size_t m_objects = 0;
      std::size_t m_functions = 0;
      std::size_t m_additions = 0;
      std::size_t m_asterisks = 0;
      std::size_t m_references = 0;
   };

}  // namespace tao::config::internal

#endif
