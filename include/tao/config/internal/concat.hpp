// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONCAT_HPP
#define TAO_CONFIG_INTERNAL_CONCAT_HPP

#include <list>
#include <utility>

#include "entry_kind.hpp"
#include "forward.hpp"
#include "json.hpp"
#include "key1.hpp"
#include "pegtl.hpp"
#include "reverse.hpp"

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

      void operator=( basic_concat&& ) = delete;
      void operator=( const basic_concat& ) = delete;

      [[nodiscard]] bool is_simple() const noexcept
      {
         return ( concat.size() == 1 ) && concat.back.is_value();
      }

      [[nodiscard]] std::size_t all_references() const noexcept
      {
         std::size_t result = 0;

         for( const auto& e : concat ) {
            result += e.all_references();
         }
         return result;
      }

      [[nodiscard]] const json_t* get_value() const
      {
         const json_t* result = nullptr;

         for( const auto& e : reverse( concat ) ) {
            switch( e.kind() ) {
               case entry_kind::value:
                  if( result != nullptr ) {
                     return nullptr;
                  }
                  result = &e.get_value();
                  continue;
               case entry_kind::reference:
                  assert( false );  // UNREACHABLE
               case entry_kind::array:
                  throw std::string( "array in reference" );
               case entry_kind::object:
                  throw std::string( "object in reference" );
               case entry_kind::remove:
                  return result;
            }
            assert( false );  // UNREACHABLE
         }
         return result;
      }

      void back_ensure_kind( const entry_kind k, const pegtl::position& p )
      {
         if( concat.empty() || ( concat.back().kind() != k ) ) {
            concat.emplace_back( k, p );
         }
      }

      void post_insert_merge( const iterator_t& i )
      {
         if( i != concat.begin() ) {
            auto j = i;
            post_function_merge( --j, i );
         }
      }

      void post_append_merge()
      {
         if( concat.size() >= 2 ) {
            post_function_merge( ----concat.end(), --concat.end() );
         }
      }

      std::list< E > concat;
      pegtl::position position;

   private:
      void post_function_merge( const typename std::list< E >::iterator& e, const typename std::list< E >::iterator& f )
      {
         if( f->kind() == entry_kind::remove ) {
            concat.erase( concat.begin(), f );
            return;
         }
         if( e->kind() == f->kind() ) {
            switch( e->kind() ) {
               case entry_kind::value:
               case entry_kind::reference:
                  return;
               case entry_kind::array:
                  e->get_array().array.splice( e->get_array().array.end(), f->get_array().array );
                  concat.erase( f );
                  return;
               case entry_kind::object:
                  for( std::pair< const std::string, basic_concat >& n : f->get_object().object ) {
                     const auto p = e->get_object().object.try_emplace( std::move( n.first ), std::move( n.second ) );
                     if( !p.second ) {
                        for( entry& m : n.second.concat ) {
                           p.first->second.concat.emplace_back( m );
                           p.first->second.post_append_merge();
                        }
                     }
                  }
                  concat.erase( f );
                  return;
               case entry_kind::remove:
                  assert( false );  // UNREACHABLE
            }
            assert( false );  // UNREACHABLE
         }
      }
   };

}  // namespace tao::config::internal

#endif
