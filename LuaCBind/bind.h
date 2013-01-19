#ifndef _BIND_H
#define _BIND_H

#include "impl.h"
#include "util.h"

LUACBIND_MODULE_BEGIN(Account)
  static int createAccount(lua_State *L)
  {
    double balance = luaL_checknumber(L, 1);
    *((Account**)lua_newuserdata(L, sizeof(Account))) = new Account(balance);
    luaL_getmetatable(L, className);
    lua_setmetatable(L, -2);
    return 1; ///< return one result
  }
  static int deposit(lua_State *L)
  {
    Account* self = getAccountObj(L, 1);
    double amount = luaL_checknumber(L, 2);
    self->deposit(amount);
    return 0; ///< return zero result
  }
  static int withdraw(lua_State *L)
  {
    Account* self = getAccountObj(L, 1);
    double amount = luaL_checknumber(L, 2);
    self->withdraw(amount);
    return 0;
  }
  static int balance(lua_State *L)
  {
    Account* self = getAccountObj(L, 1);
    double balance = self->balance();
    lua_pushnumber(L, balance);
    return 1; ///< return one result
  }
LUACBIND_MODULE_END(Account)

LUACBIND_METHOD_DECLARE_BEGIN(Account)
CLASS_METHOD(Account , deposit)
CLASS_METHOD(Account , withdraw)
CLASS_METHOD(Account , balance)
LUACBIND_METHOD_DECLARE_END()

#endif