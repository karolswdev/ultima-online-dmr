//-----------------------------------------------------------------------------

use math;

//-----------------------------------------------------------------------------

function ClStr(Str)

  if(!Str)
    return "";
  elseif(Str["<uninit"] or Str["errortext"])
    return "";
  else
    return Str;
  endif

endfunction

//-----------------------------------------------------------------------------

function RemoveStrInBracets(byref Name, sSeek)

  var Str, sPos;

  repeat

    Str := FindStrInBracets(Name, sPos);
    if(Str)
      if(Str[sSeek])
        Name[Str] := "";
        return 1;
      endif
    endif

  until(!Str);

  return 0;

endfunction

//--------------------------------------------------------------------------------

function FindStrInBracets(byref Name, byref sPos)

  var Pos := find(Name, "(", sPos);

  if(Pos)
    var ePos := find(Name, ")", Pos);
    if(ePos)
      sPos := Pos;
      return Name[Pos, ePos];
    endif
  endif

  return "";

endfunction

//-----------------------------------------------------------------------------

function SplitTextToLines(byref Text, mLen)

  if(len(Text) < mLen)
    return array(Text);
  endif

  var Lines := array;
  var Words := SplitWords(Text);
  var Line, i;

  for(i:=1;i<=Words.size();i:=i+1)
    if(len(Line) + len(Words[i]) > mLen)
      Lines.append(Line);
      Line := "";
    endif

    AddWord(Line, Words[i]);    
  endfor

  if(Line)
    Lines.append(Line);
  endif

  return Lines;

endfunction

//-----------------------------------------------------------------------------

function CombineWords(byref arr, s := 1, e := 0, sp := " ")

  if(arr == error or !arr)
    return "";
  endif

  var combined := "",sep := "",i := s;

  if(!e)
    e := arr.size();
  endif
  
  for(i:=s;i<=e;i:=i+1)
    combined := combined + sep + arr[i];
    sep := sp;
  endfor
  
  return combined;
  
endfunction

//-----------------------------------------------------------------------------

function CutWord(Word, wLen)

  if(len(Word) > wLen)
    return Word[1,wLen]+"...";
  endif

  return Word;

endfunction

//-----------------------------------------------------------------------------

function AddWord(byref Sentence, byref Word)

  if(!Sentence)
    Sentence := Word;
  else
    Sentence := Sentence+" "+Word;
  endif

endfunction

//-----------------------------------------------------------------------------

function ZeroFill(Num, Length)

  Num := CInt(Num);

  var i;
  for(i:=2;i<=Length;i:=i+1)
    if(Num < Pow(10, i-1))
      Num := "0" + Num;
    endif
  endfor
  
  return Num;

endfunction

//-----------------------------------------------------------------------------

function UnZeroFill(Num)

  while(Num[1] == "0" and Num)
    Num[1] := "";
  endwhile

  return Num;

endfunction

//-----------------------------------------------------------------------------

function FormatSize(Size, Sep := " ")

  Size := CStr(Size);

  var i      := len(Size);
  var RetStr := "";

  if(i > 3)
    while(i > 3)
      RetStr := Sep+Size[i-2,3]+RetStr;
      i := i - 3;
    endwhile
    RetStr := Size[1,i]+RetStr;
  else
    RetStr := Size;
  endif

  return RetStr;

endfunction

//-----------------------------------------------------------------------------

function IsNumber(Str)

  if(CInt(Str))
    return 1;
  elseif(Str == "0" or Str == "0x0")
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
