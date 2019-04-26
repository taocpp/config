// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONTROL_HPP
#define TAO_CONFIG_INTERNAL_CONTROL_HPP

#include "assign.hpp"
#include "entry.hpp"
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
         struct control
            : public pegtl::normal< Rule >
         {
         };

         template<>
         struct control< rules::pointer >
            : public pegtl::normal< rules::pointer >
         {
            template< typename Input >
            static void start( const Input&, state& st )
            {
               assert( st.rstack.empty() );

               st.temporary = json::empty_array;
               st.rstack.emplace_back( &st.temporary );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.pop_back();
            }

            template< typename Input >
            static void failure( const Input&, state& st )
            {
               assert( !st.rstack.empty() );

               st.temporary.discard();
               st.rstack.pop_back();
            }
         };

         template<>
         struct control< rules::ref_body >
            : public pegtl::normal< rules::ref_body >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               if( st.rstack.empty() ) {
                  st.rstack.emplace_back( &st.lstack.back()->emplace_back_reference( in.position(), json::empty_array ) );
               }
               else {
                  st.rstack.emplace_back( &st.rstack.back()->emplace_back( json::empty_array ) );
               }
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.rstack.empty() );

               st.rstack.pop_back();
            }
         };

         template<>
         struct control< rules::element_value_list >
            : public pegtl::normal< rules::element_value_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.clear );
               assert( !st.astack.empty() );

               st.lstack.emplace_back( &st.astack.back()->emplace_back( in.position() ) );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.astack.empty() );
               assert( !st.lstack.empty() );

               st.lstack.pop_back();
            }
         };

         template<>
         struct control< rules::element_list >
            : public pegtl::normal< rules::element_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.astack.emplace_back( &st.lstack.back()->emplace_back_array( in.position() ) );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.lstack.empty() );
               assert( !st.astack.empty() );

               st.astack.pop_back();
               st.lstack.back()->post_array_merge();
            }
         };

         template<>
         struct control< rules::member_value_list >
            : public pegtl::normal< rules::member_value_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.ostack.empty() );

               const auto pos = in.position();

               st.lstack.emplace_back( &assign( pos, *st.ostack.back(), key_from_value( pos, st.temporary ) ) );
               st.lstack.back()->clear( st.clear );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.ostack.empty() );
               assert( !st.lstack.empty() );

               st.lstack.pop_back();
            }
         };

         template<>
         struct control< rules::member_list >
            : public pegtl::normal< rules::member_list >
         {
            template< typename Input >
            static void start( const Input& in, state& st )
            {
               assert( !st.lstack.empty() );

               st.ostack.emplace_back( &st.lstack.back()->emplace_back_object( in.position() ) );
            }

            template< typename Input >
            static void success( const Input&, state& st )
            {
               assert( !st.lstack.empty() );
               assert( !st.ostack.empty() );

               st.ostack.pop_back();
               st.lstack.back()->post_object_merge();
            }
         };

         template<>
         struct control< rules::member_array_list >
            : public control< rules::member_value_list >
         {
         };

         template<>
         struct control< rules::member_object_list >
            : public control< rules::member_value_list >
         {
         };

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
