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

      basic_concat() = default;
      // basic_concat() = delete;

      // explicit basic_concat( const pegtl::position& p )
      //    : position( p )
      // {}

      basic_concat( basic_concat&& ) = default;
      basic_concat( const basic_concat& ) = default;

      ~basic_concat() = default;

      void operator=( basic_concat&& ) = delete;
      void operator=( const basic_concat& ) = delete;

      void back_ensure_kind( const entry_kind k )
      {
         if( concat.empty() || ( concat.back().kind() != k ) ) {
            concat.emplace_back( k );
         }
      }

      void back_aggregate_merge()
      {
         if( concat.size() >= 2 ) {
            E& e = *----concat.end();
            E& f = *--concat.end();
            if( e.kind() == f.kind() ) {
               switch( e.kind() ) {
                  case entry_kind::value:
                  case entry_kind::reference:
                     return;
                  case entry_kind::array:
                     e.get_array().array.splice( e.get_array().array.end(), f.get_array().array );
                     concat.pop_back();  // Invalidates f.
                     return;
                  case entry_kind::object:
                     for( std::pair< std::string, basic_concat >& n : f.get_object().object ) {
                        const auto p = e.get_object().object.try_emplace( std::move( n.first ), std::move( n.second ) );
                        if( !p.second ) {
                           for( entry& m : n.second.concat ) {
                              p.first->second.concat.emplace_back( m );
                              p.first->second.concat.post_aggregate_merge();
                           }
                        }
                     }
                     concat.pop_back();  // Invalidates f.
                     return;
               }
               assert( false );  // UNREACHABLE
            }
         }
      }

      std::list< E > concat;
      //      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
