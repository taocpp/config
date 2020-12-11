// Copyright (c) 2019-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_ARRAY_HPP
#define TAO_CONFIG_INTERNAL_ARRAY_HPP

#include <iterator>
#include <list>

#include "forward.hpp"
#include "pegtl.hpp"

namespace tao::config::internal
{
   template< typename C >
   struct basic_array
   {
      using data_t = std::list< C >;

      basic_array() = delete;

      explicit basic_array( const pegtl::position& pos )
         : position( pos )
      {}

      basic_array( basic_array&& ) = delete;
      basic_array( const basic_array& ) = delete;

      ~basic_array() = default;

      void operator=( basic_array&& ) = delete;
      void operator=( const basic_array& ) = delete;

      void erase( const std::size_t n )
      {
         assert( n < array.size() );

         auto i = array.begin();
         std::advance( i, n );
         array.erase( i );
      }

      void pop_back()
      {
         assert( !array.empty() );

         array.pop_back();
      }

      [[nodiscard]] C& back() noexcept
      {
         assert( !array.empty() );

         return array.back();
      }

      [[nodiscard]] const C& back() const noexcept
      {
         assert( !array.empty() );

         return array.back();
      }

      [[nodiscard]] C& operator[]( const std::size_t n ) noexcept
      {
         assert( n < array.size() );

         auto i = array.begin();
         std::advance( i, n );
         return *i;
      }

      [[nodiscard]] const C& operator[]( const std::size_t n ) const noexcept
      {
         assert( n < array.size() );

         auto i = array.begin();
         std::advance( i, n );
         return *i;
      }

      std::list< C > array;
      pegtl::position position;
   };

}  // namespace tao::config::internal

#endif
