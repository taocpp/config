// Copyright (c) 2021-2024 Dr. Colin Hirsch and Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/config/

#include <sstream>
#include <string>
#include <string_view>

#include "test.hpp"

#include <tao/config.hpp>
#include <tao/config/internal/to_stream.hpp>

namespace tao::config
{
   void unit_test()
   {
      const std::string input = "a = 1 b { c = 42 d = [ null, true ] }";
      internal::config_parser cp;
      cp.parse( std::string_view( input ), __FUNCTION__ );
      std::ostringstream oss;
      internal::to_stream( oss, cp.st.root );
      const std::string output = oss.str();
      const std::string reference = "{position:\"(root):1:1\",object_data:{a:{remove:true,implicit:false,temporary:false,position:\"unit_test:1:1\",concat_list:[{type:\"unsigned\",data:1}]},b:{remove:false,implicit:false,temporary:false,position:\"unit_test:1:7\",concat_list:[{type:\"object\",data:{position:\"unit_test:1:9\",object_data:{c:{remove:true,implicit:false,temporary:false,position:\"unit_test:1:11\",concat_list:[{type:\"unsigned\",data:42}]},d:{remove:true,implicit:false,temporary:false,position:\"unit_test:1:18\",concat_list:[{type:\"array\",data:{position:\"unit_test:1:22\",function:\"\",array_data:[{remove:false,implicit:false,temporary:false,position:\"unit_test:1:24\",concat_list:[{type:\"null\"}]},{remove:false,implicit:false,temporary:false,position:\"unit_test:1:24\",concat_list:[{type:\"boolean\",data:true}]}]}}]}}}}]}}}";
      TAO_CONFIG_TEST_ASSERT( output == reference );
   }

}  // namespace tao::config

#include "main.hpp"
