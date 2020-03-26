//--------------------------------------------------------------------------------

include "include/npcs";
include "include/loots";
include ":quests:quest_ctrl";
include "include/checks/locchecks";

//--------------------------------------------------------------------------------

function CriticalFailure(who, targetsquare)

  var tnum := RandomInt(3)+1,it;

  case(tnum)
    1: if(!IsNpcNear(targetsquare.x, targetsquare.y, targetsquare.z,who.realm, 10, "seaserpent"))
         it := CreateNpc("seaserpent", targetsquare.x, targetsquare.y, targetsquare.z,who.realm);
       endif

    2: if(!IsNpcNear(targetsquare.x, targetsquare.y, targetsquare.z,who.realm, 10, "walrus"))
         it := CreateNpc("walrus", targetsquare.x, targetsquare.y, targetsquare.z,who.realm);
       endif

    3: if(!IsNpcNear(targetsquare.x, targetsquare.y, targetsquare.z,who.realm, 10, "waterelemental"))
         it := CreateNpc("seaserpent", targetsquare.x, targetsquare.y, targetsquare.z,who.realm);
       endif
  endcase
  
  if(it)
    case(tnum)
      1: SendSysMessage(who, "Oops, to nie ryba!",FONT_NORMAL,COLOR_RED);
         SendChEvent(it, who, EVID_ENGAGED);
      2: SendSysMessage(who, "Mors przyplywa zobaczyc co robisz.",FONT_NORMAL,COLOR_GREEN);

      3: SendSysMessage(who, "Przyciagn"+ggm(who,5)+" uwage dosc mocnej ryby!",FONT_NORMAL,COLOR_RED);
         SendChEvent(it, who, EVID_ENGAGED);
    endcase

    SaveSkillSum(It);
  endif

endfunction

//--------------------------------------------------------------------------------

function AssignSosMsgData(Message)

  var SosCfg := ReadConfigFile(":fishing:sosarea");
  if(!SosCfg)
    LogError("UseSosBootle: nie mozna bylo odczytac configa ':fishing:sosarea'");
    return 0;
  endif
  
  var MaxKey := GetConfigMaxIntKey(SosCfg);
  var Elem   := SosCfg[RandomInt(MaxKey)+1];

  var X      := CInt(elem.x)+RandomInt(CInt(elem.range)*2) - CInt(elem.range);
  var Y      := CInt(elem.y)+RandomInt(CInt(elem.range)*2) - CInt(elem.range);
  
  SetObjProperty(message, "tileX", X);
  SetObjProperty(message, "tileY", Y);
  SetObjProperty(message, "tileRlm", Elem.Realm);
  SetObjProperty(message, "msgnum", RandomInt(4));

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetSOSTreasure(who, sosmsg)

  if(CheckSkill(who, SKILLID_FISHING, 80, 70))
    var ItemObj, ObjTypes, Item;

    if(RandomInt(100) < 75)
      case(RandomInt(2))
        0: ObjTypes := GetConfigStringArray(fGroupCfg["Paintings"],"Item");
        1: ObjTypes := GetConfigStringArray(fGroupCfg["Pillows"],"Item");
      endcase

      ItemObj := ObjTypes[RandomInt(ObjTypes.size())+1];
      
      if(ItemObj)
        Item := CreateHarvResource(who, SKILLID_FISHING, GetHarvestDescriptor(ItemObj, who), 1);
      endif
      
      SetName(Item, GetDescForItem(ItemObj) + " [Wydobyte z wraku]");
      Item.movable := 1;

      SendSysMessage(who, "Znalazl"+ggm(who,3)+" cos interesujacego!",FONT_NORMAL,COLOR_GREEN);
    else
      if(!DestroyItem(sosmsg))
        SendSysMessage(who, "Oh, cos duzego dopiero co Ci ucieklo!",FONT_NORMAL,COLOR_RED);
        return 0;
      endif

      SendSysMessage(who, "Zlapal"+ggm(who,3)+" zatopiony skarb!",FONT_NORMAL,COLOR_BLUE);
      var chest := CreateHarvResource(who, SKILLID_FISHING, GetHarvestDescriptor(0x9e40, who), 1);

      case(RandomInt(4) + 1)
        1: chest.graphic := 0xe40;
        2: chest.graphic := 0xe41;
        3: chest.graphic := 0xe7c;
        4: chest.graphic := 0x9ab;
      endcase

      SetName(Chest, "metalowa skrzynka [Wydobyta z wraku]");
      chest.movable := 1;
      CreateLoot(chest, RandomInt(2) + 5);
    endif
  else
    SendSysMessage(who, "Oh, cos duzego dopiero co Ci ucieklo!",FONT_NORMAL,COLOR_RED);
    DestroyItem(sosmsg);
  endif

  return 0;
  
endfunction

//--------------------------------------------------------------------------------

function isSOS(who, Loc)

  var packitems := EnumerateItemsInContainer(who.backpack);
  var i, sosx, sosy;

  for(i := 1; i <= packItems.size(); i := i + 1)

    if(packitems[i].objtype == 0xa363)
      sosx := GetObjProperty(packitems[i], "tileX");
      sosy := GetObjProperty(packitems[i], "tileY");
      if(coordist(Loc.x, Loc.y, sosx, sosy) < 5)
        GetSOSTreasure(who, packitems[i]);
        return 1;
      endif
    endif

  endfor

  return 0;

endfunction

//--------------------------------------------------------------------------------
