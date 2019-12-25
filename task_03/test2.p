program test2;
var i : integer;
    b : array[0..2] of integer;
begin
 for i := -2 + 2 to 2 + i do begin
  if b[i] = (b[i] + b[i]) then
    break;
  b[i] := i + 2 * (3 xor 4);
 end;
 i := b[i] + b[i];
end.
