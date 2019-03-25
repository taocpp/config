// Copyright (c) 2018-2019 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_VALUE_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_VALUE_EXTENSIONS_HPP

#include "access.hpp"
#include "action.hpp"
#include "control.hpp"
#include "erase.hpp"
#include "grammar.hpp"
#include "state.hpp"
#include "system.hpp"
#include "to_stream.hpp"
#include "utility.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Input >
         using extension_t = std::function< void( Input&, state& ) >;

         template< typename Input >
         using extension_map_t = std::map< std::string, extension_t< Input > >;

         template< typename Input >
         inline void do_inner_extension( Input& in, state& st );

         template< typename Input >
         inline json::value obtain_jaxn( Input& in )
         {
            json::events::to_value consumer;
            pegtl::parse< pegtl::must< json::jaxn::internal::rules::sor_value >, json::jaxn::internal::action, json::jaxn::internal::errors >( in, consumer );
            return std::move( consumer.value );
         }

         template< typename Input >
         inline std::string obtain_string( Input& in )
         {
            json::internal::string_state s2;
            pegtl::parse< pegtl::must< json::jaxn::internal::rules::string_fragment >, json::jaxn::internal::unescape_action >( in, s2 );
            return std::move( s2.unescaped );
         }

         template< typename Input >
         inline key obtain_key( Input& in, state& st )
         {
            pegtl::parse< pegtl::must< rules::pointer >, action, control >( in, st );
            return key_from_value( in.position(), st.temporary );
         }

         template< typename Input >
         inline void cbor_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               st.temporary = json::cbor::from_string( st.temporary.as< std::string >() );
               return;
            }
            throw std::runtime_error( format( "require string to parse cbor", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline void copy_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto p = obtain_key( in, st );

            concat& d = *st.lstack.back();
            const concat& s = access( pos, d.parent(), p );
            d.append( s );  // TOOD: Modify/update d.position?
         }

         template< typename Input >
         inline void debug_extension( Input& in, state& st )
         {
            const auto pos = in.position();
            const auto p = obtain_key( in, st );

            std::ostringstream oss;
            to_stream( oss, access( pos, st.lstack.back()->parent(), p ) );
            st.temporary = oss.str();
         }

         template< typename Input >
         inline void env_extension( Input& in, state& st )
         {
            const auto pos = in.position();
            const auto v = obtain_string( in );

            st.temporary = get_env_throws( pos, v );
         }

         template< typename Input >
         inline void env_if_extension( Input& in, state& st )
         {
            const auto pos = in.position();
            const auto v = obtain_string( in );

            const auto e = get_env_nothrow( v );

            if( pegtl::parse< rules::wsp >( in ) ) {
               const auto d = obtain_string( in );
               st.temporary = e.value_or( d );
               return;
            }
            st.temporary = e;
         }

         template< typename Input >
         inline void jaxn_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               st.temporary = json::jaxn::from_string( st.temporary.as< std::string >() );
               return;
            }
            throw std::runtime_error( format( "require string to parse jaxn", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline void json_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               st.temporary = json::from_string( st.temporary.as< std::string >() );
               return;
            }
            throw std::runtime_error( format( "require string to parse json", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline void msgpack_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               st.temporary = json::msgpack::from_string( st.temporary.as< std::string >() );
               return;
            }
            throw std::runtime_error( format( "require string to parse msgpack", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline void parse_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               pegtl::string_input< pegtl::tracking_mode::eager, typename Input::eol_t, const char* > i2( st.temporary.as< std::string >(), __FUNCTION__ );
               pegtl::parse_nested< rules::value, action, control >( in, i2, st );
               return;
            }
            throw std::runtime_error( format( "require string to parse value", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline void read_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               st.temporary = read_file_throws( st.temporary.as< std::string >() );
               return;
            }
            throw std::runtime_error( format( "require string as filename", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline void shell_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               st.temporary = shell_popen_throws( pos, st.temporary.as< std::string >() );
               return;
            }
            throw std::runtime_error( format( "require string for shell command", { &pos, st.temporary.type() } ) );
         }

         // clang-format off
         struct split_plus_ws : pegtl::plus< pegtl::space > {};
         struct split_star_ws : pegtl::star< pegtl::space > {};
         struct split_string : pegtl::plus< pegtl::not_one< ' ', '\n', '\r', '\t', '\v', '\f' > > {};
         struct split_grammar : pegtl::must< split_star_ws, pegtl::list_tail< split_string, split_plus_ws >, pegtl::eof > {};

         template< typename > struct split_action {};
         // clang-format on

         template<>
         struct split_action< split_string >
         {
            template< typename Input >
            static void apply( const Input& in, json::value& temporary )
            {
               temporary.emplace_back( in.string() );
            }
         };

         template< typename Input >
         inline void split_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               pegtl::string_input< pegtl::tracking_mode::eager, typename Input::eol_t, const char* > i2( st.temporary.as< std::string >(), __FUNCTION__ );
               st.temporary = json::empty_array;
               pegtl::parse_nested< split_grammar, split_action >( in, i2, st.temporary );
               return;
            }
            throw std::runtime_error( format( "require string to split", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline void ubjson_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_string_type() ) {
               st.temporary = json::ubjson::from_string( st.temporary.as< std::string >() );
               return;
            }
            throw std::runtime_error( format( "require string to parse ubjson", { &pos, st.temporary.type() } ) );
         }

         template< typename Input >
         inline const auto& value_extension_map()
         {
            static const extension_map_t< Input > map = {
               { "cbor", cbor_extension< Input > },
               // "copy" does not return a single value.
               { "debug", debug_extension< Input > },
               { "env", env_extension< Input > },
               { "env?", env_if_extension< Input > },
               { "jaxn", jaxn_extension< Input > },
               { "json", json_extension< Input > },
               { "msgpack", msgpack_extension< Input > },
               // "parse" does not return a single value.
               { "read", read_extension< Input > },
               { "shell", shell_extension< Input > },
               { "split", split_extension< Input > },
               { "ubjson", ubjson_extension< Input > }
            };
            return map;
         }

         template< typename Input >
         inline void do_inner_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            if( pegtl::parse< rules::inner, action, control >( in, st ) ) {
               const auto ext = std::move( st.extension );
               const auto& map = value_extension_map< Input >();
               const auto i = map.find( ext );

               if( i != map.end() ) {
                  i->second( in, st );
                  pegtl::parse< pegtl::must< rules::round_z > >( in );
                  return;
               }
               throw std::runtime_error( format( "unknown value extension", { &pos, { "name", ext } } ) );
            }
            st.temporary = obtain_string( in );
         }

         template< typename Input >
         inline bool do_value_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            pegtl::parse< rules::outer, action, control >( in, st );

            if( st.extension == "copy" ) {
               copy_extension( in, st );
               return true;
            }
            if( st.extension == "parse" ) {
               parse_extension( in, st );
               return true;
            }
            const auto ext = std::move( st.extension );
            const auto& map = value_extension_map< Input >();
            const auto i = map.find( ext );

            if( i != map.end() ) {
               i->second( in, st );
               assert( !st.temporary.is_discarded() );
               st.lstack.back()->emplace_back_atom( in.position(), std::move( st.temporary ) );
               st.temporary.discard();
               return true;
            }
            throw std::runtime_error( format( "unknown value extension", { &pos, { "name", ext } } ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
