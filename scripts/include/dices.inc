//================================================================================
//--------------------------------------------------------------------------------
// Random Dices
//--------------------------------------------------------------------------------
//================================================================================

function CheckDiceRoll(Dice)

  if(!Cint(Dice[1]))
    return 0;
  endif

  var l;

  for(l:=1;l<=len(Dice);l:=l+1)
    if(Dice[l] != "d" and Dice[l] != "+" and CStr(Dice[l]) != "0")
      if(!CInt(Dice[l]))
        return 0;
      endif
    endif
  endfor

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetDiceMinValue(Dice)

  if(!CheckDiceRoll(Dice))
    return 0;
  endif
  var SpDice := SplitDice(Dice);
  return SpDice[1] + SpDice[3];

endfunction

//--------------------------------------------------------------------------------

function GetDiceMaxValue(Dice)

  if(!CheckDiceRoll(Dice))
    return 0;
  endif
  var SpDice := SplitDice(Dice);
  return SpDice[1] * SpDice[2] + SpDice[3];

endfunction

//--------------------------------------------------------------------------------

function SplitDice(Dice)

  var Ret := array(CInt(Dice), 0, 0);

  var MaxPos := find(Dice, "d", 1);
  var AddPos := find(Dice, "+", 1);

  if(MaxPos)
    Ret[2] := CInt(Dice[MaxPos+1, len(Dice)]);
  endif

  if(AddPos)
    Ret[3] := CInt(Dice[AddPos+1, len(Dice)]);
  endif

  return Ret;

endfunction

//--------------------------------------------------------------------------------
