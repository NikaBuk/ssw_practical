program test3;
var a : array[0..3] of integer;
 b, i : integer;
begin
  b := 1;
  for i := -1 + 1 to 1 * 2  do
  begin
    b := i * i + b;
    a[i] := i - 5 div b;
  end;
  a[b div i] := a[0] + a[1] - b + i;
end.
