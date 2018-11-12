// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include <sstream>
#include <stdexcept>

#include "access.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "pegtl.hpp"
#include "state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Rule >
         struct action
            : public pegtl::nothing< Rule >
         {
         };

         template<>
         struct action< rules::null_s >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->v.emplace_back( entry::make_atom( in.position(), json::null ) );
            }
         };

         template<>
         struct action< rules::true_s >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->v.emplace_back( entry::make_atom( in.position(), true ) );
            }
         };

         template<>
         struct action< rules::false_s >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.lstack.back()->v.emplace_back( entry::make_atom( in.position(), false ) );
            }
         };

         template<>
         struct action< rules::identifier >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( in.string() );
            }
         };

         template<>
         struct action< rules::index >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( std::stoul( in.string() ) );
            }
         };

         template<>
         struct action< rules::star >
         {
            static void apply0( state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( true );  // See token_from_value() in utility.hpp -> true -> token::star.
            }
         };

         template<>
         struct action< rules::minus >
         {
            static void apply0( state& st )
            {
               assert( !st.rstack.empty() );
               assert( st.rstack.back()->is_array() );

               st.rstack.back()->emplace_back( false );  // See token_from_value() in utility.hpp -> false -> token::minus.
            }
         };

         template<>
         struct action< rules::plus_equals >
         {
            static void apply0( state& st )
            {
               st.clear_for_assign = false;
            }
         };

         template<>
         struct action< rules::equals >
         {
            static void apply0( state& st )
            {
               st.clear_for_assign = true;
            }
         };

         template<>
         struct action< rules::ext_name >
         {
            template< typename Input >
            static void apply( const Input& in, state& st )
            {
               st.extension = in.string();
            }
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
