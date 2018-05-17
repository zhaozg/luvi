-- test gdbm binding

local gdbm=require"gdbm"

-- support code --------------------------------------------------------------

gdbm.version=string.gsub(gdbm.version,"\n.*version (.-). .*$","using GDBM %1")

do
local M= {
	__index=function (t,k) return t.gdbm:fetch(k) end,
	__newindex=function (t,k,v)
		if v~=nil then t.gdbm:replace(k,v) else t.gdbm:delete(k) end
	end
}

function gdbm.proxy(t)
 return setmetatable({gdbm=t},M)
end

local function keys(d,k)
  if k==nil then return d:firstkey() else return d:nextkey(k) end
end

function gdbm.keys(d)
 return keys,d
end

local function entries(d,k)
  local v
  if k==nil then k=d:firstkey() else k=d:nextkey(k) end
  if k==nil then v=nil else v=d:fetch(k) end
  return k,v
end

function gdbm.entries(d)
 return entries,d
end

end

------------------------------------------------------------------------------

function testing(s)
 print""
 print("-------------------------------------------------------",s)
end

function gdbm.show(d)
 local n=0
 for k,v in d:entries() do
  n=n+1
  print(n,k,v)
 end
 return d
end

------------------------------------------------------------------------------
print(gdbm.version)

F="test.gdbm"
d=assert(gdbm.open(F,"n"))

------------------------------------------------------------------------------
testing"insert method"
d:insert("JAN","January")
d:insert("FEB","February")
d:insert("MAR","March")
d:insert("APR","April")
d:insert("MAY","May")
d:insert("JUN","June")
d:show()

------------------------------------------------------------------------------
testing"proxy insert"
t=d:proxy()
t.JUL="July"
t.AUG="August"
t.SEP="September"
t.OCT="October"
t.NOV="November"
t.DEC="December"
d:show()

------------------------------------------------------------------------------
testing"proxy delete"
t.JAN=nil
t.FEB=nil
t.MAR=nil
t.APR=nil
t.MAY=nil
t.JUN=nil
d:show()

--[[
for i=1,1000 do t[i]=tostring(i) end
os.execute("ls -l "..F.." ; md5sum "..F)
for i=1,1000 do t[i]=nil end
print("reorganize",d:reorganize())
os.execute("ls -l "..F.." ; md5sum "..F)
--]]

------------------------------------------------------------------------------
testing"return codes"
print("JAN exists?",d:exists("JAN"))
print("replace JAN?",d:replace("JAN","Janeiro"))
print("JAN is now",t.JAN)
print("DEC exists?",d:exists("DEC"))
print("replace DEC?",d:replace("DEC","Dezembro"))
print("DEC is now",t.DEC)
print("insert JUL?",d:insert("JUL","Julho"))
print("insert XXX?",d:insert("XXX","Julho"))
print("JUL is",d:fetch("JUL"))
print("XXX is",d:fetch("XXX"))
print("YYY is",d:fetch("YYY"))
print("insert XXX",d:insert("XXX","a"))
print("insert XXX",d:insert("XXX","b"))
print("XXX is",d:fetch("XXX"))
print("replace XXX",d:replace("XXX","1"))
print("replace XXX",d:replace("XXX","2"))
print("XXX is",d:fetch("XXX"))
print("delete XXX",d:delete("XXX"))
print("delete XXX",d:delete("XXX"))

------------------------------------------------------------------------------
testing"chaining"
d:insert("FEB","Fevereiro"):delete("JUL"):replace("NOV","Novembro"):show()

------------------------------------------------------------------------------
if d.export and d.import then
testing"export/import"
F="test.flat"
print("export  ",d:export(F))
print("import store",d:import(F))
print("import replace",d:import(F,true))
end

d:close()

------------------------------------------------------------------------------
print""
print(gdbm.version)
