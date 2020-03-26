//-----------------------------------------------------------------------------

var AggroData;
var AggroTarg;
var AggroPlayers;
var AggroTable;

//-----------------------------------------------------------------------------

const AGGRO_FORGET_DELAY := 15;

//-----------------------------------------------------------------------------

function ResetAggro()

  AggroData := struct;
  AggroData.+startTime := 0;
  AggroData.+level := 0;
  AggroData.+points := 0;
  AggroTarg := 0;

  CheckAggroPlayers();

  SetProperty("#AggroLevel", AggroData.level);
  SetProperty("#AggroPoints", AggroData.points);

endfunction

//-----------------------------------------------------------------------------

function CheckAggroPlayers()

  if(!AggroPlayers)
    return;
  endif

  var Keys := AggroPlayers.keys();

  foreach Key in Keys
    if(AggroPlayers[Key].startTime + AGGRO_FORGET_DELAY < ReadGameClock())
      AggroPlayers.erase(Key);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function GetAggroPlayerData(Player)

  if(AggroPlayers)
    if(AggroPlayers.exists(Player.serial))
      return AggroPlayers[Player.serial];
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SetAggroPlayerData(Player)

  if(!AggroPlayers)
    AggroPlayers := dictionary;
  endif

  if(AggroPlayers.exists(Player.serial))
    AggroPlayers[Player.serial].startTime := ReadGameClock();

    return AggroPlayers[Player.serial];
  else
    var Data := struct;
    Data.+startTime := ReadGameClock();
    Data.+level := 0;
    Data.+points := 0;
    AggroPlayers[Player.serial] := Data;
    return Data;
  endif

endfunction

//-----------------------------------------------------------------------------

function SetAggro(who)

  if(AggroTarg == who)
    return;
  endif
  
  CheckAggroPlayers();

  if(AggroTarg)
    AggroPlayers[AggroTarg.serial] := AggroData;
    AggroPlayers[AggroTarg.serial].startTime := ReadGameClock();
  endif

  AggroTarg := who;
  AggroData := SetAggroPlayerData(who);

  if(IsSpecOn(NSPECF_DEBUG))
    syslog("NPC ["+hex(me.serial)+"] SetAggro: "+GetName(who)+" Data: "+AggroData.points+"/"+AggroData.level);
  endif

  SetProperty("#AggroLevel", AggroData.level);
  SetProperty("#AggroPoints", AggroData.points);
  
endfunction

//-----------------------------------------------------------------------------

function IncreaseAggro(Amount)
  
  var oiled := IsOiledShield();
  
  
   if(oiled == 4) // jezeli duzy miks (czyli lvl 4) to moze wbic 4 lvl agro
      AggroData.points := ToMax(AggroData.points + (Amount*1.2), 500);
    elseif(oiled == 3)
      AggroData.points := ToMax(AggroData.points + (Amount*1.1), 400); // tylko szybciej bedzie wbijal agro z malym olejem
    else
      AggroData.points := ToMax(AggroData.points + Amount, 400);
   endif
  
  var NewLevel := GetAggroLevelMult(AggroData.points);

  if(!IsPlayer(opponent))
    NewLevel := Min(3, NewLevel);
  endif

  SetAggroTable(AggroTarg, NewLevel);

  if(AggroData.level != NewLevel)
    AggroData.level := NewLevel;
    SetProperty("#AggroLevel", AggroData.level);
    return 1;
  endif

  SetProperty("#AggroPoints", AggroData.points);
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetAggroTable(AggroTarg, Level)

  if(!AggroTable)
    AggroTable := GetProperty("AggroTable");
  endif
  
  if(!AggroTable or TypeOf(AggroTable) != VAR_DICTIONARY)
    AggroTable := dictionary;
  endif

  if(!AggroTable.exists(AggroTarg.serial))
    AggroTable[AggroTarg.serial] := array(ReadGameClock(), Level);
  elseif(AggroTable[AggroTarg.serial][2] != Level)
    AggroTable[AggroTarg.serial] := array(ReadGameClock(), Level);
  elseif(ReadGameClock() + 15 < AggroTable[AggroTarg.serial][1])
    AggroTable[AggroTarg.serial] := array(ReadGameClock(), Level);
  else
    return;
  endif

  SetProperty("AggroTable", AggroTable);

