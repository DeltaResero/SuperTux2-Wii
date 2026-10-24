/*
*
* Bounds behaviour where a script hands a native function a size or an index.
* Every rejection below used to crash or read out of bounds, so this recording
* is the guards working. The valid call beside each one is what catches a guard
* that rejects too much.
*
*/

local function show(label, fn) {
  local out;
  try {
    local r = fn();
    out = (r == null) ? "null" : r.tostring();
  } catch(e) {
    out = "error: " + e;
  }
  print(label + " -> " + out + "\n");
}

print("-- blob --\n");
show("blob(-1)",              function() { return blob(-1); });
show("blob(4).resize(-1)",    function() { local b = blob(4); b.resize(-1); return "ok"; });
show("blob(4).resize(8)",     function() { local b = blob(4); b.resize(8); return "ok"; });
show("blob(8).readblob(-1)",  function() { return blob(8).readblob(-1); });
show("blob(8).readblob(4)",   function() { return blob(8).readblob(4).len(); });
show("blob(8).readblob(99)",  function() { return blob(8).readblob(99).len(); });

print("-- array --\n");
show("array(-1)",             function() { return array(-1); });
show("array(-1, 0)",          function() { return array(-1, 0); });
show("array(3).len()",        function() { return array(3).len(); });
show("array(3, 7)[2]",        function() { return array(3, 7)[2]; });
show("[1,2].resize(-1)",      function() { local a = [1, 2]; a.resize(-1); return "ok"; });

print("-- call stack --\n");
show("getstackinfos(-5)",     function() { return getstackinfos(-5); });
show("getstackinfos(-1)",     function() { return getstackinfos(-1); });
show("getstackinfos(0)",      function() { return typeof(getstackinfos(0)); });
local function named() { return getstackinfos(1).func; }
show("getstackinfos(1).func", function() { return named(); });

/* blob and file share _stream_readblob, so the negative read above covers both */
print("-- regexp --\n");
local re = regexp("a+b");
show("search(\"ab\", 5)",       function() { return re.search("ab", 5); });
show("search(\"ab\", -2^31)",   function() { return re.search("ab", -2147483647); });
show("search(\"ab\", 2)",       function() { return re.search("ab", 2); });
show("search(\"aab\", 1)",      function() { local m = re.search("aab", 1); return m.begin + "," + m.end; });
show("capture(\"ab\", 5)",      function() { return re.capture("ab", 5); });
show("capture(\"aab\", 1)",     function() { local c = re.capture("aab", 1); return c[0].begin + "," + c[0].end; });
