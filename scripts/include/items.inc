//--------------------------------------------------------------------------------

use uo;
use os;
use polsys;

//--------------------------------------------------------------------------------

include "include/const/misc";
include "include/const/props";
include "include/client/fonts";
include "include/misc";
include "include/names";
include "include/items/made";
include "include/items/creation";
include "include/tools";

//--------------------------------------------------------------------------------

const ITEMF_NO_LOOT          := 0x00000001;
const ITEMF_NO_INSERT        := 0x00000100;
const ITEMF_NO_EQUIP         := 0x00000200;
const ITEMF_INDESTRUCTABLE   := 0x00000400;

//================================================================================
//--------------------------------------------------------------------------------
// Ownership
//--------------------------------------------------------------------------------
//================================================================================

function CanAccess(who, item)

  if(!IsOwner(who,item))
    SendSysMessage(who, "To nie nalezy do Ciebie. Nie mozesz tego uzyc.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif
  
  return 1;
  
endfunction

//-------------------------------------------------------------------------------

function CanModifyItem(who, Item)

  
  if(IsStaff(who))

    if(!item.isa(POLCLASS_ITEM) or IsGmCreated(Item) or IsAdmin(who) or (IsQuestItem(Item) and IsGM(who)))
      return 1;
    endif

    return 0;

  endif

  return error;

endfunction

//-------------------------------------------------------------------------------

function SetItemOwner(item,Owner)

  SetObjProperty(item,PROP_OWNER,Owner.serial);
  
endfunction

//-------------------------------------------------------------------------------

function EraseItemOwner(item)

  EraseObjProperty(item, PROP_OWNER);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Listing
//-----------------------------------------------------------------------------
//=============================================================================

function EnumerateItemsInContainerOfObjtype(container, objtype)

  var ret := { };
  foreach item in EnumerateItemsInContainer(container)
    if ( item.objtype == objtype )
      ret.append(item);
    endif
  endforeach

  return ret;

endfunction

//-----------------------------------------------------------------------------

function ListRootItemsInContainer(container)

	return EnumerateItemsInContainer(container, ENUMERATE_ROOT_ONLY);

endfunction

//-----------------------------------------------------------------------------

function ListRootItemsInContainerOfObjtype(container, objtype)

  var ret := { };
  foreach item in EnumerateItemsInContainer(container , ENUMERATE_ROOT_ONLY)
    if (item.objtype == objtype) 
      ret.append(item);
    endif
  endforeach

  return ret;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Checking
//-----------------------------------------------------------------------------
//=============================================================================

function InInventory(dstCont, it)

  var Cont := it.container;

  while(Cont.isa(POLCLASS_CONTAINER))
    if(Cont == dstCont)
      return 1;
    endif
    Cont := Cont.container;
  endwhile

  return 0;

endfunction

//-------------------------------------------------------------------------------

function GetItemConfig(Obj)

  if(Obj.isa(POLCLASS_ITEM))
    Obj := Obj.objtype;
  endif

  Obj := CInt(Obj);

  var ItemsCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemsCfg[Obj])
    return ItemsCfg[Obj];
  endif

  return error{"errortext" := "Nie znaleziono itemdesc.cfg dla objtype: "+hex(Obj)};

endfunction

//-------------------------------------------------------------------------------

function GetTileConfig(ObjType)

  ObjType := CInt(ObjType);

  if(!ObjType) 
    return 0;
  endif

  if(ObjType > 0x3FFF)
    var iCfg := ReadConfigFile(ITEMS_CFG);
    if(!iCfg[ObjType])
      return 0;
    endif
    ObjType := CInt(iCfg[ObjType].Graphic);
  endif
  
  var TilesCfg := ReadConfigFile("::tiles");
  if(TilesCfg[ObjType])
    return TilesCfg[ObjType];
  endif

  return error{"errortext" := "TileConfig "+ObjType+" nie istnieje!"};
   
endfunction

//-------------------------------------------------------------------------------


function IsStackableObj(ObjType)

  var TileConfig := GetTileConfig(ObjType);
  if(TileConfig)
    if(CInt(TileConfig.UoFlags) & TILEDATA_FLAG_STACKABLE)
      return 1;
    endif
  endif
  
  return 0;
  
endfunction

//-------------------------------------------------------------------------------

function MoveItemFromContToCont(sCont,dCont)

  var Res, rRes;

  foreach Item in ListRootItemsInContainer(sCont)
    Res := MoveItemToContainer(Item,dCont);
    if(rRes)
      rRes := Res;
    endif
  endforeach

  return rRes;

endfunction

//-------------------------------------------------------------------------------

