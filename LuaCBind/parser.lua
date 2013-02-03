local keywords = {module_begin = "LUACBIND_MODULE_BEGIN" , module_end = "LUACBIND_MODULE_END" , method_begin = "LUACBIND_METHOD_BEGIN" , method_end = "LUACBIND_METHOD_END"}
-- 不支持指针，不支持多态
local paramTypeCastFunc = {["double"] = "luaL_checknumber" , ["float"] = "(float)luaL_checknumber" , ["int"] = "luaL_checkint" , ["long"] = "luaL_checklong" , ["char"] = "(char)luaL_checkint" , ["short"] = "(short)luaL_checkint"}
local returnTypeCastFunc = {["double"] = "lua_pushnumber" , ["float"] = "lua_pushnumber" , ["int"] = "lua_pushinteger" , ["bool"] = "lua_pushboolean" , ["char"] = "lua_pushinteger" , ["short"] = "lua_pushinteger"}
local classes = {} --{ ["name"] = {methods = { ["name"] = { { params = {{type = "" , name=""}} , ret = ""} , ... }  , ...}} , ["..."] = {} }
local bind_file_name = "impl.h"

local function param_parser(content)
	local params = {}
	for param_type , param_name in string.gmatch(content , "([^%s,]+)%s+([^%s,]+)") do
		params[#params + 1] = {type = param_type , name = param_name}
	end

	return params
end

local function signature_parser(content , class_name)
	if content ~= nil then
  		local ret_str , method_str , params_str = string.match(content , "([^%s]*)%s*([^%s]*)%s*%((.-)%)")
		--print(string.format("%s:%s:%s" , ret_str , method_str , params_str))
		if ret_str ~= nil and method_str == "" then -- construct method
			method_str = ret_str
			ret_str = ""
			--print(method_str)
		end
		if classes[class_name].methods[method_str] == nil then
			classes[class_name].methods[method_str] = {}
		end
		table.insert(classes[class_name].methods[method_str] , {ret = ret_str , params = param_parser(params_str)})
	end
end

local function module_parser(content)
	--print(content)
	if content ~= nil then
		-- fetch class name
		local class_name = string.match(content , "class%s+(.-)[%s*|{]")
		--print(class_name)
		if class_name ~= nil then
			classes[class_name] = {methods = {}}
			-- fetch the signature of construct methods
			for signature in string.gmatch(content , string.format("%s%%s*%%(.-%%)" , class_name)) do
				signature_parser(signature , class_name)
			end
			-- fetch the signature of normal mehtods
			for method_content in string.gmatch(content , string.format("//%s(.-)//%s" , keywords.method_begin , keywords.method_end)) do
				if method_content ~= nil then
					for signature in string.gmatch(method_content , "[^%s]*%s*[^%s]*%s*%(.-%)") do
						signature_parser(signature , class_name)
					end
				end
			end		
		end
  	end
end

local function parser(content)
  assert(content ~= nil)
  for module_content in string.gmatch(content , string.format("//%s(.-)//%s" , keywords.module_begin , keywords.module_end)) do
    module_parser(module_content)
  end
end

local function param_pack_string(params)
	local paramStr
	for i = 1 , #params do
		if i == 1 then
			paramStr = string.format("%s" , params[i].name)
		else
			paramStr = string.format("%s , %s" , paramStr , params[i].name)
		end		
	end

	return paramStr
end

local function gen_bind(debug)
	if debug then
		for class_name , methods_table in pairs(classes) do
			print("----"..class_name.."----")
			for method_name , signatures in pairs(methods_table.methods) do
				for _ , signature in pairs(signatures) do
					local sign_str = string.format("%s %s(" , signature.ret or "" , method_name)
					if signature.params ~= nil then
						for i = 1 , #signature.params do
							if i == 1 then
								sign_str = string.format("%s%s %s" , sign_str , signature.params[i].type , signature.params[i].name)
							else
								sign_str = string.format("%s , %s %s" , sign_str , signature.params[i].type , signature.params[i].name)
							end
						end
						sign_str = sign_str..")"
						print(sign_str)
					end
				end
			end
		end
	end

	local bind_file = io.open("bind.h" , "w+")
	for class_name , methods_table in pairs(classes) do
		bind_file:write(string.format("#ifndef _BIND_H\n#define _BIND_H\n\n#include \"%s\"\n#include \"util.h\"\n\nLUACBIND_MODULE_BEGIN(%s)\n" , bind_file_name , class_name))
		for method_name , signatures in pairs(methods_table.methods) do
			for _ , signature in pairs(signatures) do
				if method_name == class_name then -- constructor
					bind_file:write(string.format("\tstatic int create%s(lua_State* L)\n\t{\n" , class_name))
					for i = 1 , #signature.params do
						bind_file:write(string.format("\t\t%s %s = %s(L , %d);\n" , signature.params[i].type , signature.params[i].name , paramTypeCastFunc[signature.params[i].type] , i))
					end
					bind_file:write(string.format("\t\t*((%s**)lua_newuserdata(L , sizeof(%s))) = new %s(%s);\n" , class_name , class_name , class_name , param_pack_string(signature.params)))
					bind_file:write(string.format("\t\tluaL_getmetatable(L , \"%s\");\n\t\tlua_setmetatable(L, -2);\n" , class_name))
				else	-- normal
					bind_file:write(string.format("\tstatic int %s(lua_State* L)\n\t{\n" , method_name))
					bind_file:write(string.format("\t\t%s* self = get%sObj(L , 1);\n" , class_name , class_name))
					for i = 1 , #signature.params do
						bind_file:write(string.format("\t\t%s %s = %s(L , %d);\n" , signature.params[i].type , signature.params[i].name , paramTypeCastFunc[signature.params[i].type] , i + 1))
					end
          if returnTypeCastFunc[signature.ret] ~= nil then
            bind_file:write(string.format("\t\t%s(L , self->%s(%s));\n" , returnTypeCastFunc[signature.ret] , method_name , param_pack_string(signature.params) or ""))
          else
            bind_file:write(string.format("\t\tself->%s(%s);\n" , method_name , param_pack_string(signature.params) or ""))
          end
				end
				if signature.ret ~= "void" then
					bind_file:write("\t\treturn 1;\n\t}\n")
				else
					bind_file:write("\t\treturn 0;\n\t}\n")
				end				
			end
		end
		bind_file:write(string.format("LUACBIND_MODULE_END(%s)\n\n" , class_name))
		bind_file:write(string.format("LUACBIND_METHOD_DECLARE_BEGIN(%s)\n" , class_name))
		for method_name , signatures in pairs(methods_table.methods) do
			if method_name ~= class_name then
				bind_file:write(string.format("\tCLASS_METHOD(%s , %s)\n" , class_name , method_name))
			end
		end
		bind_file:write("LUACBIND_METHOD_DECLARE_END()\n\n#endif")
	end
	bind_file:close()
end

local file = io.open(bind_file_name , "r")
local content = file:read("*a")

parser(content)
--module_parser(content)
--methods_parser(content)
gen_bind(true)

file:close()
