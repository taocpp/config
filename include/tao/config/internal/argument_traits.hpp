// Copyright (c) 2020-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ARGUMENT_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_ARGUMENT_TRAITS_HPP

#include <optional>
#include <string>
#include <string_view>

#include "forward.hpp"
#include "inner_extensions.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "key1.hpp"
#include "parse_utility.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   inline json_t do_inner_extension( state&, pegtl_input_t&, const extension_maps& );

   template< typename T >
   struct argument_traits
   {
      argument_traits( pegtl_input_t& in, state& st, const extension_maps& em )
         : m_j( do_inner_extension( in, st, em ) )
      {}

      [[nodiscard]] T convert() const
      {
         try {
            return m_j.as< T >();
         }
         catch( const std::logic_error& e ) {
            throw std::runtime_error( e.what() );
         }
      }

   private:
      const json_t m_j;
   };

   template<>
   struct argument_traits< std::string >
   {
      argument_traits( pegtl_input_t& in, state& st, const extension_maps& em )
         : m_p( in.position() ),
           m_s( convert_impl( do_inner_extension( in, st, em ) ) )
      {}

      [[nodiscard]] const std::string& convert() const
      {
         return m_s;
      }

   private:
      const pegtl::position m_p;
      const std::string m_s;

      [[nodiscard]] std::string convert_impl( json_t&& j )
      {
         if( j.type() == json::type::STRING ) {
            return std::move( j.get_string() );
         }
         if( j.type() == json::type::BINARY ) {
            const auto bv = j.as< tao::binary_view >();
            std::string s( reinterpret_cast< const char* >( bv.data() ), bv.size() );
            if( !json::internal::validate_utf8_nothrow( s ) ) {
               throw pegtl::parse_error( "invalid utf-8 in extension result", m_p );
            }
            return s;
         }
         throw pegtl::parse_error( "invalid type for string", m_p );
      }
   };

   template<>
   struct argument_traits< std::string_view >
   {};

   template<>
   struct argument_traits< json_t >
   {
      argument_traits( pegtl_input_t& in, state& st, const extension_maps& em )
         : m_j( do_inner_extension( in, st, em ) )
      {}

      [[nodiscard]] const json_t& convert() const
      {
         return m_j;
      }

   private:
      const json_t m_j;
   };

   template<>
   struct argument_traits< key1 >
   {
      argument_traits( pegtl_input_t& in, state& /*unused*/, const extension_maps& /*unused*/ )
         : m_k( parse_key1( in ) )
      {}

      [[nodiscard]] const key1& convert() const noexcept
      {
         return m_k;
      }

   private:
      const key1 m_k;
   };

   template<>
   struct argument_traits< pegtl::position >
   {
      argument_traits( pegtl_input_t& in, state& /*unused*/, const extension_maps& /*unused*/ )
         : m_p( in.position() )
      {}

      [[nodiscard]] const pegtl::position& convert() const noexcept
      {
         return m_p;
      }

   private:
      const pegtl::position m_p;
   };

   template<>
   struct argument_traits< state >
   {
      argument_traits( pegtl_input_t& /*unused*/, state& st, const extension_maps& /*unused*/ )
         : m_st( st )
      {}

      [[nodiscard]] state& convert() const noexcept
      {
         return m_st;
      }

   private:
      state& m_st;
   };

   template<>
   struct argument_traits< extension_maps >
   {
      argument_traits( pegtl_input_t& /*unused*/, state& /*unused*/, const extension_maps& em )
         : m_em( em )
      {}

      [[nodiscard]] const extension_maps& convert() const noexcept
      {
         return m_em;
      }

   private:
      const extension_maps& m_em;
   };

   template< typename T >
   struct argument_traits< std::optional< T > >
   {
      argument_traits( pegtl_input_t& in, state& st, const extension_maps& em )
         : m_j( do_inner_extension( in, st, em ) )
      {}

      [[nodiscard]] std::optional< T > convert() const
      {
         try {
            return m_j.optional< T >();
         }
         catch( const std::logic_error& e ) {
            throw std::runtime_error( e.what() );
         }
      }

   private:
      const json_t m_j;
   };

}  // namespace tao::config::internal

#endif
