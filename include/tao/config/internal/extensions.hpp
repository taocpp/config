// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_EXTENSIONS_HPP

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
         inline json::value parse_jaxn( Input& in )
         {
            json::events::to_value consumer;

            if( pegtl::parse< json::jaxn::internal::rules::sor_value, json::jaxn::internal::action, json::jaxn::internal::control >( in, consumer ) ) {
               return std::move( consumer.value );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline std::string parse_string( Input& in )
         {
            json::internal::string_state s2;

            if( pegtl::parse< json::jaxn::internal::rules::string_fragment, json::jaxn::internal::unescape_action >( in, s2 ) ) {
               return std::move( s2.unescaped );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline pointer parse_pointer( Input& in, state& st )
         {
            if( pegtl::parse< rules::pointer, action, control >( in, st ) ) {
               return pointer_from_value( st.pointer );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline void cbor_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = parse_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::cbor::parse_file( f ) ) );
         }

         template< typename Input >
         inline void copy_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );
            assert( !st.lstack.empty() );

            const auto pos = in.position();

            concat& d = *st.lstack.back();
            const concat& s = access( pos, *st.ostack.front(), parse_pointer( in, st ) );
            d.v.insert( d.v.end(), s.v.begin(), s.v.end() );
         }

         template< typename Input >
         inline void debug_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();

            std::ostringstream oss;
            to_stream( oss, access( pos, *st.ostack.front(), parse_pointer( in, st ) ) );
            st.lstack.back()->v.emplace_back( entry::make_atom( pos, oss.str() ) );
         }

         template< typename Input >
         inline void env_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            st.lstack.back()->v.emplace_back( entry::make_atom( pos, get_env( pos, parse_string( in ) ) ) );
         }

         template< typename Input >
         inline void jaxn_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = parse_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::jaxn::parse_file( f ) ) );
         }

         template< typename Input >
         inline void json_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = parse_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::parse_file( f ) ) );
         }

         template< typename Input >
         inline void msgpack_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = parse_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::msgpack::parse_file( f ) ) );
         }

         template< typename Input >
         inline void parse_extension( Input& in, state& st )
         {
            pegtl::file_input i2( parse_string( in ) );
            pegtl::parse_nested< rules::value, action, control >( in, i2, st );
         }

         template< typename Input >
         inline void read_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = parse_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, read_file( pos, f ) ) );
         }

         template< typename Input >
         inline void shell_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto c = parse_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, shell_popen( pos, c ) ) );
         }

         template< typename Input >
         inline void ubjson_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = parse_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::ubjson::parse_file( f ) ) );
         }

         template< typename Input = pegtl::file_input<> >
         inline extension_map_t value_extension_map()
         {
            return {
               { "cbor", cbor_extension< Input > },
               { "copy", copy_extension< Input > },
               { "debug", debug_extension< Input > },
               { "env", env_extension< Input > },
               { "jaxn", jaxn_extension< Input > },
               { "json", json_extension< Input > },
               { "msgpack", msgpack_extension< Input > },
               { "parse", parse_extension< Input > },
               { "read", read_extension< Input > },
               { "shell", shell_extension< Input > },
               { "ubjson", ubjson_extension< Input > }
            };
         }

         template< typename Input >
         inline void erase_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = parse_pointer( in, st );

            if( erase( pos, *st.ostack.back(), p ) == 0 ) {
               throw std::runtime_error( format( "nothing to delete", { &pos, { "key", &p } } ) );
            }
         }

         template< typename Input >
         inline void erase_if_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = parse_pointer( in, st );

            erase( pos, *st.ostack.back(), p );
         }

         template< typename Input >
         inline void include_extension( Input& in, state& st )
         {
            const auto pos = in.position();
            const auto f = parse_string( in );

            try {
               pegtl::file_input i2( f );
               pegtl::parse_nested< grammar, action, control >( in, i2, st );
            }
            catch( const pegtl::input_error& e ) {
               throw std::runtime_error( format( "include failed", { &pos, { "filename", f }, { "error", e.what() }, { "errno", e.errorno } } ) );
            }
            catch( const pegtl::parse_error& e ) {
               throw std::runtime_error( format( "include failed", { &pos, { "filename", f }, { "error", e.what() } } ) );
            }
         }

         template< typename Input >
         inline void include_if_extension( Input& in, state& st )
         {
            const auto pos = in.position();
            const auto f = parse_string( in );

            try {
               pegtl::file_input i2( f );
               pegtl::parse_nested< grammar, action, control >( in, i2, st );
            }
            catch( const pegtl::input_error& )
            {
               // TODO: Are we ignoring too many errors here?
            }
            catch( const pegtl::parse_error& e ) {
               throw std::runtime_error( format( "include failed", { &pos, { "filename", f }, { "error", e.what() } } ) );
            }
         }

         template< typename Input >
         inline void stderr_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = parse_pointer( in, st );

            to_stream( std::cerr, access( pos, *st.ostack.back(), p ), 3 );
            std::cerr << std::endl;
         }

         template< typename Input >
         inline void transient_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = parse_pointer( in, st );

            access( pos, *st.ostack.back(), p ).transient = true;
         }

         template< typename Input = pegtl::file_input<> >
         inline extension_map_t member_extension_map()
         {
            return {
               { "delete", erase_extension< Input > },
               { "delete?", erase_if_extension< Input > },
               { "include", include_extension< Input > },
               { "include?", include_if_extension< Input > },
               { "stderr", stderr_extension< Input > },
               { "transient", transient_extension< Input > }
            };
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
