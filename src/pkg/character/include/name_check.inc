//--------------------------------------------------------------------------------

const MIN_LEN           := 3;
const MAX_LEN           := 25;

const BNF_CHECK_BOTH    := 0x01;
const BNF_CHECK_SECNAME := 0x02;
const BNF_CHECK_WOLFNAME := 0x04;

//================================================================================
//--------------------------------------------------------------------------------
// Bad Names
//--------------------------------------------------------------------------------
//================================================================================

function IsBad(who,name, Flags := 0)

  var char,i,cName;
  var Accs := ListAccounts();
  Name := lower(Name);

  if(!Name)
    return RetError("Nie podano imienia.");
  elseif(len(name) < MIN_LEN)
    return RetError("Nazwa jest za krotka. Minimum "+MIN_LEN+" znakow.");
  elseif(len(name) > MAX_LEN)
    return RetError("Nazwa jest za dluga. Maksimum "+MAX_LEN+" znakow.");
  endif

  if(Name != CheckCorrName(Name))
    return RetError("Nazwa zawiera nieprawidlowe znaki!");
  endif

  return 1;

  var FirstName := lower(GetFirstName(Name));
  var SecName := lower(GetSecName(Name));
  var cSecName;

  foreach acc in Accs
    acc := FindAccount(acc);
    for(i:=1;i<=5;i:=i+1)
      char  := acc.GetCharacter(i);
      cName := lower(GetRealName(char));
      var CharFirstName := lower(GetFirstName(cName));
      if(char != who and char)
        if(CharFirstName == FirstName)
          return RetError("Istnieje juz postac z takim imieniem.");
        elseif(IsSimilarName(CharFirstName, FirstName))
          return RetError("Istnieje juz postac z bardzo podobnym imieniem!");
        endif

        if(Flags & BNF_CHECK_WOLFNAME)
          var WolfName := GetObjProperty(char, "WolfName");
          if(WolfName)
            WolfName := lower(WolfName);
            if(WolfName[Name])
              return RetError("Istnieje juz postac z takim imieniem.");
            elseif(IsSimilarName(WolfName, Name))
              return RetError("Istnieje juz postac z bardzo podobnym imieniem!");
            endif
          endif
        endif

        if(Flags & BNF_CHECK_BOTH)
          if(Name[cName])
            return RetError("Istnieje juz postac z takim imieniem.");
          endif
        endif

        if(Flags & BNF_CHECK_SECNAME)
          cSecName := lower(GetSecName(cName));
          if(cSecName)
            if(name[cSecName])
              return RetError("Istnieje juz postac z takim nazwiskiem!");
            elseif(IsSimilarName(cSecName, SecName))
              return RetError("Istnieje juz postac z bardzo podobnym nazwiskiem!");
            endif
          endif
        endif

        if(SecName)
          if(cName[SecName] or SecName[cName])
            return RetError("Istnieje juz postac z takim imieniem jak twoje nazwisko!");
          endif
        endif

      endif
    endfor

    sleepms(5);
  endforeach

  var BCfg := ReadConfigFile(BADNAMES_CFG);
  
  if(!BCfg)
    LogError("[Badname] IsBad: Nie mozna znalezc configa zlych imion!");
  endif

  foreach BadName in GetConfigStringArray(BCfg["BadNames"],"Name")
    if(name[BadName])
      return RetError("Imie jest niedozwolone!");
    endif
  endforeach

  var c := 0,let := dictionary;

  for(i:=1;i<=len(name);i:=i+1)
    if(!let[name[i]])
      let[name[i]] := 1;
      c := c + 1;
    endif
  endfor

  if((len(name) / 2) >= c)
    return RetError("Wpisana nazwa to belkot!");
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetSecNameChar(who, name)

  var Accs := ListAccounts();
  var char, cSecName, cName, i;
  Name := lower(Name);

  foreach acc in Accs
    acc := FindAccount(acc);
    for(i:=1;i<=5;i:=i+1)
      char  := acc.GetCharacter(i);

      if(char != who and char)
        cName := GetRealName(char);
        cSecName := lower(GetSecName(cName));
        if(cSecName)
          if(name[cSecName])
            return char;
          endif
        endif
      endif
    endfor

    sleepms(5);
  endforeach

  return 0;

endfunction

//--------------------------------------------------------------------------------

function GetNameCheckSum(Name)

  var i;
  var Arr := array;

  for(i:=1; i<=len(Name); i+=1)
    Arr[i] := CAsc(Name[i]);
  endfor

  return Arr;

endfunction

//--------------------------------------------------------------------------------

function IsSimilarName(Name1, Name2)

  var i;
  var MinLen := Min(len(Name1), len(Name2));
  var MaxLen := Max(len(Name1), len(Name2));
  var Chr1, Chr2;
  var Sum := 0;
  var PrimeSyms := array("a", "e", "i", "o", "u");

  for(i:=1; i<=MaxLen; i+=1)
    if(i > MinLen)
      Sum += (maxLen-i)+1;
      continue;
    endif

    Chr1 := Name1[i];
    Chr2 := Name2[i];

    if(Chr1 != Chr2)
      if((Chr1 in PrimeSyms) and (Chr2 in PrimeSyms))
        Sum += CInt((MaxLen-i)/2)+1;
      else
        Sum += (MaxLen-i)+1;
      endif
    endif
  endfor

//  print("Name1: "+Name1+" Name2: "+Name2+" Sum: "+Sum+"/"+CInt(1.4*MinLen)+" maxlen: "+MaxLen);

  if(Sum < CInt(0.9*MinLen)) // przynajmniej 2 podobienstwa i bad!
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function FixName(Name)

  var i;

  for(i:=len(Name);i>0;i:=i-1)
    if(Name[i] == " ")
      Name[i] := "";
    else
      break;
    endif
  endfor

  Name := CheckCorrName(Name);

  var NName := "";

  foreach Word in SplitWords(Name)
    Word    := lower(Word);
    Word[1] := upper(Word[1]);

    if(!NName)
      NName := Word;
    else
      NName := NName + " " + Word;
    endif
    Name := NName;
  endforeach
  
  for(i:=1;i<=len(NName);i:=i+1)
    if(NName[i] == "'")
      if(i+2 <= len(NName))
        if(NName[i+2] != " ")
          NName[i+1] := Upper(NName[i+1]);
        endif
      endif
    endif
  endfor
  
  return NName;

endfunction

//--------------------------------------------------------------------------------

function CheckCorrName(Name)

  var Asc,i;

  for(i:=len(Name);i>0;i:=i-1)
    Asc := CAsc(Name[i]);
    if(!(Asc >= 0x41 and Asc <= 0x5A) and !(Asc >= 0x61 and Asc <= 0x7A) and Asc != 0x20 and Asc != 0x27)
      Name[i] := "";
    endif
  endfor

  return Name;

endfunction

//-----------------------------------------------------------------------------

function GetFirstName(Name)

  Name := SplitWords(Name);

  return Name[1];

endfunction

//-----------------------------------------------------------------------------

function GetSecName(Name)

  Name := SplitWords(Name);

  if(Name[2])
    var i := 2;
    var RetName := "";
    while(Name[i])
      if(!RetName)
        RetName := Name[i];
      else
        RetName := RetName + " " + Name[i];
      endif
      i := i + 1;
    endwhile
    return RetName;
  endif
  
  return "";

endfunction

//--------------------------------------------------------------------------------
