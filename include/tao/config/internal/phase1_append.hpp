// Copyright (c) 2018-2020 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP
#define TAO_CONFIG_INTERNAL_PHASE1_APPEND_HPP

#include <string>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "json.hpp"
#include "json_traits.hpp"
#include "key1.hpp"
#include "object.hpp"
#include "ref2.hpp"

namespace tao::config::internal
{
   [[noreturn]] inline bool phase1_append( json_t& j, const key1& path, const ref1& value )
   {
      throw std::string( "assigning reference to json value" );
   }

   inline bool phase1_append( json_t& j, const key1& path, const json_t& value );

   inline bool phase1_append_star( json_t& j, const key1& path, const json_t& value )
   {
      bool result = false;

      switch( j.type() ) {
         case json::type::ARRAY:
            for( auto& v : j.get_array() ) {
               result |= phase1_append( v, path, value );
            }
            return result;
         case json::type::OBJECT:
            for( auto& v : j.get_object() ) {
               result |= phase1_append( v.second, path, value );
            }
            return result;
         default:
            throw std::string( "assigning to wrong json value type" );
      }
   }

   inline bool phase1_append_minus( json_t& j, const key1& path, const json_t& value )
   {
      assert( false );  // TODO?
      return true;
   }

   inline bool phase1_append_name( json_t& j, const std::string name, const key1& path, const json_t& value )
   {
      return phase1_append( j[ name ], path, value );
   }

   inline bool phase1_append_index( json_t& j, const std::size_t index, const key1& path, const json_t& value )
   {
      return phase1_append( j.at( index ), path, value );
   }

   inline bool phase1_append( json_t& j, const key1& path, const json_t& value )
   {
      if( path.empty() ) {
         j = value;
         return true;
      }
      switch( path.at( 0 ).kind() ) {
         case key1_kind::star:
            return phase1_append_star( j, pop_front( path ), value );
         case key1_kind::minus:
            return phase1_append_minus( j, pop_front( path ), value );
         case key1_kind::name:
            return phase1_append_name( j, path.at( 0 ).get_name(), pop_front( path ), value );
         case key1_kind::index:
            return phase1_append_index( j, path.at( 0 ).get_index(), pop_front( path ), value );
      }
      assert( false );
   }

   inline bool phase1_append( concat& c, const key1& path, const json_t& value );

   template< typename V >
   void phase1_append_star( array& a, const key1& path, const T& value )
   {
      for( auto& c : a.array ) {
         phase1_append( c, path, value );
      }
   }

   template< typename V >
   bool phase1_append_star( concat& c, const key1& path, const T& value )
   {
      for( auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               phase1_append( e.get_value(), path, value );
               continue;
            case entry_kind::reference:
               // TODO: Error or ignore?
               continue;
            case entry_kind::array:
               for( auto& c : e.get_array().array ) {
                  phase1_append( c, path, value );
               }
               continue;
            case entry_kind::object:
               for( auto& c : e.get_object().object ) {
                  phase1_append( c.second, path, value );
               }
               continue;
         }
         assert( false );
      }
      return true;
   }

   template< typename V >
   bool phase1_append_name( concat& c, const std::string& name, const key1& path, const V& value )
   {
      return phase1_append( c.ensure_object().object[ name ], path, value );
   }

   template< typename V >
   bool phase1_append_index( concat& c, const std::size_t index, const key1& path, const V& value )
   {
      assert( false );  // TODO
   }

   inline void phase1_append( concat& c, const json_t& value )
   {
      c.concat.emplace_back( value );
   }

   inline void phase1_append( concat& c, const ref2& value )
   {
      c.concat.emplace_back( value );
   }

   inline void phase1_append( concat& c, const json::empty_array_t /*unused*/ )
   {
      c.ensure_array();
   }

   inline void phase1_append( concat& c, const json::empty_array_t /*unused*/ )
   {
      c.ensure_object();
   }

   template< typename V >
   bool phase1_append( concat& c, const key1& path, const T& value )
   {
      if( path.empty() ) {
         phase1_append( c, value );
         return true;
      }
      switch( path.at( 0 ).kind() ) {
         case key1_kind::star:
            return phase1_append_star( c, pop_front( path ), value );
         case key1_kind::minus:
            assert( false );  // TODO
         case key1_kind::name:
            return phase1_append_name( c, path.at( 0 ).get_name(), pop_front( path ), value );
         case key1_kind::index:
            return phase1_append_index( c, path.at( 0 ).get_index(), pop_front( path ), value );
      }
      assert( false );
   }

}  // namespace tao::config::internal

#endif
