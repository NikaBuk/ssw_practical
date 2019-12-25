program push;
var a, b, c : integer;
begin
  begin
    a := 5;
    begin
      b := 5;
      begin
        c := 2;
      end;
    end;
  end;
a := b + c - 3 * b div 2 + c - 3 + (144 - 42) * 10 + 100500 - 12 * 2 div 1;
end.
