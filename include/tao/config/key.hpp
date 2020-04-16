// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_KEY_HPP
#define TAO_CONFIG_KEY_HPP

#include <sstream>
#include <vector>

#include "internal/grammar.hpp"

#include "part.hpp"

namespace tao::config
{
   namespace internal
   {
      template< typename Rule >
      struct key_action
         : public pegtl::nothing< Rule >
      {
      };

      template<>
      struct key_action< rules::identifier >
      {
         template< typename Input >
         static void apply( const Input& in, std::vector< part >& st )
         {
            st.emplace_back( in.string() );
         }
      };

      template<>
      struct key_action< rules::index >
      {
         template< typename Input >
         static void apply( const Input& in, std::vector< part >& st )
         {
            st.emplace_back( std::stoul( in.string() ) );
         }
      };

      template<>
      struct key_action< rules::star >
      {
         static void apply0( std::vector< part >& st )
         {
            st.emplace_back( part::star_t() );
         }
      };

      template<>
      struct key_action< rules::minus >
      {
         static void apply0( std::vector< part >& st )
         {
            st.emplace_back( part::minus_t() );
         }
      };

   }  // namespace internal

   struct key
      : public std::vector< part >
   {
      key() = default;

      key( key&& ) = default;
      key& operator=( key&& ) = default;

      key( const key& ) = default;
      key& operator=( const key& ) = default;

      ~key() = default;

      explicit key( const std::string& s )
      {
         parse( s );
      }

      key( const std::initializer_list< part >& l )
         : std::vector< part >( l )
      {
      }

      key( const std::vector< part >::const_iterator& begin, const std::vector< part >::const_iterator& end )
         : std::vector< part >( begin, end )
      {
      }

      key& operator=( const std::string& s )
      {
         clear();
         parse( s );
         return *this;
      }

      key& operator=( const std::initializer_list< part >& l )
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

      std::vector< part >& vector() noexcept
      {
         return static_cast< std::vector< part >& >( *this );
      }

      const std::vector< part >& vector() const noexcept
      {
         return static_cast< const std::vector< part >& >( *this );
      }

      void parse( const std::string& s )
      {
         using grammar = pegtl::must< internal::rules::pointer, pegtl::eof >;
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

   inline key& operator+=( key& l, const part& p )
   {
      l.emplace_back( p );
      return l;
   }

   inline key& operator+=( key& l, const part::kind k )
   {
      l.emplace_back( k );
      return l;
   }

   inline key& operator+=( key& l, const std::size_t i )
   {
      l.emplace_back( i );
      return l;
   }

   inline key& operator+=( key& l, const std::string& n )
   {
      l.emplace_back( n );
      return l;
   }

   inline key operator+( const key& l, const key& k )
   {
      key r = l;
      r += k;
      return r;
   }

   inline key operator+( const key& l, const part& p )
   {
      key r = l;
      r += p;
      return r;
   }

   inline key operator+( const key& l, const part::kind k )
   {
      key r = l;
      r += k;
      return r;
   }

   inline key operator+( const key& l, const std::size_t i )
   {
      key r = l;
      r += i;
      return r;
   }

   inline key operator+( const key& l, const std::string& n )
   {
      key r = l;
      r += n;
      return r;
   }

   inline key operator+( const std::string& n, const key& l )  // TODO: Ensure this doesn't survive the first wave of refactoring after stabilising the semantics.
   {
      key t = l + n;
      if( t.size() > 1 ) {
         std::swap( t.front(), t.back() );
      }
      return t;
   }

   inline void to_stream( std::ostream& o, const key& p )
   {
      if( !p.empty() ) {
         to_stream( o, p[ 0 ] );

         for( std::size_t i = 1; i < p.size(); ++i ) {
            o << '.';
            to_stream( o, p[ i ] );
         }
      }
   }

   inline std::string to_string( const key& p )
   {
      std::ostringstream o;
      to_stream( o, p );
      return o.str();
   }

}  // namespace tao::config

#endif
