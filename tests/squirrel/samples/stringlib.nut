/*
*
* sqstdlib's string library, which sq links but no other sample calls. Covers
* every conversion format accepts, the width and precision parser, the growth
* path the result buffer takes over many conversions, and each way a format
* string can be rejected.
*
* Float formats are limited to values that are exact in binary, so the
* recording does not depend on the host's rounding.
*
*/

local function t(label, fn) {
  local out;
  try {
    out = fn().tostring();
  } catch(e) {
    out = "error: " + e;
  }
  print(label + " |" + out + "|\n");
}

print("-- conversions --\n");
t("%d",         function() { return format("%d", 42); });
t("%i",         function() { return format("%i", -7); });
t("%u",         function() { return format("%u", 7); });
t("%o",         function() { return format("%o", 8); });
t("%x %X",      function() { return format("%x %X", 255, 255); });
t("%c",         function() { return format("%c", 65); });
t("%s",         function() { return format("%s", "str"); });
t("%%",         function() { return format("100%%"); });
t("%.2f",       function() { return format("%.2f", 2.25); });
t("%g",         function() { return format("%g", 0.5); });

print("-- width and precision --\n");
t("%-8s",       function() { return format("%-8s", "ab"); });
t("%8s",        function() { return format("%8s", "ab"); });
t("%05d",       function() { return format("%05d", 42); });
t("%+d",        function() { return format("%+d", 42); });
t("% d",        function() { return format("% d", 42); });
t("%#x",        function() { return format("%#x", 255); });
t("%8.3f",      function() { return format("%8.3f", 0.5); });
t("%99s len",   function() { return format("%99s", "x").len(); });

/* 2 digits of width is the cap, so the result buffer only has to grow when a
   format string carries many conversions */
print("-- buffer growth --\n");
t("200 convs",  function() { local s = ""; for(local i = 0; i < 200; i++) s += format("%d-", i); return s.len(); });
t("one long",   function() { return format("%s%s%s%s%s%s", "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff"); });

print("-- rejected formats --\n");
t("missing arg",  function() { return format("%d %d", 1); });
t("want integer", function() { return format("%d", "s"); });
t("want float",   function() { return format("%f", "s"); });
t("want string",  function() { return format("%s", 1); });
t("unknown conv", function() { return format("%q", 1); });
t("star width",   function() { return format("%*d", 5, 1); });
t("width digits", function() { return format("%1234d", 1); });
t("spec too long",function() { return format("%--------------------------------------------------d", 1); });

print("-- printf --\n");
printf("%s=%d\n", "n", 3);

print("-- strip, split, escape --\n");
t("strip",        function() { return "[" + strip("  ab  ") + "]"; });
t("lstrip",       function() { return "[" + lstrip("  ab  ") + "]"; });
t("rstrip",       function() { return "[" + rstrip("  ab  ") + "]"; });
t("split",        function() { local a = split("a,b,,c", ","); return a.len() + ":" + a[0] + a[1] + a[2] + a[3]; });
t("split keep",   function() { return split("a,b", ",", true).len(); });
t("split no sep", function() { return split("ab", ""); });
t("escape",       function() { return escape("a\tb\"c\\"); });

print("-- startswith, endswith --\n");
t("startswith",      function() { return startswith("hello", "he"); });
t("startswith no",   function() { return startswith("hello", "xx"); });
t("startswith long", function() { return startswith("hi", "longer"); });
t("endswith",        function() { return endswith("hello", "lo"); });
t("endswith no",     function() { return endswith("hello", "xx"); });
t("endswith long",   function() { return endswith("hi", "longer"); });
t("empty needle",    function() { return startswith("hi", ""); });
