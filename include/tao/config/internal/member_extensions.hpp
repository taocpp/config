// Copyright (c) 2018-2019Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_MEMBER_EXTENSIONS_HPP
#define TAO_CONFIG_INTERNAL_MEMBER_EXTENSIONS_HPP

#include "format.hpp"
#include "value_extensions.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         template< typename Input >
         inline void erase_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto k = obtain_key( in, st );

            if( erase( pos, *st.ostack.back(), k ) == 0 ) {
               throw std::runtime_error( format( "nothing to delete", { &pos, &k } ) );
            }
         }

         template< typename Input >
         inline void erase_if_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = obtain_key( in, st );

            erase( pos, *st.ostack.back(), p );
         }

         template< typename Input >
         inline void include_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( !st.temporary.is_string_type() ) {
               throw std::runtime_error( format( "include requires string", { &pos, st.temporary.type() } ) );
            }
            const auto f = st.temporary.as< std::string >();

            try {
               pegtl::file_input i2( f );
               pegtl::parse_nested< rules::grammar, action, control >( in, i2, st );
               st.temporary.discard();
            }
            catch( const pegtl::input_error& e ) {
               throw std::runtime_error( format( "include failed", { &pos, { "filename", f }, { "error", e.what() }, { "errno", e.errorno } } ) );
            }
            catch( const pegtl::parse_error& e ) {
               throw std::runtime_error( format( "include failed", { &pos, { "filename", f }, { "error", e.what() } } ) );  // TODO: Or rely on parse_nested()'s parse_error?
            }
         }

         template< typename Input >
         inline void include_if_extension( Input& in, state& st )
         {
            const auto pos = in.position();

            do_inner_extension( in, st );

            if( st.temporary.is_null() ) {
               return;
            }
            if( !st.temporary.is_string_type() ) {
               throw std::runtime_error( format( "include requires string", { &pos, st.temporary.type() } ) );
            }
            const auto f = st.temporary.as< std::string >();

            try {
               pegtl::file_input i2( f );
               pegtl::parse_nested< rules::grammar, action, control >( in, i2, st );
               st.temporary.discard();
            }
            catch( const pegtl::input_error& ) {
               // TODO: Are we ignoring too many errors here?
            }
            catch( const pegtl::parse_error& e ) {
               throw std::runtime_error( format( "include failed", { &pos, { "filename", f }, { "error", e.what() } } ) );  // TODO: Or rely on parse_nested()'s parse_error?
            }
         }

         template< typename Input >
         inline void stderr_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = obtain_key( in, st );

            to_stream( std::cerr, access( pos, *st.ostack.back(), p ), 3 );
            std::cerr << std::endl;
         }

         template< typename Input >
         inline void temporary_extension( Input& in, state& st )
         {
            assert( !st.ostack.empty() );

            const auto pos = in.position();
            const auto p = obtain_key( in, st );

            assign( pos, *st.ostack.back(), p ).set_temporary();
         }

         template< typename Input >
         inline const auto& member_extension_map()
         {
            static const extension_map_t< Input > map = {
               { "delete", erase_extension< Input > },
               { "delete?", erase_if_extension< Input > },
               { "include", include_extension< Input > },
               { "include?", include_if_extension< Input > },
               { "stderr", stderr_extension< Input > },
               { "temporary", temporary_extension< Input > }
            };
            return map;
         }

         template< typename Input >
         inline bool do_member_extension( Input& in, state& st )
         {
            const auto pos = in.position();
            pegtl::parse< rules::outer, action, control >( in, st );

            const auto ext = std::move( st.extension );
            const auto& map = member_extension_map< Input >();
            const auto i = map.find( ext );

            if( i != map.end() ) {
               i->second( in, st );
               return true;
            }
            throw std::runtime_error( format( "unknown member extension", { &pos, { "name", ext } } ) );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
