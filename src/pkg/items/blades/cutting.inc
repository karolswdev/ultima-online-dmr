//-----------------------------------------------------------------------------

include "include/npcs";
include "include/noto";
include "include/items";
include "include/gumps";
include "include/tools";
include "include/mobile/npcs/specs";
include "ai/include/abilities";

include "corpse";
include ":guildstone:guild";
include ":lumberjacking:trees";

//=============================================================================
//-----------------------------------------------------------------------------
// Using Bladed Tool
//-----------------------------------------------------------------------------
//=============================================================================

function CheckOtherChop(who,blade,Loc)

  if(!Loc.container)
    if(coordist(who.x, who.y, Loc.x, Loc.y) > 1)
      SendSysMessage(who, "Stoisz za daleko.", FONT_NORMAL, COLOR_RED);
      return;
    endif
  endif

  var UseTimes := 1;
  var Res := 0;

  if(CanProcessShearing(who, Loc))
    Res := ProcessShearing(who, Loc);
    UseTimes := 3;
  elseif(Loc.objtype == UOBJ_CORPSE)
    CheckToolWear(who, blade, SKILLID_TRACKING, 5);
    ReleaseItem(blade);
    CarveCorpse(who, Loc); // carveCorpse robi waita i w ogole tam jest jeden wielki mess, wiec niech sobie leci na detachu...
    return;
  elseif(IsCutItem(Loc.objtype))
    ProcessCut(who, Blade, Loc);
    Res := 1;
  elseif(IsTreeObjType(Loc.objtype))
    CarveTree(who, Loc);
    Res := 1;
  elseif(CanCrashBones(blade, Loc))
    CrashBones(who, Loc);
    UseTimes := 15;
    Res := 1;
  elseif(CanCrashRaptileEgg(blade, Loc))
    CrashRaptileEgg(who, Loc);
    UseTimes := 15;
    Res := 1;
  elseif(blade.objtype == UOBJ_SKINNINGKNIFE and Loc.objtype == UOBJ_RUNEBOOK)
    if(GetEffectiveSkill(who, SKILLID_INSCRIPTION) >= 110);
      Binding(who, Loc);
      return;
    else
      SendSysMessage(who,"Twoje umiejetnosci sa zbyt male.",FONT_NORMAL,COLOR_RED);
      return;
    endif
  else
    SendSysMessage(who, "Nie wiesz jak uzyc te przedmioty razem.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(Res)
    CheckToolWear(who, blade, SKILLID_TRACKING, UseTimes);
  endif

endfunction

//-----------------------------------------------------------------------------

function CanCrashBones(blade, Loc)

  var Graphics := GetItemGroupObjTypes("GroundBones");

  if(Loc.graphic in Graphics)
    var ItemElem := GetItemConfig(blade);

    if(ItemElem.Attribute == ATTRIBUTEID_AXESMANSHIP or ItemElem.Attribute == ATTRIBUTEID_MACEFIGHTING)
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CrashBones(who, Loc)

  TurnCharToward(who, Loc);

  if(!MakeNoiseDependant(who, Loc, 0x13A, 1500, UACTION_CHOP, 3))
    if(!Loc)
      SendSysMessage(who, "Kosci rozpadly sie!!", FONT_NORMAL, COLOR_RED_LIGHT);
    endif
    return;
  endif
  
  if(HaveChance(50))
    SendSysMessage(who, "Zmiazdzyl"+ggm(who,3)+" kosci!", FONT_NORMAL, COLOR_GREEN);

    PlaySoundEffect(who, 0x48E);
    PlayPcStEffect(Loc.x, Loc.y, Loc.z, Loc.realm, FX_EXPLODE_2, 999, 16, 16, 2);
    StopSpawnBaseItemControl(Loc);
    DropContainerContent(Loc);

    if(!ReturnTempItem(Loc))
      DestroyItem(Loc);
    endif
  else
    SendBad(who, "Nie udalo sie zmiazdzyc kosci!");
  endif

endfunction

//-----------------------------------------------------------------------------

function CanCrashRaptileEgg(blade, Loc)

  var Objs := GetItemGroupObjTypes("RaptileEggs");
  var Graphics := dictionary;

  foreach Obj in Objs
    Graphics[GetItemConfig(Obj).Graphic] := Obj;
  endforeach

  if(Graphics.exists(Loc.graphic))
    if(GetItemConfig(Graphics[Loc.graphic]).Color == Loc.color)
      var ItemElem := GetItemConfig(blade);

      if(ItemElem.Attribute == ATTRIBUTEID_AXESMANSHIP or ItemElem.Attribute == ATTRIBUTEID_MACEFIGHTING)
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CrashRaptileEgg(who, Loc)

  TurnCharToward(who, Loc);

  if(!MakeNoiseDependant(who, Loc, 0x13A, 1500, UACTION_CHOP, 3))
    if(!Loc)
      SendSysMessage(who, "Jajo peklo!!", FONT_NORMAL, COLOR_RED_LIGHT);
    endif
    return;
  endif

  if(HaveChance(50))  
    SendSysMessage(who, "Zmiazdzyl"+ggm(who,3)+" jajo gada.", FONT_NORMAL, COLOR_GREEN);

    PlaySoundEffect(who, 0x48E);
    PlayPcStEffect(Loc.x, Loc.y, Loc.z, Loc.realm, FX_EXPLODE_2, 999, 16, 16, 2);
    StopSpawnBaseItemControl(Loc);

    if(!ReturnTempItem(Loc))
      DestroyItem(Loc);
    endif
  else
    SendBad(who, "Nie udalo sie zmiazdzyc jaja!");
  endif

endfunction

//-----------------------------------------------------------------------------

function IsCutItem(ItemObj)

  var Cfg := ReadConfigFile(":blades:cut");

  if(Cfg[ItemObj])
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function ProcessCut(who, Blade, Item)

  start_script(":blades:cut_item", array(who, Blade, Item, Item.objtype, CRAFT_MAX_ITERATIONS));

endfunction

//-----------------------------------------------------------------------------

function IsFurniture(Item)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  return CInt(ItemCfg[Item.objtype].Furniture);

endfunction

//-----------------------------------------------------------------------------

function ChopFurniture(me,Item)

  if(!StandardItemCheck(me, Item, NEW_DIST_CHECK | NO_COMBAT_CHECK, 1))
    return;
  endif

  foreach thing in EnumerateItemsInContainer(Item)
    if(Item.container)
      MoveItemToContainer(thing, Item.container);
    else
      SetObjProperty(thing, "#CorpseMoveGround", ReadGameClock());
      MoveObject(thing, Item.x, Item.y, Item.z, Item.realm, MOVEITEM_FORCELOCATION);
    endif
  endforeach

  if(IsTrappedItem(Item))
    ArmTrap(me,Item);
  endif

  var LogType  := GetItemMakeMod(Item);
  if(!LogType)
    LogType := 1;
  endif

  var BoardObj := GetBoardObjTypes()[LogType];
  var CarpCfg  := ReadConfigFile(":carpentry:carpentry");
  var Material := SplitWords(CarpCfg[Item.objtype].material);

  if(!BoardObj or !CInt(Material[2]) or Material[1] != "wood")
    SendSysMessage(me,"Nie uda Ci sie tego porabac.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(!MakeNoise(me, 0x13A, 1500, UACTION_CHOP))
    return;
  endif

  Material[2] := CInt(Material[2]);

  var Amount := CInt(Material[2] / 5) + CInt(Material[2] * (CInt(GetEffectiveSkill(me,SKILLID_LUMBERJACKING) / 4) * 0.01)) + RandomInt(CInt(Material[2] / 6));
  if(Amount >= Material[2])
    Amount := Material[2] - 1;
  endif

  if(Amount < 1)
    Amount := 1;
  endif

  DestroyItem(Item);
  CreateItemInPack(me, BoardObj, Amount);

endfunction

//-----------------------------------------------------------------------------

function CanProcessShearing(who, Npc)

  if(!Npc.isa(POLCLASS_NPC))
    return 0;
  elseif(IsSummoned(Npc))
    return 0;
  endif

  if(!StandardMobCheck(who, Npc, NO_COMBAT_CHECK))
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function ProcessShearing(who, Npc)

  var Res := CanNpcShearing(Npc);

  if(!Res)
    SendSysMessage(who, Res.errortext, FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(NpcShearing(Npc))
    CreateItemInPack(who, GetNpcConfig(Npc).ShearingItem, 3+RandomInt(5));
    SendSysMessage(who, "Ostrzygl"+ggm(who,3)+" zwierze "+Npc.name+".", FONT_NORMAL, COLOR_BLUE);
    return 1;
  else
    SendSysMessage(who, "Oopppss.. cos poszlo nie tak!", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function CarveTree(who, Tree)

  if(CoorDist(Tree.x,Tree.y,who.x,who.y) >= 3)
    SendSysMessage(who, "Nie widzisz tego.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(!MakeNoise(who, 0x13A, 500, UACTION_CHOP))
    return;
  endif

  var Amt := CInt(GetEffectiveSkill(who, SKILLID_TRACKING) / 10);
  if(!Amt)
    Amt := 1;
  endif

  CreateItemInPack(who, UOBJ_KINDLING, Amt);
  SendSysMessage(who, "Uci"+ggm(who,5)+" troche drewna na podpalke i wlozyl"+ggm(who,3)+" do plecaka.",FONT_NORMAL,COLOR_GREEN);

endfunction

//-----------------------------------------------------------------------------

function Binding(who, RuneBook)
  
  var twine := FindObjTypeInContainer(who.backpack,UOBJ_TWINE);
        
  if(!twine or twine.amount < 5)
    SendSysMessage(who,"Masz za malo dratwy.",FONT_NORMAL,COLOR_RED);
    return;
  endif

  if(!StandardItemCheck(who,twine))
    return;
  endif

  if(!StandardItemCheck(who,RuneBook))
    return;
  endif

  SendSysMessage(who, "Ktora skore uzyc do oprawy runebooka ?",FONT_NORMAL,COLOR_GREEN);
  var Leather := Target(who);

  if(!Leather)
      SendSysMessage(who, "Anulowano.",FONT_NORMAL,COLOR_RED);
      return;
  endif

  if(Leather.objtype in GetLeatherObjTypes())
    if(Leather.amount >= 10)
	  if(!StandardItemCheck(who,Leather))
	    return;
	  endif
          SendSysMessage(who, "Probujesz zmienic oprawe runebooka...",FONT_NORMAL,COLOR_GREEN);
          if(!MakeNoise(who,SOUND_EFFECT_INSCRIPTION, 1500, 0, 10))
            return;
          endif
	  sleep(1);
          if(SubtractAmount(twine, 5) and SubtractAmount(Leather, 10))
            RuneBook.color := Leather.color;
            SendSysMessage(who, "Oprawil"+ggm(who,3)+" runebooka!",FONT_NORMAL,COLOR_GREEN);
            return;
          else
            SendSysMessage(who, "Nie udalo sie.",FONT_NORMAL,COLOR_RED);
            return;
          endif
    else
      SendSysMessage(who,"Masz za malo skory.",FONT_NORMAL,COLOR_RED);
      return;
    endif
  else
    SendSysMessage(who,"Nie wiesz co z tym zrobic.",FONT_NORMAL,COLOR_RED);
    return;   
  endif
  
  
endfunction

//--------------------------------------------------------------------------------
