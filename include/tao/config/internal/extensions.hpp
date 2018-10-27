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
         inline json::value obtain_jaxn( Input& in )
         {
            json::events::to_value consumer;

            if( pegtl::parse< json::jaxn::internal::rules::sor_value, json::jaxn::internal::action, json::jaxn::internal::control >( in, consumer ) ) {
               return std::move( consumer.value );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline std::string obtain_string( Input& in )
         {
            json::internal::string_state s2;

            if( pegtl::parse< json::jaxn::internal::rules::string_fragment, json::jaxn::internal::unescape_action >( in, s2 ) ) {
               return std::move( s2.unescaped );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline pointer obtain_pointer( Input& in, state& st )
         {
            if( pegtl::parse< rules::pointer, action, control >( in, st ) ) {
               return pointer_from_value( st.pointer );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline std::string obtain_filename( Input& in, state& st )
         {
            if( !pegtl::parse< rules::chain, action, control >( in, st ) ) {
               return obtain_string( in );
            }
            if( st.extension == "env" ) {
               const auto pos = in.position();
               return get_env( pos, obtain_string( in ) );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline std::string obtain_contents( Input& in, state& st )
         {
            if( !pegtl::parse< rules::chain, action, control >( in, st ) ) {
               const auto pos = in.position();
               return read_file( pos, obtain_string( in ) );
            }
            if( st.extension == "env" ) {
               const auto pos = in.position();
               return read_file( pos, get_env( pos, obtain_string( in ) ) );
            }
            if( st.extension == "shell" ) {
               const auto pos = in.position();
               return shell_popen( pos, obtain_string( in ) );
            }
            throw std::string( __PRETTY_FUNCTION__ );
         }

         template< typename Input >
         inline void cbor_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = obtain_contents( in, st );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::cbor::from_string( f ) ) );
         }

         template< typename Input >
         inline void copy_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );
            assert( !st.lstack.empty() );

            const auto pos = in.position();

            concat& d = *st.lstack.back();
            const concat& s = access( pos, *st.ostack.front(), obtain_pointer( in, st ) );
            d.v.insert( d.v.end(), s.v.begin(), s.v.end() );
         }

         template< typename Input >
         inline void debug_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();

            std::ostringstream oss;
            to_stream( oss, access( pos, *st.ostack.front(), obtain_pointer( in, st ) ) );
            st.lstack.back()->v.emplace_back( entry::make_atom( pos, oss.str() ) );
         }

         template< typename Input >
         inline void env_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            st.lstack.back()->v.emplace_back( entry::make_atom( pos, get_env( pos, obtain_string( in ) ) ) );
         }

         template< typename Input >
         inline void jaxn_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = obtain_contents( in, st );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::jaxn::from_string( f ) ) );
         }

         template< typename Input >
         inline void json_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = obtain_contents( in, st );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::from_string( f ) ) );
         }

         template< typename Input >
         inline void msgpack_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = obtain_contents( in, st );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::msgpack::from_string( f ) ) );
         }

         template< typename Input >
         inline void parse_extension( Input& in, state& st )
         {
            const auto f = obtain_contents( in, st );
            input_t i2( f, __PRETTY_FUNCTION__ );
            pegtl::parse_nested< rules::value, action, control >( in, i2, st );
         }

         template< typename Input >
         inline void read_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = obtain_filename( in, st );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, read_file( pos, f ) ) );
         }

         template< typename Input >
         inline void shell_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto c = obtain_string( in );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, shell_popen( pos, c ) ) );
         }

         template< typename Input >
         inline void ubjson_extension( Input& in, state& st )
         {
            assert( !st.lstack.empty() );

            const auto pos = in.position();
            const auto f = obtain_contents( in, st );

            st.lstack.back()->v.emplace_back( entry::make_atom( pos, json::ubjson::from_string( f ) ) );
         }

         inline extension_map_t value_extension_map()
         {
            return {
               { "cbor", cbor_extension< input_t > },
               { "copy", copy_extension< input_t > },
               { "debug", debug_extension< input_t > },
               { "env", env_extension< input_t > },
               { "jaxn", jaxn_extension< input_t > },
               { "json", json_extension< input_t > },
               { "msgpack", msgpack_extension< input_t > },
               { "parse", parse_extension< input_t > },
               { "read", read_extension< input_t > },
               { "shell", shell_extension< input_t > },
               { "ubjson", ubjson_extension< input_t > }
            };
         }

         template< typename Input >
         inline void erase_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = obtain_pointer( in, st );

            if( erase( pos, *st.ostack.back(), p ) == 0 ) {
               throw std::runtime_error( format( "nothing to delete", { &pos, { "key", &p } } ) );
            }
         }

         template< typename Input >
         inline void erase_if_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = obtain_pointer( in, st );

            erase( pos, *st.ostack.back(), p );
         }

         template< typename Input >
         inline void include_extension( Input& in, state& st )
         {
            const auto pos = in.position();
            const auto f = obtain_filename( in, st );

            try {
               pegtl::file_input i2( f );
               pegtl::parse_nested< rules::grammar, action, control >( in, i2, st );
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
            const auto f = obtain_filename( in, st );

            try {
               pegtl::file_input i2( f );
               pegtl::parse_nested< rules::grammar, action, control >( in, i2, st );
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
            const auto p = obtain_pointer( in, st );

            to_stream( std::cerr, access( pos, *st.ostack.back(), p ), 3 );
            std::cerr << std::endl;
         }

         template< typename Input >
         inline void temporary_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = obtain_pointer( in, st );

            access( pos, *st.ostack.back(), p ).temporary = true;
         }

         inline extension_map_t member_extension_map()
         {
            return {
               { "delete", erase_extension< input_t > },
               { "delete?", erase_if_extension< input_t > },
               { "include", include_extension< input_t > },
               { "include?", include_if_extension< input_t > },
               { "stderr", stderr_extension< input_t > },
               { "temporary", temporary_extension< input_t > }
            };
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
