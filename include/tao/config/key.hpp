// Copyright (c) 2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_KEY_HPP
#define TAO_CONFIG_INTERNAL_KEY_HPP

#include <vector>

#include "internal/key_action.hpp"
#include "internal/key_grammar.hpp"
#include "internal/pegtl.hpp"

#include "key_part.hpp"

namespace tao::config
{
   struct key
      : std::vector< key_part >
   {
      key() = default;

      key( key&& ) = default;
      key& operator=( key&& ) = default;

      ~key() = default;

      key( const key& ) = default;
      key& operator=( const key& ) = default;

      explicit key( const std::string& s )
      {
         assign( s );
      }

      key( const std::initializer_list< key_part >& l )
         : std::vector< key_part >( l )
      {}

      key( const std::vector< key_part >::const_iterator& begin, const std::vector< key_part >::const_iterator& end )
         : std::vector< key_part >( begin, end )
      {}

      key& operator=( const std::string& s )
      {
         clear();
         assign( s );
         return *this;
      }

      key& operator=( const std::initializer_list< key_part >& l )
      {
         vector() = l;
         return *this;
      }

      void pop_front()
      {
         assert( !empty() );
         pop_front();
      }

      void pop_back()
      {
         assert( !empty() );
         pop_back();
      }

      [[nodiscard]] std::vector< key_part >& vector() noexcept
      {
         return static_cast< std::vector< key_part >& >( *this );
      }

      [[nodiscard]] const std::vector< key_part >& vector() const noexcept
      {
         return static_cast< const std::vector< key_part >& >( *this );
      }

      void assign( const std::string& s )
      {
         using grammar = pegtl::must< internal::rules::key_rule, pegtl::eof >;
         pegtl::memory_input< pegtl::tracking_mode::lazy, pegtl::eol::lf_crlf, const char* > in( s, __FUNCTION__ );
         pegtl::parse< grammar, internal::key_action >( in, vector() );
      }
   };

   inline key pop_front( const key& p )
   {
      assert( !p.empty() );
      return key( p.begin() + 1, p.end() );
   }

   inline key pop_back( const key& p )
   {
      assert( !p.empty() );
      return key( p.begin(), p.end() - 1 );
   }

   inline key& operator+=( key& l, const key& r )
   {
      l.insert( l.end(), r.begin(), r.end() );
      return l;
   }

   inline key& operator+=( key& l, const key_part& p )
   {
      l.emplace_back( p );
      return l;
   }

   [[nodiscard]] inline key operator+( const key& l, const key& r )
   {
      key t( l );
      t += r;
      return t;
   }

   [[nodiscard]] inline key operator+( const key& l, const key_part& r )
   {
      key t( l );
      t += r;
      return t;
   }

}  // namespace tao::config

#endif
