//-----------------------------------------------------------------------------

include "include/objtypes/misc";
include "include/move";

//-----------------------------------------------------------------------------

const TR_CFG              := ":tourney:tourney";
const TR_ALI_CFG          := ":tourney:al_items";
const TRCFG_GLOBAL        := "Global";
const TRCFG_MISCITEMS     := "MiscItems";
const TRCFG_EQPITEMS      := "EqpItems_";

// ----

const TR_STATUS           := 1;

const TRLOC_MAIN          := 2;
const TRLOC_RED_ROOM      := 3;
const TRLOC_BLUE_ROOM     := 4;
const TRLOC_GUARD         := 5;
const TRLOC_OUTROOM       := 8;
const TRLOC_ROOMFOOD      := 9;
const TRLOC_ARCHROOM      := 10;
const TRLOC_WARRROOM      := 11;
const TRLOC_MAGEROOM      := 12;
const TRLOC_BANS          := 13;
const TRLOC_STABLES       := 15;
const TRLOC_EVERYTHING    := 16;

const TRLOC_ISLAND        := 6;
const TRLOC_MAINROOMS     := 7;

const TR_TICKET_STATUS    := 14;

const TR_ENTRANCE_PORTAL  := 17;

// ----

const TR_STAT_BEGAN       := 1;
const TR_STAT_STARTED     := 2;
const TR_STAT_ENDED       := 3;
const TR_STAT_END         := 4;

const TR_ENTER_COST       := 5000;

const TR_CLASS_WARRIOR    := 1;
const TR_CLASS_ARCHER     := 2;
const TR_CLASS_MAGE       := 3;

const TR_FSK_SWORDS       := 1;
const TR_FSK_MACES        := 2;
const TR_FSK_FENC         := 3;
const TR_FSK_WREST        := 4;
const TR_FSK_ARCHERY      := 5;
const TR_FSK_AXES         := 6;

const TR_KILL_POINTS      := 50;

//-----------------------------------------------------------------------------
// Tr Items
//-----------------------------------------------------------------------------

const UOBJ_RED_TC         := 0xA248;
const UOBJ_BLUE_TC        := 0xA249;

//-----------------------------------------------------------------------------

function IsInTourneyIsland(x,y)

  var Loc := GetTourneyProp(TRLOC_ISLAND);
  
  if(Loc)
    if(x >= Loc[1] and x <= Loc[3] and y >= Loc[2] and y <= Loc[4])
      return 1;
    endif
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsInTourneyLoc(LocId, x, y)

  var Loc := GetTourneyProp(LocId);
  
  if(Loc and Loc.size() == 4)
    if(x >= Loc[1] and x <= Loc[3] and y >= Loc[2] and y <= Loc[4])
      return 1;
    endif

    return 0;
  endif

  return error;

endfunction

//-----------------------------------------------------------------------------

function MoveFromIsland(who,corpse)

  var Loc := GetTrFightProps(who);
  
  if(!Loc)
    Loc := TRLOC_MAIN;
  endif

  Loc := GetTourneyProp(Loc);

  PlayStationaryEff(who.x,who.y,who.z,FX_HEAL_EFFECT,0x1F,0x1F);
  MoveObject(who, Loc[1], Loc[2], Loc[3], who.realm, MOVECHAR_FORCELOCATION);
  PlayStationaryEff(Loc[1], Loc[2], Loc[3], FX_HEAL_EFFECT,0x1F,0x1F);
  ResurrectPlayer(who);
  
  foreach Item in EnumerateItemsInContainer(corpse)
    DestroyItem(Item);
  endforeach

  EraseTrFightProps(who);

endfunction

//-----------------------------------------------------------------------------

function GetTrFightProps(who)

  return GetObjProperty(who,"TourneyProps");

endfunction

//-----------------------------------------------------------------------------

function EraseTrFightProps(who)

  return EraseObjProperty(who,"TourneyProps");

endfunction

//-----------------------------------------------------------------------------

function GetTrTeam(who)

  return CInt(GetObjProperty(who,"TrTeam"));

endfunction

//-----------------------------------------------------------------------------

function GetTourneyProp(Lc := 0)

  var Props := GetGlobalProperty("TourneyProps");
  if(!Props)
    Props := {};
    SetGlobalProperty("TourneyProps",Props);
  endif

  if(Lc)
    return Props[Lc];
  endif
  
  return Props;
  
endfunction

//-----------------------------------------------------------------------------

