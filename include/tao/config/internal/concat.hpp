// Copyright (c) 2018-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <iterator>
#include <list>
#include <stdexcept>
#include <utility>

#include "entry_kind.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename E >
   struct basic_concat
   {
      using data_t = std::list< E >;
      using iterator_t = typename std::list< E >::iterator;

      basic_concat() = delete;

      explicit basic_concat( const pegtl::position& p )
         : position( p )
      {}

      basic_concat( basic_concat&& ) = default;
      basic_concat( const basic_concat& ) = default;

      ~basic_concat() = default;

      basic_concat& operator=( basic_concat&& ) = default;
      basic_concat& operator=( const basic_concat& ) = default;

      [[nodiscard]] bool omit_from_final_result() const noexcept
      {
         return implicit || temporary || concat.empty();
      }

      [[nodiscard]] std::size_t count_references_recursive() const noexcept
      {
         std::size_t result = 0;

         for( const auto& e : concat ) {
            result += e.count_references_recursive();
         }
         return result;
      }

      [[nodiscard]] const json_t* get_value() const
      {
         const json_t* result = nullptr;

         for( const auto& e : concat ) {
            switch( e.kind() ) {
               case entry_kind::value:
                  if( result != nullptr ) {
                     return nullptr;
                  }
                  result = &e.get_value();
                  continue;
               case entry_kind::reference:
                  throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
               case entry_kind::array:
                  throw pegtl::parse_error( "array as referenced reference part", e.get_array().position );
               case entry_kind::object:
                  throw pegtl::parse_error( "object as referenced reference part", e.get_object().position );
               case entry_kind::concat:
                  throw pegtl::parse_error( "concat as referenced reference part", e.get_concat().position );
            }
            throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         }
         return result;
      }

      void back_ensure_kind( const entry_kind k, const pegtl::position& p )
      {
         if( concat.empty() || ( concat.back().kind() != k ) ) {
            concat.emplace_back( k, p );
         }
      }

      bool remove = false;     // Whether generated by += (false) or by = (true), because in the latter case everything that comes before must be removed.
      bool implicit = false;   // Whether implicitly generated by a delete, e.g. a.b.c = delete when a.b doesn't even exist, so it's implicitly generated to set the remove flag on a.b.c.
      bool temporary = false;  // Whether flagged as temporary by the user, i.e. this is not to be included in the final result and will be omitted by phase3.

      std::uint64_t generation = 0;

      std::string schema;
      std::list< E > concat;
      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
