//
//  main.cpp
//  LuaCBind
//
//  Created by Heath Luo on 8/1/13.
//  Copyright (c) 2013 Heath Luo. All rights reserved.
//  tolua++采用了类似的导出方法，只是它将类方法包装成了C接口，而非像这个代码中的static class methods

#include <iostream>
#include <lua.hpp>
#include "bind.h"

int main(int argc, const char * argv[])
{

  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  
 LUACBIND_MODULE_REGISTER(Account);
  
  if(luaL_dofile(L , argv[1]))
  {
    std::cout << "Can't open file "<< lua_tostring(L , -1) << std::endl;
    return 1;
  }
  
  lua_close(L);
  
  std::cout << "OK!\n";
    return 0;
}

