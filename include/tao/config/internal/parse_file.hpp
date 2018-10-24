// Copyright (c) 2018 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#ifndef TAO_CONFIG_INTERNAL_PARSE_FILE_HPP
#define TAO_CONFIG_INTERNAL_PARSE_FILE_HPP

#include <cassert>
#include <string>

#include "action.hpp"
#include "control.hpp"
#include "grammar.hpp"
#include "pegtl.hpp"
#include "state.hpp"

namespace tao
{
   namespace config
   {
      namespace internal
      {
         inline void parse_file_impl( state& st, const std::string& filename )
         {
            assert( st.astack.empty() );
            assert( st.lstack.empty() );
            assert( st.ostack.size() == 1 );

            pegtl::file_input in( filename );
            pegtl::parse< grammar, action, control >( in, st );

            assert( st.astack.empty() );
            assert( st.lstack.empty() );
            assert( st.ostack.size() == 1 );
         }

      }  // namespace internal

   }  // namespace config

}  // namespace tao

#endif
