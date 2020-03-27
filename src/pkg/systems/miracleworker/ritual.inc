include ":vampire:common";

//-----------------------------------------------------------------------

CONST EV_GOTO := 0x1234;
//CONST REGENDUR := 10800;
CONST REGENDUR := 10800;
CONST PROP_REGENERATIONEND := "MiracleRegenerationEnd";

//-----------------------------------------------------------------------

function FindEmptyChamber(Cfg)

  var dkeys := GetConfigStringKeys(Cfg);
  var data := GetGlobalProperty("RegenerationChambers");
  var keys := array;
  
  foreach key in dkeys
    if(Find(lower(key),lower("chamb"),0))
      keys.append(key);
    endif
  endforeach
  if(!data)
    data := dictionary;
    foreach key in keys
      data[key] :="empty";
    endforeach
    SetGlobalProperty("RegenerationChambers",data);
  endif
  
  foreach key in keys
    if(!data[key])
      data[key]:="empty";
    endif
    if(data[key]=="empty")
      return key;
    endif
  endforeach

  return error;

endfunction

//--------------------------------------------------------------------------

function SetChamberBusy(who,key)
  var data := GetGlobalProperty("RegenerationChambers");
 
  if(!data)
    return 0; 
  elseif(data[key]=="empty")
    data[key]:=who.serial;
    SetGlobalProperty("RegenerationChambers",data);
    return 1;
  else
    return 0;
  endif
  
endfunction

//-------------------------------------------------------------------------

function SetChamberEmpty(key)
  var data := GetGlobalProperty("RegenerationChambers");
 
  if(!data)
    return 0; 
  elseif(data[key])
    data[key]:="empty";
    SetGlobalProperty("RegenerationChambers",data);
    return 1;
  else
    return 0;
  endif
  
endfunction

//------------------------------------------------------------------------

function GetServantList(me,byref npcs)
  
  npcs := array;
  
  var mobiles := ListMobilesNearLocation( me.x, me.y, me.z, 10, me.realm ); 
  
  foreach mobile in mobiles
    if(mobile.npctemplate=="chrysalis")
      npcs.append(mobile);
      PrintTextAbove(mobile, "*hrhr*");
    endif
  endforeach
  
endfunction

//------------------------------------------------------------------------

function FindNearestServant(me,servants)
  
  var dist;
  var nearest;
  foreach servant in servants
    
    if( Distance(me,servant)<dist or !dist)
      dist := Distance(me,servant);
      nearest := servant;
    endif
    
  endforeach
  
  return nearest;
  
endfunction
    

//-------------------------------------------------------------------------

function BuzzPlayer(who,servant)
  
  var dest := struct;
  dest.+x:=who.x;
  dest.+y:=who.y;
  dest.+z:=who.z;
  
  var tEv := struct;
  tEv.+type := EV_GOTO;
  tEv.+dest := who;
  SendEvent(servant,tEv);
  
  var bWaitTime := ReadGameClock()+30;
  
  while(Distance(who,servant)>1 and bWaitTime > ReadGameClock())
    sleep(1);
  endwhile
  
  PrintTextAbove(servant, "*strzyze*");
  var hair  := GetEquipmentByLayer( who, LAYER_HAIR );
  DestroyItem( hair );
  
endfunction

//-------------------------------------------------------------------------

function Unequip(who)

  if(who.dead)
    return;
  endif

  UnEquipAll(who);
  
  var BankBox := FindBankBox(who);

  foreach item in ListRootItemsInContainer(who.backpack)
    
      MoveItemToContainer(item, BankBox);
   
  endforeach
  
endfunction
  
//-------------------------------------------------------------------------

function MoveToChamber(who,chamber,cfg)
  
  var dest := struct;
  dest.+x := cfg[chamber].posx;
  dest.+y := cfg[chamber].posy;
  dest.+z := cfg[chamber].posz;
  dest.+realm := cfg[chamber].realm;
  
  var RelTime := ReadGameClock()+REGENDUR;
  SetObjProperty(who,PROP_REGENERATIONEND,RelTime);
  MoveObject(who, dest.x, dest.y, dest.z, dest.realm, MOVECHAR_FORCELOCATION, MOVETYPE_CMD);
  SetChamberBusy(who,chamber);
  return;
  
endfunction
  
//----------------------------------------------------------------------------

function FinishRitual(who)
  
  var Chambers := GetGlobalProperty("RegenerationChambers");
  var Cfg := ReadConfigFile(":miracleworker:miracleconfig");
  var keys := GetConfigStringKeys(Cfg);
  
  foreach key in keys
    if(Chambers[key]==who.serial)
      SetChamberEmpty(key);
      RemoveSkill(who);
      EraseObjProperty(who, PROP_REGENERATIONEND );
      who.frozen := 0;
      
      var exitCord := struct;
      exitCord.+x := Cfg["exit"].posx;
      exitCord.+y := Cfg["exit"].posy;
      exitCord.+z := Cfg["exit"].posz;
      exitCord.+realm := Cfg["exit"].realm;
      MoveObject(who, exitCord.x, exitCord.y, exitCord.z, exitCord.realm, MOVECHAR_FORCELOCATION, MOVETYPE_CMD);
      return;
    endif
  endforeach
  
endfunction

//--------------------------------------------------------------------------

function RemoveSkill(who)
  
  if(IsVampire(who))
    EraseObjProperty(who, "IsVampire");
    UndoMod(who, MOD_STR);
    UndoMod(who, MOD_DEX);
    UndoMod(who, MOD_INT);
    UndoMod(who, MOD_GRAPHIC);
    UndoMod(who, MOD_COLOR);
    UndoMod(who, MOD_LIGHT);
    EraseObjProperty(who, "#VampChangeComplete");
    EraseObjProperty(who, "EndedVampire");
    EraseObjProperty(who, "#VampireStartSkill");
    EraseObjProperty(who, "CanBeVampire");

    if(CInt(GetObjProperty(who, "ColorBeforeVamp")) or who.color == VAMP_COLOR)
      
      var OldColor  := GetColorBefVamp(who);
      who.color     := OldColor;
      who.truecolor := OldColor;
      
    endif
    
    EraseObjProperty(who, "ColorBeforeVamp");
    SetBaseSkill(who, SKILLID_VAMPIRE, 0);
    return 1;
    
  elseif(IsWearwolf(who))
    
    EraseObjProperty(who, "IsWearWolf");
    UndoAllMods(who, UMDF_WAIT);
    EraseObjProperty(who, "#WearWolfMorph");
    EraseObjProperty(who, "#WearWolfStartSkill");
    EraseObjProperty(who, "#EndedMorph");
    EraseObjProperty(who, "#WWInitiateComplete");
    EraseObjProperty(who, "CanBeWearWolf");
    EraseObjProperty(who, "WolfName");
    SetBaseSkill(who, SKILLID_WEARWOLF, 0);
    
    if(!who.murderer)
      
      SetCriminal(who, 0);
      
    endif
    
    return 1;
    
  else 
    return 0;
  endif
    
endfunction

//-----------------------------------------------------------------------