function GetTopContMob(Item)

  if(Item.container.isa(POLCLASS_MOBILE))
    return Item.container;
  endif

  // topCont nie zwraca siebie, jesli jest sam topContem. Tak zrobilem i takie mialo byc kiedys zalozenie.. jesli zacznie sam siebie zwracac, to sie cos zapetli rekursive, nie mozna tego ruszac.
  // dlatego check wyzej jest

  var tCont := GetTopCont(Item);

  if(tCont.container.isa(POLCLASS_MOBILE))
    return tCont.container;
  endif

  return 0; 

endfunction

//-------------------------------------------------------------------------------

function GetTopContOwner(Item)

  var tCont  := GetTopCont(Item);
  var tOwner := tCont.container;

  if(!tCont or !tOwner)
    return 0;
  endif

  if(!IsPlayer(tOwner))
    if(tOwner.isa(POLCLASS_NPC))
      tOwner := GetMaster(tOwner);
    endif
  endif

  return tOwner;

endfunction

//-------------------------------------------------------------------------------

const CB_ALL                 := 0x03;
const CB_SMALL               := 0x01;
const CB_BIG                 := 0x02;
const CB_SMEAR               := 0x04;

const COLOR_POISON_BLOOD     := 64;
const COLOR_DRAGON_BLOOD     := 79;

//-------------------------------------------------------------------------------

function DropBlood(Mob)

  if(!Mob.isa(POLCLASS_MOBILE))
    return 0;
  endif

  var ObjTypes := GetBloodObjTypes(CB_ALL);

  // aby nie dublowac krwi.
  foreach Item in ListItemsAtLocation(Mob.x, Mob.y, Mob.z, Mob.realm)
    if(Item.objtype in ObjTypes)
      return 0;
    endif
  endforeach

  var Color := 0;

  if(Mob.isa(POLCLASS_NPC))
    var Elementals := GetNpcElementals(Mob);

    if(NPCELEM_POISON in Elementals)
      Color := COLOR_POISON_BLOOD;
    elseif(NPCELEM_DRAGON in Elementals)
      Color := COLOR_DRAGON_BLOOD;
    elseif(NPCELEM_ELEMENTAL in Elementals)
      return 0;
    elseif(NPCELEM_UNDEAD in Elementals)
      return 0;
    endif

  endif

  if(Mob.poisoned)
    Color := COLOR_POISON_BLOOD;
  endif

  var Item := CreateBlood(Mob.x, Mob.y, Mob.z, Mob.realm);

  if(Item)
    Item.color := Color;
  endif

  return Item;

endfunction

//-------------------------------------------------------------------------------

function GetBloodObjTypes(Flags)

  var ObjTypes := array;
  
  if(Flags & CB_SMALL)
    ObjTypes.append(UOBJ_BLOOD_SMALL1);
    ObjTypes.append(UOBJ_BLOOD_SMALL2);
    ObjTypes.append(UOBJ_BLOOD_SMALL3);
  endif

  if(Flags & CB_BIG)
    ObjTypes.append(UOBJ_BLOOD_BIG1);
    ObjTypes.append(UOBJ_BLOOD_BIG2);
  endif

  if(Flags & CB_SMEAR)
    ObjTypes.append(UOBJ_BLOOD_SMEAR);
  endif

  return ObjTypes;

endfunction

//-------------------------------------------------------------------------------

function CreateBlood(x, y, z, Realm, Flags := CB_ALL)

  var ObjTypes := GetBloodObjTypes(Flags);

  if(!ObjTypes.size())
    return 0;
  endif

  var ObjType := ObjTypes[RandomInt(ObjTypes.size())+1];
  if(!ObjType)
    ObjType := ObjTypes[1];
  endif

  return CreateItemAtLoc(x, y, z, ObjType, 1, Realm);

endfunction

//--------------------------------------------------------------------------------

function CanStack(sItem, dItem)

  if(sItem.graphic != dItem.graphic)
    return 0;
  endif

  if(sItem.objtype != dItem.objtype)
    return 0;
  endif

  if(sItem.color != dItem.color)
    return 0;
  endif


  var sProps := GetObjPropertyNames(sItem);
  var dProps := GetObjPropertyNames(dItem);

  if(sProps.size() != dProps.size())
    return 0;
  endif

  foreach PropName in sProps
    if(GetObjProperty(sItem, PropName) != GetObjProperty(dItem, PropName))
      return 0;
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanBeCustomItemName(Name)

  Name := lower(Name);
  var iCfg := ReadConfigFile(ITEMS_CFG);
  var i, aName;

  for(i:=UOBJ_ART_START;i<=UOBJ_ART_END;i:=i+1)
    aName := lower(iCfg[i].Desc);
    if(aName[Name] or Name[aName])
      return 0;
    endif
  endfor

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetGraphicForItem(objType)

  if(!CInt(objType))
    objType := GetObjTypeByName(objType);
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[objType].Graphic)
    return ItemCfg[objType].Graphic;
  endif

  return objType;

