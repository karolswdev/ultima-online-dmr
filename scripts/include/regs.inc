//--------------------------------------------------------------------------------

include "include/items";
include "include/itemuses";
include "include/tempmods";

//================================================================================
//--------------------------------------------------------------------------------
// Reagents
//--------------------------------------------------------------------------------
//================================================================================

const TRF_NOT_NEEDED         := 0x01;
const TRF_NO_LOWER_FC        := 0x02;

//--------------------------------------------------------------------------------

function HaveReagents(who, cfgElem, Amt := 1, Flags := 0)

  var Reagents := GetConfigStringArray(cfgElem, "reagent");

  if(!Reagents)
    return 0;
  endif

  var Reagent,Regs := {};
  var ArtMod := GetTempModAmount(who, MOD_LOWER_RC);

  if(Flags & TRF_NO_LOWER_FC)
    ArtMod := 0;
  endif

  foreach Reg in Reagents
    Reg     := SplitWords(Reg);
    Reg[2]  := CInt(ToMin(Cint(Reg[2]), 1) * Amt);

    Reagent := FindItemInContainer(who.backpack, Reg[1], 0, Reg[2]);

    if(!Reagent)
      return 0;
    endif
    
    if(ArtMod)
      if(Reg[2] - ArtMod >= 1)
        Reg[2] := Reg[2] - ArtMod;
      endif
    endif
    
    if(Reagent.amount < Reg[2])
      return 0;
    endif

    Regs.append({Reagent, Reg[2]});
  endforeach

  foreach Reg in Regs
    if(GetItemUses(Reg[1]) < Reg[2])
      return 0;
    endif
  endforeach

  return 1;

endfunction

//--------------------------------------------------------------------------------

function TakeReagents(who, cfgElem, Amt := 1, Flags := 0)

  var Reagents := GetConfigStringArray(cfgElem, "reagent");

  if(!Reagents)
    return (Flags & TRF_NOT_NEEDED);
  endif

  return EatReagents(who, Reagents, Amt, Flags);

endfunction

//--------------------------------------------------------------------------------

function EatReagents(who, Reagents, Amt := 1, Flags := 0)

  if(!Reagents)
    return 0;
  endif

  var Reagent,Regs := {};
  var ArtMod := GetTempModAmount(who,MOD_LOWER_RC);

  if(Flags & TRF_NO_LOWER_FC)
    ArtMod := 0;
  endif

  foreach Reg in Reagents
    Reg     := SplitWords(Reg);
    Reg[2]  := CInt(ToMin(Cint(Reg[2]), 1) * Amt);

    Reagent := FindItemInContainer(who.backpack, Reg[1], 0, Reg[2]);

    if(!Reagent)
      return 0;
    endif
    
    if(ArtMod)
      if(Reg[2] - ArtMod >= 1)
        Reg[2] := Reg[2] - ArtMod;
      endif
    endif
    
    if(Reagent.amount < Reg[2])
      return 0;
    endif

    Regs.append({Reagent, Reg[2]});
  endforeach

  foreach Reg in Regs
    if(!SubItemUses(Reg[1], Reg[2]))
      return 0;
    endif
  endforeach

  return 1;

endfunction

//--------------------------------------------------------------------------------

function TellWhatReagents(who, cfgElem, Amt := 1)

  var Reagents := GetConfigStringArray(cfgElem,"reagent");
  SendReagentsList(who, Reagents, Amt);

endfunction

//--------------------------------------------------------------------------------

function GetReagentsList(cfgElem, Amt := 1)

  var Reagents := GetConfigStringArray(cfgElem,"reagent");
  return BuildReagentsList(Reagents, Amt);

endfunction

//--------------------------------------------------------------------------------

function BuildReagentsList(Reagents, Amt := 1)

  var Msg := {};
  var desc, amount;
  
  if(!Reagents)
    Msg.append("Nie masz potrzebnych skladnikow.",FONT_NORMAL,COLOR_RED);
  else
    Msg.append("Potrzebujesz:");
    foreach Reg in Reagents
      Reg     := SplitWords(Reg);
      if(Reg[1] and !CInt(Reg[1]))
        Reg[1] := GetObjTypeByName(Reg[1]);
      else
        Reg[1] := CInt(Reg[1]);
      endif

      amount  := Cint(Reg[2]) * Amt;
      desc    := GetDescForItem(reg[1]);
      if(!desc)
        desc := "nieznany skladnik";
      else
        if(!amount)
          amount := Amt;
        endif
        Msg.append(amount + " " + desc);
      endif
    endforeach
  endif

  return Msg;

endfunction

//--------------------------------------------------------------------------------

function SendReagentsList(who, Reagents, Amt := 1)

  var Msg := BuildReagentsList(Reagents, Amt);

  foreach Message in Msg
    if(Message)
      SendSysMessage(who,Message,FONT_NORMAL,COLOR_RED);
      sleepms(100);
    endif
  endforeach

endfunction

//--------------------------------------------------------------------------------
