// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ACTION_HPP
#define TAO_CONFIG_INTERNAL_ACTION_HPP

#include <sstream>
#include <stdexcept>
#include <string>

#include "binary_state.hpp"
#include "grammar.hpp"
#include "json.hpp"
#include "number_action.hpp"
#include "number_state.hpp"
#include "pegtl.hpp"
#include "state.hpp"
#include "string_state.hpp"

namespace tao::config::internal
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

         st.lstack.back()->emplace_back_atom( in.position(), json::null );
      }
   };

   template<>
   struct action< rules::true_s >
   {
      template< typename Input >
      static void apply( const Input& in, state& st )
      {
         assert( !st.lstack.empty() );

         st.lstack.back()->emplace_back_atom( in.position(), true );
      }
   };

   template<>
   struct action< rules::false_s >
   {
      template< typename Input >
      static void apply( const Input& in, state& st )
      {
         assert( !st.lstack.empty() );

         st.lstack.back()->emplace_back_atom( in.position(), false );
      }
   };

   template<>
   struct action< rules::delete_s >
   {
      template< typename Input >
      static void apply( const Input& in, state& st )
      {
         assert( !st.lstack.empty() );

         st.lstack.back()->emplace_back_atom( in.position(), tao::json::uninitialized );
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

         st.rstack.back()->emplace_back( in.string(), in.position() );
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

         st.rstack.back()->emplace_back( std::stoul( in.string() ), in.position() );
      }
   };

   template<>
   struct action< rules::star >
   {
      template< typename Input >
      static void apply( const Input& in, state& st )
      {
         assert( !st.rstack.empty() );
         assert( st.rstack.back()->is_array() );

         st.rstack.back()->emplace_back( true, in.position() );  // See value_to_part() in key_utility.hpp -> true -> token::star.
      }
   };

   template<>
   struct action< rules::minus >
   {
      template< typename Input >
      static void apply( const Input& in, state& st )
      {
         assert( !st.rstack.empty() );
         assert( st.rstack.back()->is_array() );

         st.rstack.back()->emplace_back( false, in.position() );  // See value_to_part() in key_utility.hpp -> false -> token::minus.
      }
   };

   template<>
   struct action< rules::equals >
   {
      static void apply0( state& st )
      {
         assert( !st.clear );

         st.clear = true;
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

   template<>
   struct action< json::jaxn::internal::rules::sor_value >
      : public pegtl::change_action_and_state< number_action, number_state >
   {
   };

   struct kludge_state
   {
      template< typename Input >
      explicit kludge_state( const Input& in, state& st )
         : pos( in.position() ),
           st( st )
      {
         assert( !st.lstack.empty() );
      }

      kludge_state( const kludge_state& ) = delete;
      kludge_state( kludge_state&& ) = delete;

      ~kludge_state() = default;

      void operator=( const kludge_state& ) = delete;
      void operator=( kludge_state&& ) = delete;

      template< typename Input >
      void success( const Input& /*unused*/, state& /*unused*/ )
      {
      }

      const pegtl::position pos;
      state& st;
   };

   template< typename Rule >
   struct kludge_action
      : public pegtl::nothing< Rule >
   {
   };

   template<>
   struct kludge_action< rules::binary_choice >
      : public pegtl::change_action_and_states< json::jaxn::internal::bunescape_action, std::vector< std::byte > >
   {
      template< typename Input >
      static void success( const Input& /*unused*/, std::vector< std::byte >& value, kludge_state& state )
      {
         state.st.lstack.back()->emplace_back_atom( state.pos, std::move( value ) );
      }
   };

   template<>
   struct kludge_action< rules::string_choice >
      : public pegtl::change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Input >
      static void success( const Input& /*unused*/, std::string& value, kludge_state& state )
      {
         state.st.lstack.back()->emplace_back_atom( state.pos, std::move( value ) );
      }
   };

   template<>
   struct action< rules::binary_choice >
      : public pegtl::change_action_and_state< kludge_action, kludge_state >
   {
   };

   template<>
   struct action< rules::string_choice >
      : public pegtl::change_action_and_state< kludge_action, kludge_state >
   {
   };

   template<>
   struct action< rules::quoted_choice >
      : public pegtl::change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Input >
      static void success( const Input& in, std::string& unescaped, state& st )
      {
         assert( !st.rstack.empty() );

         st.rstack.back()->emplace_back( std::move( unescaped ), in.position() );
      }
   };

}  // namespace tao::config::internal

#endif
