// Copyright (c) 2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE2_FUNCTIONS_HPP
#define TAO_CONFIG_INTERNAL_PHASE2_FUNCTIONS_HPP

#include <cstddef>
#include <stdexcept>
#include <string>

#include "array.hpp"
#include "concat.hpp"
#include "config_action.hpp"
#include "config_grammar.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "function_traits.hpp"
#include "object.hpp"
#include "pegtl.hpp"
#include "state.hpp"

namespace tao::config::internal
{
   struct phase2_functions_impl
   {
      phase2_functions_impl( state& st, const function_map& fm )
         : m_state( st ),
           m_functions( fm )
      {}

      [[nodiscard]] std::size_t process()
      {
         for( auto& p : m_state.root.object ) {
            process_concat( p.second );
         }
         return m_changes;
      }

   private:
      state& m_state;
      std::size_t m_changes = 0;
      const function_map& m_functions;

      void process_concat( concat& c )
      {
         for( auto& e : c.concat ) {
            process_entry( e );
         }
      }

      void process_entry( entry& e )
      {
         switch( e.kind() ) {
            case entry_kind::NULL_:
            case entry_kind::BOOLEAN:
            case entry_kind::STRING:
            case entry_kind::BINARY:
            case entry_kind::SIGNED:
            case entry_kind::UNSIGNED:
            case entry_kind::DOUBLE:
               return;
            case entry_kind::ARRAY:
               for( auto& c : e.get_array().array ) {
                  process_concat( c );
               }
               if( !e.get_array().function.empty() ) {
                  process_function( e );
               }
               return;
            case entry_kind::OBJECT:
               for( auto& p : e.get_object().object ) {
                  process_concat( p.second );
               }
               return;
            case entry_kind::ASTERISK:
            case entry_kind::REFERENCE:
               return;
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }

      void process_function( entry& e )
      {
         array& a = e.get_array();

         if( a.function == "parse" ) {
            process_parse_function( e, a );
            ++m_changes;
            return;
         }
         const auto i = m_functions.find( a.function );

         if( i == m_functions.end() ) {
            throw pegtl::parse_error( "unknown function name " + a.function, a.position );
         }
         if( i->second( e ) ) {
            ++m_changes;
         }
      }

      void process_parse_function( entry& e, array& a )
      {
         try {
            state st;
            std::size_t i = 0;
            const std::string s = function_traits< std::string >::get( a, i );
            const key1 k = { key1_part( std::string( "\0", 1 ), a.position ) };
            const key1_guard kg( st, key1( k ) );
            pegtl::string_input< pegtl::tracking_mode::eager, pegtl_input_t::eol_t > in( s, __FUNCTION__ );
            pegtl::parse_nested< rules::value, config_action >( a.position, static_cast< pegtl_input_t& >( in ), st, m_functions );
            assert( st.root.object.size() == 1 );
            assert( st.root.object.begin()->second.concat.size() == 1 );
            e = st.root.object.begin()->second.concat.front();  // TODO: This is slightly hack-ish.
         }
         catch( const arguments_unready& ) {
         }
      }
   };

   [[nodiscard]] inline std::size_t phase2_functions( state& st, const function_map& fm )
   {
      return phase2_functions_impl( st, fm ).process();
   }

}  // namespace tao::config::internal

#endif
