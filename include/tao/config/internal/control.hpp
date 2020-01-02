// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_CONTROL_HPP
#define TAO_CONFIG_INTERNAL_CONTROL_HPP

#include "entry.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "key_utility.hpp"
#include "pegtl.hpp"
#include "phase1_assign.hpp"
#include "state.hpp"

namespace tao::config::internal
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
      static void start( const Input& in, state& st )
      {
         assert( st.rstack.empty() );

         st.temporary.assign( json::empty_array, in.position() );
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

         st.temporary.set_uninitialized();
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
         json_t tmp( json::empty_array, in.position() );
         if( st.rstack.empty() ) {
            st.rstack.emplace_back( &st.lstack.back()->emplace_back_reference( std::move( tmp ) ) );
         }
         else {
            st.rstack.emplace_back( &st.rstack.back()->emplace_back( std::move( tmp ) ) );
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

         st.lstack.emplace_back( &phase1_assign( pos, *st.ostack.back(), value_to_key( st.temporary ) ) );
         st.lstack.back()->do_clear( st.clear );
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

}  // namespace tao::config::internal

#endif
