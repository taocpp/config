// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_CALL_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_CALL_HPP

#include <cstddef>
#include <stdexcept>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "extension_maps.hpp"
#include "object.hpp"

namespace tao::config::internal
{
   struct phase2_call_impl
   {
      phase2_call_impl( object& root, const extension_maps& em )
         : m_root( root ),
           m_maps( em )
      {}

      [[nodiscard]] std::size_t process()
      {
         for( auto& p : m_root.object ) {
            process_concat( p.second );
         }
         return m_changes;
      }

   private:
      object& m_root;
      std::size_t m_changes = 0;
      const extension_maps& m_maps;

      void process_concat( concat& c )
      {
         for( auto& e : c.concat ) {
            process_entry( e );
         }
      }

      void process_entry( entry& e )
      {
         switch( e.kind() ) {
            case entry_kind::value:
               return;
            case entry_kind::function:
               process_function( e );
               return;
            case entry_kind::reference:
               return;
            case entry_kind::array:
               for( auto& c : e.get_array().array ) {
                  process_concat( c );
               }
               return;
            case entry_kind::object:
               for( auto& p : e.get_object().object ) {
                  process_concat( p.second );
               }
               return;
            case entry_kind::concat:
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }

      void process_function( entry& e )
      {
         function& f = e.get_function();

         for( auto& c : f.array ) {
            process_concat( c );
         }
         const auto i = m_maps.functions.find( f.name );

         if( i == m_maps.functions.end() ) {
            throw pegtl::parse_error( "unknown function name " + f.name, f.position );
         }
         if( i->second( e ) ) {
            ++m_changes;
         }
      }
   };

   [[nodiscard]] inline std::size_t phase2_call( object& root, const extension_maps& em )
   {
      return phase2_call_impl( root, em ).process();
   }

}  // namespace tao::config::internal

#endif
