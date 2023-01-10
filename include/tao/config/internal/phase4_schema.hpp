// Copyright (c) 2018-2023 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PHASE4_SCHEMA_HPP
#define TAO_CONFIG_INTERNAL_PHASE4_SCHEMA_HPP

#include <stdexcept>

#include "array.hpp"
#include "concat.hpp"
#include "entry.hpp"
#include "forward.hpp"
#include "object.hpp"
#include "phase5_repack.hpp"

#include "../schema/builtin.hpp"
#include "../schema/internal/forward.hpp"

#include "../value.hpp"

namespace tao::config::internal
{
   inline void phase4_schema( const value& v, const std::string& s, const schema::builtin& b )
   {
      if( const auto error = schema::internal::phase2_from_file_and_validate( s, b, v ) ) {
         throw std::runtime_error( json::to_string( error ) );
      }
   }

#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4702 )
#endif

   inline void phase4_schema( const concat& c, const schema::builtin& b )
   {
      if( !c.schema.empty() ) {
         phase4_schema( phase5_repack< traits >( c ), c.schema, b );
      }
      for( const auto& e : c.concat ) {
         switch( e.kind() ) {
            case entry_kind::value:
               return;
            case entry_kind::reference:
               throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
            case entry_kind::array:
               for( const auto& i : e.get_array().array ) {
                  phase4_schema( i, b );
               }
               return;
            case entry_kind::object:
               for( const auto& p : e.get_object().object ) {
                  phase4_schema( p.second, b );
               }
               return;
            case entry_kind::concat:
               throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
         }
         throw std::logic_error( "code should be unreachable" );  // LCOV_EXCL_LINE
      }
   }

#if defined( _MSC_VER )
#pragma warning( pop )
#endif

   inline void phase4_schema( const object& root, const std::string& s, const schema::builtin& b = schema::builtin() )
   {
      if( !s.empty() ) {
         phase4_schema( phase5_repack< traits >( root ), s, b );
      }
      for( const auto& p : root.object ) {
         phase4_schema( p.second, b );
      }
   }

}  // namespace tao::config::internal

#endif
