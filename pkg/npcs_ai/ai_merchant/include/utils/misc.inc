//-----------------------------------------------------------------------------

include "include/items/prices";
include "include/names";
include "include/coords";

include ":npcguilds:guildsdata";
include ":npcs_ai:const";

//-----------------------------------------------------------------------------

function GetMercBox(Template)

  if(Template.isa(POLCLASS_NPC))
    Template := Template.npctemplate;
  endif

  return GetMercStorageItem(MERC_STORAGE,Template + MERC_FSBOX);
  
endfunction

//-----------------------------------------------------------------------------

function GetMercOldBox()

  return GetMercStorageItem(MERC_STORAGE,MERC_PBBOX);

endfunction

//-----------------------------------------------------------------------------

function GetMercType(Npc)

  return GetNpcConfig(Npc).MercType;

endfunction

//-----------------------------------------------------------------------------

function GetMercStorageItem(StorName, BoxName)

  var Storage := FindStorageArea(StorName);
  if(!Storage)
    Storage := CreateStorageArea(StorName);
    if(Storage)
      LogNotice("[AI] Stworzono Storage ["+StorName+"].", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
    endif
  endif

  var Box := FindRootItemInStorageArea(Storage, BoxName);

  if(!Box)
    Box := CreateRootItemInStorageArea( Storage, BoxName, UOBJ_MERCHANT_BOX );
    if(Box)
      LogNotice("[AI] Stworzono kontener ["+BoxName+"] w Storage ["+StorName+"].", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
    endif
  endif

  return Box;

endfunction

//-----------------------------------------------------------------------------

function FindItemInMercBox(MercBox, ObjType)

  ObjType  := hex(ObjType);
  var Item := FindObjtypeInContainer(MercBox, Objtype);

  if(!Item)
    Item := CreateItemInCont(MercBox, ObjType, 1);
  endif

  return Item;

endfunction

//-----------------------------------------------------------------------------

function CanBuyItem(Item)

  if(!Item.newbie and !Item.container.newbie and Item.item_count == 1)
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function ListItemsToSell(who, City, BuyItems)

  var RepLvl := GetPlayerNpcGuildCityLvl(who, City);
  var dItems := dictionary;

  var Price, ElemName;

  foreach Item in EnumerateItemsInContainer(who.backpack)

    if(CanBuyItem(Item))

      if(CInt(BuyItems[hex(Item.objtype)]))
        
        Price    := GetItemBuyPrice(Item, RepLvl);
        ElemName := Item.objtype+"_"+CutItemDesc(Item)+"_"+Price;

        if(dItems.exists(ElemName))
          dItems[ElemName][2] := dItems[ElemName][2] + Item.amount;
          dItems[ElemName][1].append(Item);
        else
          dItems[ElemName] := array(array(Item), Item.amount, Price);
        endif

      endif

    endif

  endforeach

  return dItems;

endfunction

//-----------------------------------------------------------------------------

function MoveIfBlocking(Npc, Source)

  if((GetStandingHeight(Npc.x-1,Npc.y,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x+1,Npc.y,Npc.z,Npc.realm).z == error) or 
     (GetStandingHeight(Npc.x,Npc.y-1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x,Npc.y+1,Npc.z,Npc.realm).z == error) or
     (GetStandingHeight(Npc.x+1,Npc.y-1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x+1,Npc.y+1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x+1,Npc.y,Npc.z,Npc.realm).z != error) or
     (GetStandingHeight(Npc.x-1,Npc.y-1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x-1,Npc.y+1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x-1,Npc.y,Npc.z,Npc.realm).z != error) or
     (GetStandingHeight(Npc.x-1,Npc.y-1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x+1,Npc.y-1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x,Npc.y-1,Npc.z,Npc.realm).z != error) or
     (GetStandingHeight(Npc.x-1,Npc.y+1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x+1,Npc.y+1,Npc.z,Npc.realm).z == error and GetStandingHeight(Npc.x,Npc.y+1,Npc.z,Npc.realm).z != error) )

    WalkAwayFrom(Source);
    WalkAwayFrom(Source);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
