local key_word = {module_begin = "LUACBIND_MODULE_BEGIN" , module_end = "LUACBIND_MODULE_END" , method_begin = "LUACBIND_METHOD_BEGIN" , method_end = "LUACBIND_METHOD_END"}
--local classes = { ["name"] = {methods = { ["name"] = { params = {type = "" , name=""} , ret = ""}  , ...}} , ["..."] = {} }

local function parser(content)
  for module_content in string.gmatch(content , string.format("//%s.+//%s" , key_word.module_begin , key_word.module_end)) do
    module_parser(module_content)
  end
end

local function module_parser(content)
  print(content)
  if content ~= nil then
    local class_name = string.match(content , "class%s+.*%s*")
    --print(class_name)
  end
end

local function methods_parser(content)
  
end

local function gen_bind()
  
end


local file = io.open("impl.h" , "r")
local content = file:read("*a")

module_parser(content)
methods_parser(content)
gen_bind()

file:close()
