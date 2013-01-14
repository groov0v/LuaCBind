//
//  main.cpp
//  LuaCBind
//
//  Created by Heath Luo on 8/1/13.
//  Copyright (c) 2013 Heath Luo. All rights reserved.
//  tolua++采用了类似的导出方法，只是它将类方法包装成了C接口，而非像这个代码中的static class methods

#include <iostream>
#include <lua.hpp>

class Account
{
public:
  Account(double balance)      { m_balance = balance; }
  void deposit(double amount)  { m_balance += amount; }
  void withdraw(double amount) { m_balance -= amount; }
  double balance(void)         { return m_balance; }
private:
  double m_balance;
};

class LuaAccount
{
  static const char className[];
  static const luaL_Reg methods[];
  
  static Account* checkaccount(lua_State *L, int n)
  {
    luaL_checktype(L, n, LUA_TUSERDATA);
    void *ud = luaL_checkudata(L, n, className);
    return *(Account**)ud;
  }
  
  static int create_account(lua_State *L)
  {
    double balance = luaL_checknumber(L, 1);
    *((Account**)lua_newuserdata(L, sizeof(Account))) = new Account(balance);
    luaL_getmetatable(L, className);
    lua_setmetatable(L, -2);
    return 1; ///< return one result
  }
  static int deposit(lua_State *L)
  {
    Account* self = checkaccount(L, 1);
    double amount = luaL_checknumber(L, 2);
    self->deposit(amount);
    return 0; ///< return zero result
  }
  static int withdraw(lua_State *L)
  {
    Account* self = checkaccount(L, 1);
    double amount = luaL_checknumber(L, 2);
    self->withdraw(amount);
    return 0;
  }
  static int balance(lua_State *L)
  {
    Account* self = checkaccount(L, 1);
    double balance = self->balance();
    lua_pushnumber(L, balance);
    return 1; ///< return one result
  }
  static int gc_account(lua_State *L)
  {
    Account *a = (Account*)checkaccount(L, 1);
    delete a;
    
    return 0;
  }
  
public:
  static void Register(lua_State* L)
  {
    /*
    /// create methods table
    lua_newtable(L); ///< BOTTOM| methodtable |TOP
    luaL_setfuncs(L , methods , 0);
    /// create metatable
    luaL_newmetatable(L , className); ///< BOTTOM| methodtable metatable |TOP
    lua_pushvalue(L , -2); ///< BOTTOM| methodtable metatable methodtable |TOP
    lua_setmetatable(L , -2); ///< BOTTOM| methodtable metatable |TOP
    lua_pushvalue(L , -2); ///< BOTTOM| methodtable metatable methodtable |TOP
    lua_setfield(L , -2 , "__index"); ///< BOTTOM| methodtable metatable |TOP
     */
    
    luaL_newmetatable(L , className); ///< BOTTOM| metatable |TOP
    lua_pushvalue(L , -1); ///< BOTTOM| metatable metatable |TOP
    lua_setfield(L , -1 , "__index"); ///< BOTTOM| metatable |TOP
    luaL_setfuncs(L , methods , 0);
    lua_setglobal(L , className); ///< empty stack
  }
};

const char LuaAccount::className[] = "Account";

#define CLASS_METHOD(class, name) {#name, class::name}

const luaL_Reg LuaAccount::methods[] =
{
  {"new"  , LuaAccount::create_account} ,
  {"__gc" , LuaAccount::gc_account} ,
  CLASS_METHOD(LuaAccount, deposit),
  CLASS_METHOD(LuaAccount, withdraw),
  CLASS_METHOD(LuaAccount, balance),  
  {0 , 0 }
};

int main(int argc, const char * argv[])
{

  lua_State* L = luaL_newstate();
  luaL_openlibs(L);
  
  LuaAccount::Register(L);
  
  if(luaL_dofile(L , argv[1]))
  {
    std::cout << "Can't open file "<< lua_tostring(L , -1) << std::endl;
    return 1;
  }
  
  lua_close(L);
  
  std::cout << "OK!\n";
    return 0;
}

