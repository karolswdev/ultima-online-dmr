include "include/noto";
include "include/items";

//----------------------------------------------------------------------------
// GRAVES
//----------------------------------------------------------------------------

function DigGrave(who,Loc,tool,byref IterData)
  
  var Range   := 3;
  var Realm   := who.realm;

  foreach Multi in ListMultisInBox(who.x-Range, who.y-Range, -127, who.x+Range, who.y+Range,127, Realm)
     if(Multi.isa(POLCLASS_MULTI))
       SendSysMessage(who, "W tym miejscu nic nie wykopiesz.",FONT_NORMAL,COLOR_RED);
       return error;
     endif
  endforeach

  if(GetEquipmentByLayer(who,LAYER_HAND1) or GetEquipmentByLayer(who,LAYER_HAND2))
    SendSysMessage(who,"Musisz miec wolne rece!",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(IterData.curIter == 1)
    SendSysMessage (who, "Zaczynasz rozkopywac grob...",FONT_NORMAL,COLOR_GREEN);
  endif

  if(!CheckToolWear(who,tool,SKILLID_MINING))
    return error;
  endif

  TurnCharTowardLocation(who, Loc);

  if(!HarvestDelay(who, IterData, 0x126, UACTION_PICK, "Rozkopujesz grob..."))
    return error;
  endif

  var Num := HarvestResource( "graves" , Loc.x , Loc.y , 1 , 1, who.realm );

  if(CInt(Num) <= 0)

    Sendsysmessage(who, "Tutaj nic wiecej nie ma.",FONT_NORMAL,COLOR_RED);
    return error;

  else
    var Amt := 15+RandomInt(40);
    var Gold := CreateGold(who.backpack, Amt, who);

    if(Gold)
      PerformAction(who, UACTION_PICK);
      SendSysMessage(who, "Udalo Ci sie znalezc troche zlota w grobie.",FONT_NORMAL,COLOR_GREEN);

      if(RandomInt(100) < 10)
        var ObjType := GetRandomItemFromGroups("CanDigg");
        var Item;
        if(ObjType)
          if(CanBeMagicObjType(ObjType))
            Item := CreateItemInPack(who,ObjType);
          endif
        endif
        if(Item)
          var MagicLvl := 5;
          if(RandomInt(10)<3)
            MagicLvl := MagicLvl + RandomInt(2);
          endif
          if(TryToMakeItemMagic(Item, 80, MagicLvl))
            SendSysMessage(who,"Wykopal"+ggm(who,3)+" tez magiczny przedmiot!",FONT_NORMAL,COLOR_BLUE);
          else
            SendSysMessage(who,"Wykopal"+ggm(who,3)+" poza zlotem jakis przedmiot.",FONT_NORMAL,COLOR_BLUE);
          endif
        endif
      endif

      KarmaLost(who);
    
      if(RandomInt(20) < 15)
        sleep(1);
        var Undead := CreateRPNpc(GetRandomUndead(),who, who.realm);
        if(Undead)
          SendSysMessage(who,"Oho! Wyglada na to, ze obudzil"+ggm(who,3)+" nieumarlego!",FONT_NORMAL,COLOR_RED);
          SendChEvent(Undead,who,EVID_ENGAGED);
          SaveSkillSum(Undead);
        endif
        return error;
      endif

      return 1;

    else
      SendSysMessage(who,"Nie masz miejsca w plecaku.",FONT_NORMAL,COLOR_RED);
      return error;
    endif
    
  endif

  return 0;
  
endfunction

//-----------------------------------------------------------------------------

function GetRandomUndead()
  
  var GroupCfg := ReadConfigFile("gravenpcs");
  var npcs := GetConfigStringArray(GroupCfg["npcs"],"npc");
  
  var num := RandomInt(npcs.size())+1;
  return npcs[num];
  
endfunction
  