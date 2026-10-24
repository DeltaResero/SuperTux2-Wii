/*
*
* array.insert is the only route into sqvector::insert, so it is the only place
* the shift can be observed. Identity is checked as well as order, because the
* elements are moved rather than reassigned.
*
*/

// an array prints as its address, so contents are compared as a joined string
local function join(a) {
  local out = "";
  foreach(v in a) out += v.tostring() + ",";
  return out;
}

local function show(label, fn) {
  local out;
  try {
    out = fn();
  } catch(e) {
    out = "error: " + e;
  }
  print(label + " -> " + out + "\n");
}

print("-- positions --\n");
show("into empty",   function() { local a = []; a.insert(0, "x"); return join(a); });
show("at head",      function() { local a = [2, 3]; a.insert(0, 1); return join(a); });
show("in middle",    function() { local a = [1, 3]; a.insert(1, 2); return join(a); });
show("at len",       function() { local a = [1, 2]; a.insert(2, 3); return join(a); });

print("-- rejections --\n");
show("idx -1",       function() { local a = [1]; a.insert(-1, 0); return join(a); });
show("idx len+1",    function() { local a = [1]; a.insert(2, 0); return join(a); });

print("-- identity --\n");
// the same table inserted many times stays the same table, so a bad move shows
// up as a stale copy or a collected one
local shared = { n = 1 };
local ids = [];
for(local i = 0; i < 300; i++) ids.insert(0, shared);
shared.n = 2;
print("shared " + ids[0].n + ids[150].n + ids[299].n + " len " + ids.len() + "\n");

print("-- order across reallocs --\n");
// head inserts reverse the input, tail inserts preserve it; both cross several
// growth steps
local head = [];
local tail = [];
for(local i = 0; i < 200; i++) {
  head.insert(0, i);
  tail.insert(tail.len(), i);
}
local ok = true;
for(local i = 0; i < 200; i++) {
  if(head[i] != 199 - i || tail[i] != i) ok = false;
}
print("head reversed and tail in order " + ok + "\n");

print("-- mixed with the other paths --\n");
// insert leaves the array usable by append, remove and pop, each of which has
// its own idea of how the storage grows
local mixed = [];
for(local i = 0; i < 100; i++) {
  mixed.insert(0, i);
  if(i % 3 == 0) mixed.append(i);
  if(i % 5 == 0) mixed.remove(mixed.len() / 2);
  if(i % 7 == 0) mixed.pop();
}
print("len " + mixed.len() + " sum " + mixed.reduce(@(x, y) x + y) + "\n");

/* EOF */
