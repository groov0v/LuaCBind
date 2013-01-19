#ifndef _UTIL_H
#define _UTIL_H

#define LUACBIND_MODULE_BEGIN(name) \
	class name##_Bind \
	{ \
		static const char className[]; \
		static const luaL_Reg methods[]; \
		public: \
		  static void Register(lua_State* L) \
		  { \
        luaL_newmetatable(L , className); \
        lua_pushvalue(L , -1); \
        lua_setfield(L , -1 , "__index"); \
        luaL_setfuncs(L , methods , 0); \
        lua_setglobal(L , className); \
		  } \
		private: \
		  static name* get##name##Obj(lua_State *L , int n) \
		  { \
        luaL_checktype(L , n , LUA_TUSERDATA); \
        void *ud = luaL_checkudata(L , n , className); \
        return *(name**)ud; \
		  } \
		  static int gc##name(lua_State *L) \
		  { \
        name* obj = (name*)get##name##Obj(L , 1); \
        if(obj) \
          delete obj; \
        return 0; \
		  }
#define LUACBIND_MODULE_END(name) }; \
	const char name##_Bind::className[] = #name;

#define LUACBIND_METHOD_DECLARE_BEGIN(name) \
const luaL_Reg name##_Bind::methods[] = \
{ \
  {"new"  , name##_Bind::create##name} , \
  {"__gc" , name##_Bind::gc##name} ,

#define CLASS_METHOD(class , name) {#name, class##_Bind::name} ,

#define LUACBIND_METHOD_DECLARE_END() \
  {0 , 0 } \
};

#define LUACBIND_MODULE_REGISTER(name) name##_Bind::Register(L)

#endif