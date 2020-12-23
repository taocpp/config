// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ARGUMENT_TRAITS_HPP
#define TAO_CONFIG_INTERNAL_ARGUMENT_TRAITS_HPP

#include <optional>

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

      T convert() const
      {
         return m_j.as< T >();
      }

   private:
      const json_t m_j;
   };

   template<>
   struct argument_traits< json_t >
   {
      argument_traits( pegtl_input_t& in, state& st, const extension_maps& em )
         : m_j( do_inner_extension( in, st, em ) )
      {}

      const json_t& convert() const
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

      const key1& convert() const noexcept
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

      const pegtl::position& convert() const noexcept
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

      state& convert() const noexcept
      {
         return m_st;
      }

   private:
      state& m_st;
   };

   template<>
   struct argument_traits< extension_maps >
   {
      argument_traits( pegtl_input_t& /*unused*/, state& st, const extension_maps& em )
         : m_em( em )
      {}

      const extension_maps& convert() const noexcept
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

      std::optional< T > convert() const
      {
         return m_j.optional< T >();
      }

   private:
      const json_t m_j;
   };

}  // namespace tao::config::internal

#endif