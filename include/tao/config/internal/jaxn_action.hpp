// Copyright (c) 2017-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_JAXN_ACTION_HPP
#define TAO_CONFIG_INTERNAL_JAXN_ACTION_HPP

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "change_action_and_states.hpp"
#include "json.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename Rule >
   struct jaxn_action
      : pegtl::nothing< Rule >
   {};

   template<>
   struct jaxn_action< json::jaxn::internal::rules::kw_null >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.null( in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::kw_true >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.boolean( true, in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::kw_false >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.boolean( false, in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::identifier >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.key( std::string_view( in.begin(), in.size() ), in.position() );
      }
   };

   template< bool NEG >
   struct jaxn_action< json::jaxn::internal::rules::hexnum< NEG > >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         std::uint64_t value = 0;
         for( char c : in ) {
            if( value & 0xF000000000000000 ) {
               throw pegtl::parse_error( "JAXN hexadecimal number too large", in );
            }
            value <<= 4;
            value += json::internal::hex_char_to_integer< std::uint8_t >( c );
         }
         if constexpr( NEG ) {
            if( value < 9223372036854775808ULL ) {
               consumer.number( -static_cast< std::int64_t >( value ), in.position() );
            }
            else if( value == 9223372036854775808ULL ) {
               consumer.number( static_cast< std::int64_t >( -9223372036854775807LL - 1 ), in.position() );
            }
            else {
               throw pegtl::parse_error( "JAXN hexadecimal number too large to negate", in );
            }
         }
         else {
            consumer.number( value, in.position() );
         }
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::array::begin >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.begin_array( in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::array::element >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.element( in.position() );  // TODO: Position needed here?
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::array::end >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.end_array( in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::object::begin >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.begin_object( in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::object::element >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.member( in.position() );  // TODO: Position needed here?
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::object::end >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.end_object( in.position() );  // TODO: Position needed here?
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::zero< false > >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.number( std::uint64_t( 0 ), in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::zero< true > >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.number( std::int64_t( 0 ), in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::kw_nan >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.number( NAN, in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::kw_infinity< false > >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.number( INFINITY, in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::kw_infinity< true > >
   {
      template< typename Input, typename Consumer >
      static void apply( const Input& in, Consumer& consumer )
      {
         consumer.number( -INFINITY, in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::esign >
   {
      template< typename Input, bool NEG >
      static void apply( const Input& in, json::internal::number_state< NEG >& result )
      {
         result.eneg = ( in.peek_char() == '-' );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::idigits >
   {
      template< typename Input, bool NEG >
      static void apply( const Input& in, json::internal::number_state< NEG >& result )
      {
         const auto s = in.size();

         if( s == 1 && in.peek_char() == '0' ) {
            return;
         }

         if( s > ( 1 << 20 ) ) {
            throw pegtl::parse_error( "JSON number with 1 megabyte digits", in );
         }

         const auto c = ( std::min )( s, json::internal::max_mantissa_digits );
         std::memcpy( result.mantissa, in.begin(), c );
         result.exponent10 += static_cast< typename json::internal::number_state< NEG >::exponent10_t >( s - c );
         result.msize = static_cast< typename json::internal::number_state< NEG >::msize_t >( c );

         for( std::size_t i = c; i < s; ++i ) {
            if( in.peek_char( i ) != '0' ) {
               result.drop = true;
               return;
            }
         }
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::fdigits >
   {
      template< typename Input, bool NEG >
      static void apply( const Input& in, json::internal::number_state< NEG >& result )
      {
         result.isfp = true;

         const auto* b = in.begin();
         const auto* e = in.end();

         while( ( e > b ) && ( e[ -1 ] == '0' ) ) {
            --e;
         }
         if( !result.msize ) {
            while( ( b < e ) && ( b[ 0 ] == '0' ) ) {
               ++b;
               --result.exponent10;
            }
         }
         const auto c = ( std::min )( std::size_t( e - b ), json::internal::max_mantissa_digits - result.msize );
         std::memcpy( result.mantissa + result.msize, b, c );
         result.exponent10 -= static_cast< typename json::internal::number_state< NEG >::exponent10_t >( c );
         result.msize += static_cast< typename json::internal::number_state< NEG >::msize_t >( c );

         for( const auto* r = b + c; r < e; ++r ) {
            if( *r != '0' ) {
               result.drop = true;
               return;
            }
         }
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::edigits >
   {
      template< typename Input, bool NEG >
      static void apply( const Input& in, json::internal::number_state< NEG >& result )
      {
         result.isfp = true;

         const char* b = in.begin();

         while( ( b < in.end() ) && ( b[ 0 ] == '0' ) ) {
            ++b;
         }
         if( ( in.end() - b ) > 9 ) {
            throw pegtl::parse_error( "JSON exponent has more than 9 significant digits", in );
         }
         int exponent10 = 0;

         while( b < in.end() ) {
            exponent10 = ( exponent10 * 10 ) + ( b[ 0 ] - '0' );
            ++b;
         }
         result.exponent10 += ( result.eneg ? -exponent10 : exponent10 );
      }
   };

   template< bool NEG >
   struct jaxn_action< json::jaxn::internal::rules::number< NEG > >
      : pegtl::change_states< json::internal::number_state< NEG > >
   {
      template< typename Input, typename Consumer >
      static void success( const Input& in, json::internal::number_state< NEG >& state, Consumer& consumer )
      {
         state.success( consumer, in.position() );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::single_string >
      : change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Consumer >
      static void success( const pegtl::position& pos, std::string& unescaped, Consumer& consumer )
      {
         consumer.string( std::move( unescaped ), pos );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::string >
      : change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Consumer >
      static void success( const pegtl::position& pos, std::string& unescaped, Consumer& consumer )
      {
         consumer.string( std::move( unescaped ), pos );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::key >
      : change_action_and_states< json::jaxn::internal::unescape_action, std::string >
   {
      template< typename Consumer >
      static void success( const pegtl::position& pos, std::string& unescaped, Consumer& consumer )
      {
         consumer.key( std::move( unescaped ), pos );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::single_binary >
      : change_action_and_states< json::jaxn::internal::bunescape_action, std::vector< std::byte > >
   {
      template< typename Consumer >
      static void success( const pegtl::position& pos, std::vector< std::byte >& value, Consumer& consumer )
      {
         consumer.binary( std::move( value ), pos );
      }
   };

   template<>
   struct jaxn_action< json::jaxn::internal::rules::binary >
      : change_action_and_states< json::jaxn::internal::bunescape_action, std::vector< std::byte > >
   {
      template< typename Consumer >
      static void success( const pegtl::position& pos, std::vector< std::byte >& value, Consumer& consumer )
      {
         consumer.binary( std::move( value ), pos );
      }
   };

}  // namespace tao::config::internal

#endif
