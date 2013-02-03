#ifndef _BIND_H
#define _BIND_H

#include "impl.h"
#include "util.h"

LUACBIND_MODULE_BEGIN(Account)
	static int createAccount(lua_State* L)
	{
		double balance = luaL_checknumber(L , 1);
		*((Account**)lua_newuserdata(L , sizeof(Account))) = new Account(balance);
		luaL_getmetatable(L , "Account");
		lua_setmetatable(L, -2);
		return 1;
	}
	static int balance(lua_State* L)
	{
		Account* self = getAccountObj(L , 1);
		lua_pushnumber(L , self->balance());
		return 1;
	}
	static int withdraw(lua_State* L)
	{
		Account* self = getAccountObj(L , 1);
		double amount = luaL_checknumber(L , 2);
		self->withdraw(amount);
		return 0;
	}
	static int deposit(lua_State* L)
	{
		Account* self = getAccountObj(L , 1);
		double amount = luaL_checknumber(L , 2);
		self->deposit(amount);
		return 0;
	}
LUACBIND_MODULE_END(Account)

LUACBIND_METHOD_DECLARE_BEGIN(Account)
	CLASS_METHOD(Account , balance)
	CLASS_METHOD(Account , withdraw)
	CLASS_METHOD(Account , deposit)
LUACBIND_METHOD_DECLARE_END()

#endif