function SetTourneyProp(Props,Lc := 0)

  var BProps := GetGlobalProperty("TourneyProps");
  
  if(!BProps)
    BProps := {};
  endif
  
  if(Lc)
    BProps[Lc] := Props;
  else
    BProps := Props;
  endif
  
  SetGlobalProperty("TourneyProps",BProps);

endfunction

//-----------------------------------------------------------------------------

function MoveToTrOutRoom(who)

  var Loc := GetTourneyProp(TRLOC_OUTROOM);
  
  if(Loc)
    MoveObject(who,Loc[1],Loc[2],Loc[3],who.realm,MOVECHAR_FORCELOCATION);
  else
    LogNotice("[Tourney] MoveToTrOutRoom: Pozycja pokoju poczatkowego nie istnieje!", LOGF_NO_NEED, LOGF_WORLD);
  endif

endfunction

//-----------------------------------------------------------------------------

function CreateTrPack(who,Class := 0,Team := 0,FSk := 0)

  ClearScriptObjCreator();

  if(!Class or Class < 1 or Class > 3)
    Class := ChooseClass(who);
  endif

  if(!Class)
    SendSysMessage(who,"Anulowano",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!Team or Team < 1 or Team > 2)
    Team := ChooseTeam(who);
  endif

  if(!Team)
    SendSysMessage(who,"Anulowano",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  if(!Fsk or Fsk < 1 or Fsk > 5)
    Fsk := ChooseFsk(who);
  endif

  if(!Class or !Team or !FSk)
    SendSysMessage(who,"Anulowano",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  var Equipment := GetEquipmentByClass(Class,FSk);  
  var TrCfg     := ReadConfigFile(TR_CFG);
  var GlobElem  := TrCfg[TRCFG_GLOBAL];
  var StdItems  := GetConfigStringArray(TrCfg["StdItems_"+Class], "Item");
  var cItem;

  var Bag       := CreateItemInCont(who.backpack,UOBJ_TR_BAG);



  SetObjProperty(Bag, PROP_TRITEM_TEMP, ReadGameClock());
  var Name,Color;

  case(Team)
    1: Name  := GlobElem.BlueName;
       Color := GlobElem.BlueColor;
    2: Name  := GlobElem.RedName;
       Color := GlobElem.RedColor;
  endcase

  if(!Name)
    Name := "";
  endif
 
  if(!Color)
    Color := 0;
  endif

  Bag.color  := Color;
 
  SetName(Bag,"Ekwipunek "+Name);
  SetObjProperty(Bag, PROP_TRITEM_TEMP, ReadGameClock());
  
  //var IBag := CreateItemInCont(Bag,UOBJ_TR_BAG),i;
  //var cItem;

  //IBag.color := Color;
  //SetName(IBag,Name);
 
  //var ArmorBag  := CreateBagOfItems(Bag, 0 ,Color,"Zbroje", 0);
  //var WeaponBag := CreateBagOfItems(Bag, 0 ,Color,"Bronie", 0);

  var ArmorBag    := CreateItemInCont(Bag,UOBJ_TR_BAG);
  var WeaponBag   := CreateItemInCont(Bag,UOBJ_TR_BAG);
  ArmorBag.color  := Color;
  WeaponBag.color := Color;
  SetName(ArmorBag,"Zbroje");
  SetName(WeaponBag,"Bronie");
  SetObjProperty(ArmorBag, PROP_TRITEM_TEMP, ReadGameClock());
  SetObjProperty(WeaponBag, PROP_TRITEM_TEMP, ReadGameClock());


  foreach Item in Equipment  // do zbroji
    cItem := CreateItemInCont(Bag,Item);
    
    if(!cItem)
      LogError("CreateTrPack: Bledny ObjType: "+Item);
      continue;
    endif

    SetObjProperty(cItem, PROP_TRITEM_TEMP, ReadGameClock());
    cItem.color := Color;

    if(cItem.isa(POLCLASS_ARMOR))
      cItem.ar_mod := GlobElem.ArMod;
      MoveToCont(CItem,ArmorBag);
    elseif(cItem.isa(POLCLASS_WEAPON))
      cItem.dmg_mod := GlobElem.DmgMod;
      MoveToCont(CItem,WeaponBag);
    endif

    SetName(cItem,GetDescForItem(cItem.objtype) + " " + Name);
    
  endforeach

  foreach Item in StdItems  //for do do bandazow itp itd
    Item := SplitWords(Item);
    MakeIntArray(Item);

    if(Item[2] > 1 and !IsStackableObj(Item[1]))
      for i := 1 to Item[2]
        cItem := CreateItemInCont(Bag, Item[1], 1);
      endfor
    else
      cItem := CreateItemInCont(Bag, Item[1], Item[2]);
    endif

    if(!cItem)
      LogError("CreateTrPack: Bledny ObjType: "+Item);
    else
      SetObjProperty(cItem, PROP_TRITEM_TEMP, ReadGameClock());
    endif
  endforeach

  if(Class == TR_CLASS_MAGE)
    CreateBagOfItems(Bag,GetTourRareRegs(),Color," Rzadkie Skladniki", GlobElem.RareRegsAmt);
    CreateBagOfItems(Bag,GetTourRegs(),Color," Skladniki",GlobElem.RegsAmt);
  endif

  CreateBagOfItems(Bag,GetTourMixs(),Color,"Mikstury", GlobElem.MixAmt);
  CreateBagOfItems(Bag,GetTourFood(),Color,"Jedzenie", GlobElem.FoodAmt);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetTourMixs()

  var TrCfg := ReadConfigFile(TR_CFG);

  var Items := SplitWords(GetConfigString(TrCfg[TRCFG_MISCITEMS], "Mixes"));

  foreach Item in Items
    Item := CInt(Item);
  endforeach

  return Items;

endfunction

//-----------------------------------------------------------------------------

function GetTourFood()

  var TrCfg := ReadConfigFile(TR_CFG);

  var Items := SplitWords(GetConfigString(TrCfg[TRCFG_MISCITEMS], "Food"));

  foreach Item in Items
    Item := CInt(Item);
  endforeach

  return Items;

endfunction
//-----------------------------------------------------------------------------

function GetTourRegs()

  var TrCfg := ReadConfigFile(TR_CFG);

  var Items := SplitWords(GetConfigString(TrCfg[TRCFG_MISCITEMS], "Regs"));

  foreach Item in Items
    Item := CInt(Item);
  endforeach

  return Items;

endfunction

//-----------------------------------------------------------------------------

function GetTourRareRegs()

  var TrCfg := ReadConfigFile(TR_CFG);

  var Items := SplitWords(GetConfigString(TrCfg[TRCFG_MISCITEMS], "RareRegs"));

  foreach Item in Items
    Item := CInt(Item);
  endforeach

  return Items;

endfunction

//-----------------------------------------------------------------------------

function CreateBagOfItems(Bag,Items,Color,Name,Amt := 1)

  var IBag := CreateItemInCont(Bag,UOBJ_TR_BAG),i;
  var cItem;

  IBag.color := Color;
  SetName(IBag,Name);

  foreach Item in Items
    if(IsStackableObj(Item))
      cItem := CreateItemInCont(IBag,Item,Amt);
      if(!cItem)
        LogError("CreateBagOfItems: Bledny ObjType: "+Item);
      else
        SetObjProperty(cItem, PROP_TRITEM_TEMP, ReadGameClock());
      endif
    else
      for(i:=1;i<=Amt;i:=i+1)
        cItem := CreateItemInCont(IBag,Item);
        if(!cItem)
          LogError("CreateBagOfItems: Bledny ObjType: "+Item);
          break;
        else
          SetObjProperty(cItem, PROP_TRITEM_TEMP, ReadGameClock());
        endif
      endfor
    endif
  endforeach


endfunction

//-----------------------------------------------------------------------------

function GetEquipmentByClass(Class,FSk)

  var TrCfg     := ReadConfigFile(TR_CFG);
  var TrElem    := TrCfg[TRCFG_EQPITEMS+Class];

  if(!TrElem)
    LogError("GetEquipmentByClass: Tourney: Nie znaleziono elementu ekwipunku ["+TRCFG_EQPITEMS+Class+"]");
    return array;
  endif

  var EqpItems  := GetConfigString(TrElem, "Armor");
  var Weaps     := GetConfigString(TrElem, "Weapon_"+FSk);

  if(!EqpItems)
    LogError("GetEquipmentByClass: Tourney: Nie znaleziono wpisu [Armor] w ["+TRCFG_EQPITEMS+Class+"]");
    return array;
  endif

  EqpItems := SplitWords(EqpItems);

  if(Weaps)
    Weaps    := SplitWords(Weaps);
    EqpItems := EqpItems + Weaps;
  endif

  foreach Item in EqpItems
    Item := CInt(Item);
  endforeach

  EqpItems.append(UOBJ_TR_ROBE);
  EqpItems.append(UOBJ_TR_CLOAK);

  return EqpItems;

endfunction

//-----------------------------------------------------------------------------

function ChooseClass(who)

  GInit(100,100);
  GResPic(0,0,3600,200,150);

  GTextLine(40,20,510,"Wybierz Klase :");
  GButton(20,50,2714,2715,TR_CLASS_WARRIOR);
  GTextLine(40,50,590,"1. Wojownik");
  GButton(20,70,2714,2715,TR_CLASS_ARCHER);
  GTextLine(40,70,590,"2. Lucznik");
  GButton(20,90,2714,2715,TR_CLASS_MAGE);
  GTextLine(40,90,590,"3. Mag");

  return GSend(who)[0];

endfunction

//-----------------------------------------------------------------------------

function ChooseTeam(who)

  var TrCfg := ReadConfigFile(TR_CFG);

  GInit(100,100);
  GResPic(0,0,3600,200,120);

  GTextLine(40,20,510,"Wybierz Druzyne :");
  GButton(20,50,2714,2715,1);
  GTextLine(40,50,590,"1. "+TrCfg[TRCFG_GLOBAL].BlueName);
  GButton(20,70,2714,2715,2);
  GTextLine(40,70,590,"2. "+TrCfg[TRCFG_GLOBAL].RedName);

  return GSend(who)[0];

endfunction

//-----------------------------------------------------------------------------

function ChooseFSk(who)

  GInit(100,100);
  GResPic(0,0,3600,200,200);

  GTextLine(40,20,510,"Wybierz Walke :");
  GButton(20,50,2714,2715,TR_FSK_SWORDS);
  GTextLine(40,50,590,"1. Mieczami");
  GButton(20,70,2714,2715,TR_FSK_MACES);
  GTextLine(40,70,590,"2. Obuchami");
  GButton(20,90,2714,2715,TR_FSK_FENC);
  GTextLine(40,90,590,"3. Szpadami");
  GButton(20,110,2714,2715,TR_FSK_WREST);
  GTextLine(40,110,590,"4. Piesciami");
  GButton(20,130,2714,2715,TR_FSK_ARCHERY);
  GTextLine(40,130,590,"5. Lucznictwo");
  GButton(20,150,2714,2715,TR_FSK_AXES);
  GTextLine(40,150,590,"6. Toporami");

  return GSend(who)[0];

endfunction

//-----------------------------------------------------------------------------

function EraseEndProps(who)

  EraseTrFightProps(who);
  EraseObjProperty(who,"TrCaptain");
  EraseObjProperty(who,"TrTeam");
  ClearTrEqp(who);

endfunction

//-----------------------------------------------------------------------------

function ClearTrEqp(who)

  var Num := 0;

  foreach Item in EnumerateItemsInContainer(who.backpack)
    if(Item)
      if(GetObjProperty(Item, PROP_TRITEM_TEMP))
        DestroyItem(Item);
        Num := Num + 1;
      endif
    endif
  endforeach

  var Item;

  for i:=1 to 25
    Item := GetEquipmentByLayer(who,i);
    if(GetObjProperty(Item, PROP_TRITEM_TEMP))
      DestroyItem(Item);
      Num := Num + 1;
    endif
  endfor

  var Mount := GetEquipmentByLayer(who, LAYER_MOUNT);
  if(Mount)
    if(IsWearwolfMorphed(who))
      ChangeIntoWearwolf(who);
    elseif(CanMod(who, MOD_MOUNT) == 0)
      UndoMod(who, MOD_MOUNT);
    else
      DestroyItem(Mount);
    endif
    Num := Num + 1;
  endif

  UndoAllMods(who);
  SetCriminal(who, 0);

  return Num;

endfunction

//-----------------------------------------------------------------------------

function IncTourneyPoints(who, pAdd)

  var Points := CInt(GetObjProperty(who, "TourneyPoints"));

  SetObjProperty(who, "TourneyPoints", Points + CInt(pAdd));

endfunction

//-----------------------------------------------------------------------------

function DecTourneyPoints(who, pAdd)

  var Points := CInt(GetObjProperty(who, "TourneyPoints"));

  SetObjProperty(who, "TourneyPoints", Points - CInt(pAdd));

endfunction

//-----------------------------------------------------------------------------

function CheckTourneyDamage(defender, Dmg, attacker)

  Dmg := Cint(Dmg);

  if(!defender.isa(POLCLASS_MOBILE) or !attacker.isa(POLCLASS_MOBILE) or Dmg <= 0)
    return;
  endif

  if(IsInTourneyIsland(defender.x, defender.y))
    var t1 := GetTrTeam(defender);
    var t2 := GetTrTeam(attacker);

    if(t1 and t2)
      if(t1 != t2)
        IncTourneyPoints(attacker, Dmg);
      else
        DecTourneyPoints(attacker, Dmg*2);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckTourneyDeath(ghost, killer)

  if(!ghost.isa(POLCLASS_MOBILE) or !killer.isa(POLCLASS_MOBILE))
    return;
  endif

  if(IsInTourneyIsland(killer.x, killer.y))
    var t1 := GetTrTeam(ghost);
    var t2 := GetTrTeam(killer);

    if(t1 and t2)
      if(t1 != t2)
        IncTourneyPoints(killer, TR_KILL_POINTS);
      else
        DecTourneyPoints(killer, TR_KILL_POINTS*2);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function SetMobTeam(Mob,Team, Force := 0)

  var Loc := GetTourneyProp(Team);

  if(Force)
    PlayStationaryEff(Mob.x,Mob.y,Mob.z,FX_HEAL_EFFECT,0x1f,0x1f);
  else
    for i:=1 to 6
      PlayStationaryEff(Mob.x,Mob.y,Mob.z,FX_HEAL_EFFECT,0x1f,0x1f);
      sleepms(400);
    endfor
  endif

  PlayStationaryEff(Mob.x,Mob.y,Mob.z,FX_HEAL_EFFECT,0x10,0x10);
  PlayStationaryEff(Mob.x,Mob.y,Mob.z,FX_TELE_VANISH,0xf,0xf);

  MoveObject(Mob,Loc[1],Loc[2],Loc[3],Mob.realm,MOVECHAR_FORCELOCATION);

  sleepms(100);
  PlayStationaryEff(Mob.x,Mob.y,Mob.z,FX_TELE_VANISH,0xf,0xf);
  PlayStationaryEff(Mob.x,Mob.y,Mob.z,FX_HEAL_EFFECT,0x10,0x10);
  
  SetObjProperty(Mob,"TrTeam",Team);
  EraseTrFightProps(Mob);

endfunction

//-----------------------------------------------------------------------------

function HideEntrancePortal(who)

  var Portal := FindEntrancePortal(who);
  if(!Portal)
    return;
  endif

  var Hidden := GetObjProperty(Portal, "Hidden");
  if(CInt(Hidden) == 1)
    SendSysMessage(who, "Portal wejsciowy jest juz schowany.", FONT_NORMAL, COLOR_GREEN);
    return;
  endif

  Portal.graphic   := CInt(ItemDesc[Portal.objtype].AnimGrap) + 8;
  Sleepms(300);

  for i := 1 to 7
    Portal.graphic := Portal.graphic - 1;
    Sleepms(300);
  endfor

  Portal.invisible := 1;
  SetObjProperty(Portal, "Hidden", 1);

  SendSysMessage(who, "Schowano portal wejsciowy.", FONT_NORMAL, COLOR_GREEN);

endfunction

//-----------------------------------------------------------------------------

function ShowEntrancePortal(who)

  var Portal := FindEntrancePortal(who);
  if(!Portal)
    return;
  endif

  var Hidden := GetObjProperty(Portal, "Hidden");
  if(CInt(Hidden) == 0 and Hidden != error)
    SendSysMessage(who, "Portal wejsciowy jest juz odkryty.", FONT_NORMAL, COLOR_GREEN);
    return;
  endif

  Portal.graphic   := CInt(ItemDesc[Portal.objtype].AnimGrap);
  Portal.invisible := 0;

  for i := 1 to 8
    Portal.graphic := Portal.graphic + 1;
    Sleepms(300);
  endfor

  Portal.graphic   := CInt(ItemDesc[Portal.objtype].Graphic);
  SetObjProperty(Portal, "Hidden", 0);

  SendSysMessage(who, "Odkryto portal wejsciowy.", FONT_NORMAL, COLOR_GREEN);

endfunction

//-----------------------------------------------------------------------------

function FindEntrancePortal(who)

  var Portal := CInt(GetTourneyProp(TR_ENTRANCE_PORTAL));
  if(!Portal)
    SendSysMessage(who, "Portal wejsciowy nie zostal ustawiony.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  Portal := FindItemBySerial(Portal);
  if(!Portal)
    SendSysMessage(who, "Portal wejsciowy o serialu "+CInt(GetTourneyProp(TR_ENTRANCE_PORTAL))+" nie istnieje.", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  return Portal;

endfunction

//-----------------------------------------------------------------------------