endfunction


//-----------------------------------------------------------------------------

function GetColorForItem(objType)

  if(!CInt(objType))
    objType := GetObjTypeByName(objType);
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[objType].Color)
    return ItemCfg[objType].Color;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetItemWeight(objType)

  if(ObjType.isa(POLCLASS_ITEM))
    ObjType := ObjType.objtype;
  endif

  ObjType := CInt(ObjType);
  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Weight := CInt(ItemCfg[ObjType].weight);

  if(!Weight)
    if(ItemCfg[ObjType].Graphic)
      ObjType := ItemCfg[ObjType].Graphic;
    endif
    var TilesCfg := ReadConfigFile(TILES_CFG);
    Weight := Max(CInt(TilesCfg[ObjType].weight), 1);
  endif

  return Weight;

endfunction 

//-----------------------------------------------------------------------------

function GetItemObjType(Obj)

  if(!CInt(Obj) and Obj)
    return GetObjTypeByName(Obj);
  endif

  return CInt(Obj);

endfunction

//-----------------------------------------------------------------------------

function SetItemBound(Item, who)

  SetObjProperty(Item, PROP_BOUND, who.serial);

endfunction

//-----------------------------------------------------------------------------

function EraseItemBound(Item)

  EraseObjProperty(Item, PROP_BOUND);

endfunction

//-----------------------------------------------------------------------------

function GetItemBound(Item)

  return GetObjProperty(Item, PROP_BOUND);

endfunction


//-----------------------------------------------------------------------------

function CheckBound(who, Item)

  var Bound := CInt(GetObjProperty(Item, PROP_BOUND));
  
  if(Bound and Bound != who.serial and !IsGM(who))
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetRandomItemFromGroups(PropName)

  var GroupCfg  := ReadConfigFile(ITEMGROUPS_CFG);
  var Keys     := GetConfigStringKeys(GroupCfg);
  var Names := array;

  foreach Key in Keys
    
    if(GetConfigInt(GroupCfg[Key],PropName))
      Names.append(Key);
    endif

  endforeach

  var GroupName := Names[RandomInt(Names.size())+1];

  var Elem      := GroupCfg[GroupName];


  if(!Elem)
    return 0;
  endif

  var Items     := GetConfigStringArray(Elem, "Item");
  var Idx       := RandomInt(Items.size())+1;
  var Item      := Items[Idx];

  if(Item)
    Item := SplitWords(Item);
    return GetObjTypeByName(Item[2]);
  endif

  return 0;
endfunction

//--------------------------------------------------------------------------------

function IsEquipped(who,item)

  if(item in ListEquippedItems(who))
    return 1;
  endif
  
  return 0;
  
endfunction

//-------------------------------------------------------------------------
/*
//params {player,status}
function SetItemPvPStatus(item,params)

  SetObjProperty(item, "PvPItem", {params[1].serial,params[2]});

endfunction

//-------------------------------------------------------------------------

function EraseItemPvPStatus(item)

  EraseObjProperty(item, "PvPItem");

endfunction
*/
//-------------------------------------------------------------------------

//return {player,status}
/*
function GetItemPvPStatus(item)

  var ret := GetObjProperty(item, "PvPItem");

  if(ret)
    ret[1] := FindPlayerBySerial(ret[1]);
  endif

  return ret;

endfunction
*/

//-------------------------------------------------------------------------

function GetItemCondition(item);

  var ItmHp    := GetItemHp(item);
  var ItmMaxHp := GetItemMaxHp(item);
  var DmgPerc  := 100;

  if(!ItmHp)
    ItmHp := 1;
  endif
  
  if(!ItmMaxHp)
    ItmMaxHp := 1;
  endif

  DmgPerc := CInt(CInt((ItmHp * 100) / ItmMaxHp) / 10);
  
  case(DmgPerc)
    0:
    1:       return "calkowicie uszkodzony";
    2:       return "kruchy i nie warty zaufania";
    3:       return "mocno uszkodzony";
    4:       return "uszkodzony";
    5:       return "zuzyty";
    6:       return "w slabej kondycji";
    7:       return "w dosc dobrej kondycji";
    8:       return "ledwo zuzyty";
    9:       return "prawie nowy";
    10:      return "nowy";
  endcase

endfunction

//-----------------------------------------------------------------------------