endfunction

//-----------------------------------------------------------------------------

function DecreaseAggro(Amount)

  Amount := CInt(Amount * 0.8);
  Amount := CInt(Amount / Min(3, AggroData.level+1));

  AggroData.points := ToMin(AggroData.points - Amount, 0);
  
  // +50 aby nie bylo sytuacji, ze gosc wbije level i za sekunde mu spada. Ma zapas w postaci 50 dmg.
  var NewLevel := GetAggroLevelMult(AggroData.points + 50);

  if(AggroData.level > NewLevel)
    AggroData.level := NewLevel;
    SetProperty("#AggroLevel", AggroData.level);
    return 1;
  endif

  SetProperty("#AggroPoints", AggroData.points);
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetAggroLevelMult(Points)

  if(Points > 450)
    return 4;
  elseif(Points > 280)
    return 3;
  elseif(Points > 160)
    return 2;
  elseif(Points > 80)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetAggroLevelText(level)

  case(level)
    1: return "*zdenerwowany*";
    2: return "*rozzloszczony*";
    3: return "*wsciekly*";
    4: return "*rozwscieczony*";
  endcase

  return "";

endfunction

//-----------------------------------------------------------------------------

function GetAggroChangeChance()

  if(AggroData.level == 4)
    return 1; // 4 poziom tankerski, 1% szans ;)
  endif

  return RandomInt(3) + (28 - (AggroData.level * 8));

endfunction

//-----------------------------------------------------------------------------
// NPC - zastosowanie
//-----------------------------------------------------------------------------

function AggroNpcReset()

  ResetAggro();

endfunction

//-----------------------------------------------------------------------------

function AggroNpcDamaged(Npc, Source, Dmg)

  if(AggroTarg == Source or !Source)
    if(IncreaseAggro(Dmg))
      var Text := GetAggroLevelText(AggroData.level);

      if(Text)
        PrintTextAbove(Npc, Text + "(" + AggroData.level + ". poziom)", FONT_NORMAL, RED_COLOR);
      endif
    endif
  else
    if(DecreaseAggro(Dmg))
      PrintTextAbove(Npc, "*wrrrr* (poziom agresji obniza sie)");
    endif
  endif
  
endfunction

//-----------------------------------------------------------------------------

function AggroNpcOpponentHealed(Npc, Healer, Amount)

  if(AggroTarg != Healer)
    
    if(DecreaseAggro(Amount))
      PrintTextAbove(Npc, "*wrrrr*");
    endif
    
  endif

endfunction

//-----------------------------------------------------------------------------

function AggroNpcChanged(Npc)

  PrintTextAbove(Npc, "*arghhh*");
  DoMod(Npc, MOD_RUN_SPEED, RUNSPEED_FAST, 5);
  
endfunction

//-----------------------------------------------------------------------------


function IsOiledShield()

	var Oiled;

   if(GetObjProperty(opponent, "Oiled"))
	Oiled := GetObjProperty(opponent, "Oiled");
	if(Oiled)
      return Oiled.level;
	else
      return 0;  
	endif
   endif
   
   if(IsTamed(opponent))
	var master := GetMaster(opponent);
	var masterVetSkill := GetEffectiveSkill(opponent, SKILLID_VETERINARY);
	var level := CInt(masterVetSkill/5);
	return level;
   endif
	
   
   
   var hand1 := Getequipmentbylayer(opponent,LAYER_HAND1);
   var hand2 := Getequipmentbylayer(opponent,LAYER_HAND2);
   
   if(hand1.isa(POLCLASS_ARMOR))
      Oiled := GetObjProperty(hand1, "Oiled");
   else
      Oiled := GetObjProperty(hand2, "Oiled");  
   endif

   if(Oiled)
      return Oiled.level;
   else
      return 0;  
   endif

  endfunction

//-----------------------------------------------------------------------------
   