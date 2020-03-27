//-----------------------------------------------------------------------------

const UOBJ_LOSTITEM          := 0xB201;
const PROP_LOSTITEM          := "LostItem";

//-----------------------------------------------------------------------------

const LI_CFG                 := ":lostitems:lostitems";
const LI_PRICES_CFG          := ":lostitems:prices";
const LI_CFG_GLOBAL          := "Global";
const LI_CFG_TEXTS           := "Texts";

//-----------------------------------------------------------------------------

function CreateLostItemLoot(Cont)

  if(!Cont.isa(POLCLASS_CONTAINER))
    return 0;
  elseif(IsDungeon(Cont.x, Cont.y, Cont.realm))
    return 0;
  endif

  var LiCfg := ReadConfigFile(LI_CFG);
  var gElem := LiCfg[LI_CFG_GLOBAL];

  if(!gElem)
    LogError("CreateLostItemLoot: Brak globalnego elementu w cfg ["+LI_CFG+"]");
    return 0;
  endif

  if(!gElem.Enabled)
    return 0;
  endif

  if(CInt(gElem.Chance) < RandomInt(100))
    return 0;
  endif

  var City := FindClosestCity(Cont.x, Cont.y, Cont.realm, gElem.AcpDist);

  if(!City)
    return 0;
  endif

  var lItemNr   := GetRandomLostItem();
  var lItemElem := LiCfg[lItemNr];

  if(!lItemElem)
    LogError("CreateLostItemLoot: Nie udalo sie wybrac losowego elementu.");
    return 0;
  endif

  if(!lItemElem.ObjType)
    LogError("CreateLostItemLoot: Brak ObjType w LostLootItem ["+lItemNr+"].");
    return 0;
  endif
    
  if(CInt(lItemElem.Chance) < RandomInt(100))
    return 0;
  endif

  var Npc   := FindLostItemNpc(lItemElem, City);

  if(!Npc)
    return 0;
  endif

  var lItem    := CreateItemAtTempLoc(UOBJ_LOSTITEM);
  var ItemCfg  := ReadConfigFile(ITEMS_CFG);
  var Grap     := CInt(lItemElem.ObjType);
  if(!Grap)
    Grap := GetObjTypeByName(lItemElem.ObjType);
  endif

  if(ItemCfg[Grap].Graphic)
    Grap := CInt(ItemCfg[Grap].Graphic);
  endif

  lItem.graphic := Grap;
  lItem.color   := CInt(lItemElem.Color);
  SetName(lItem, ConvertLostItemName(lItemElem.Name, Npc));
  
  if(!MoveItemToContainer(lItem, Cont))
    DestroyItem(lItem);
    return 0;
  endif

  SetObjProperty(lItem, PROP_LOSTITEM, array(lItemNr, Npc.serial, ReadGameClock()));
  return 1;

endfunction

//-----------------------------------------------------------------------------

function CompleteLostItem(who, Npc, Item)

  var LiCfg  := ReadConfigFile(LI_CFG);
  var Texts  := GetConfigStringArray(LiCfg[LI_CFG_TEXTS], "Complete");
  var lProps := GetObjProperty(Item, PROP_LOSTITEM);
  var Price  := LiCfg[lProps[1]].Price;

  if(!Texts)
    LogError("CompleteLostItem: Nie znaleziono tekstow npeca [Complete]");
  else
    var Text   := Texts[RandomInt(Texts.size())+1];
    if(Text)
      PrintTextAbove(Npc, Text);
    endif
  endif

  DestroyItem(Item);

  if(!Price)
    LogError("CompleteLostItem: Brak nagrody w LostItem ["+lProps[1]+"]");
    return 0;
  endif

  Price := SplitWords(Price);
  if(Price.size() > 1)
    Price := Price[RandomInt(Price.size())+1];
  else
    Price := Price[1];
  endif

  CreateLoot(who.backpack, CInt(Price), LI_PRICES_CFG);
  return 1;

endfunction

//-----------------------------------------------------------------------------

function ConvertLostItemName(Name, Npc)

  var Pos := find(Npc.name," - ",1);
  var NpcName, NpcType;

  if(Pos)
    NpcName := Npc.name[1,Pos-1];
    NpcType := Npc.name[Pos+3, len(Npc.name)-(Pos+2)];
  else
    NpcName := Npc.name;
    NpcType := "";
  endif

  Name["%npcname"] := NpcName;
  Name["%npctype"] := NpcType;

  return Name;

endfunction

//-----------------------------------------------------------------------------

function FindLostItemNpc(lItemElem, cCity)

  var Coords := GetCityCoords(cCity);
  var Realm  := GetCityRealm(cCity);
  var Npcs   := array;
  var x,y,tx,ty;

  for(x:=Coords[1];x<Coords[3];x:=x+128)
    for(y:=Coords[2];y<Coords[4];y:=y+128)
      tx := x + 128;
      ty := y + 128;
      if(tx > Coords[3])
        tx := Coords[3];
      endif
      if(ty > Coords[4])
        ty := Coords[4];
      endif

      foreach Npc in ListObjectsInBox(x, y, -127, tx, ty, 127, Realm)
        if(Npc.isa(POLCLASS_NPC))
          if(lower(Npc.npctemplate) == lower(lItemElem.Template))
            if(lItemElem.Gender)
              if(CInt(lItemElem.Gender)-1 == Npc.gender)
                Npcs.append(Npc);
              endif
            else
              Npcs.append(Npc);
            endif
          endif
        endif
      endforeach
    endfor
  endfor

  if(!Npcs.size())
    return 0;
  endif

  return Npcs[RandomInt(Npcs.size())+1];

endfunction

//-----------------------------------------------------------------------------

function GetRandomLostItem()

  var LiCfg := ReadConfigFile(LI_CFG);
  var Items := GetConfigIntKeys(LiCfg);

  var Item  := Items[RandomInt(Items.size())+1];

  if(!Item)
    Item := Items[1];
  endif

  return Item;

endfunction

//-----------------------------------------------------------------------------

function FindClosestCity(x, y, Realm, Dist)

  var Cities  := GetCities();
  var cCity   := 0;
  var minDist := Dist;
  var Coords, cRealm, cDist;

  foreach City in Cities
    Coords := GetCityCoords(City);
    cRealm := GetCityRealm(City);
    if(cRealm == Realm)
      cDist := coordist(x, y, Coords[1] + (Coords[3] - Coords[1]), Coords[2] + (Coords[4] - Coords[2]));
      if(cDist <= Dist)
        if(minDist > cDist)
          minDist := cDist;
          cCity   := City;
        endif
      endif
    endif
  endforeach

  return cCity;

endfunction

//-----------------------------------------------------------------------------
