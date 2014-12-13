-- Copyright me, fool. No, copying and stuff.
--
-- This file is auto generated, do not change it!
--

local class = require "class"

-- \brief 
--
local StringCls_cls = class "StringCls" {
  library = "String",

  -- nil StringCls:append(string str)
  -- nil StringCls:append(number num)
  -- nil StringCls:append(number num, number num2)
  -- \brief 
  append = getFunction("String", "StringCls", "append"),

  -- String::StringCls StringCls.create(string val)
  -- \brief 
  create = getFunction("String", "StringCls", "create"),

  -- String::StringCls StringCls:toUpper()
  -- \brief 
  toUpper = getFunction("String", "StringCls", "toUpper")
}

return StringCls_cls
