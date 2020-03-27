//-----------------------------------------------------------------------------

include "include/checks/stdchecks";
include "include/objtypes/misc";
include "include/items/containers";

//=============================================================================
//-----------------------------------------------------------------------------
// Equipment
//-----------------------------------------------------------------------------
// wymusza zalozenie przedmiotu na dana postac
// zdejmuje blokujace zalozone przedmioty do plecaka osobnika
//=============================================================================

function ForceEquipItem(who,item)

  if(!StandardItemCheck(who,item,NO_COMBAT_CHECK))
    return 0;
  endif

  if(item.container.serial == who.serial)
    SendSysMessage (who, "Juz zalozyl"+Ggm(who,3)+" ten przedmiot.",FONT_NORMAL,COLOR_GREEN);
    return 0;
  endif

  if(who.backpack != GetTopCont(item))
    SendSysMessage (who, "Przedmiot musi byc w twoim plecaku!",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  SetObjProperty(who, "#LastEquip", ReadGameClock());

  if(EquipItem (who, item))
    sleep (1);
    return 1;
  endif

  var ItemDescCfg := ReadConfigFile(ITEMS_CFG);
  var Items := array;

  if(Item.isa(POLCLASS_WEAPON) or ItemDescCfg[item.objtype].Shield)

    var Itm1 := GetEquipmentByLayer(who,LAYER_HAND1);
    var Itm2 := GetEquipmentByLayer(who,LAYER_HAND2);
    if(item.isa(POLCLASS_WEAPON))
      if(ItemDescCfg[item.objtype].TwoHanded)
        if(Itm2)
          MoveItemToContainer(Itm2,who.backpack);
          Items.append(Itm2);
        endif
      endif
      if(Itm1)
        MoveItemToContainer(Itm1,who.backpack);
        Items.append(Itm1);
      elseif(Itm2)
        MoveItemToContainer(Itm2,who.backpack);
        Items.append(Itm2);
      endif
    elseif(ItemDescCfg[item.objtype].Shield)
      if(Itm2)
        MoveItemToContainer(Itm2,who.backpack);
        Items.append(Itm2);
      elseif(Itm1)
        MoveItemToContainer(Itm1,who.backpack);
        Items.append(Itm1);
      endif
    endif

  else

    if(!ItemDescCfg[item.objtype])
      LogError("ForceEquipItem: brak Itemdesc Cfg: "+item.objtype+": "+ItemDescCfg[item.objtype]);
      return 0;
    endif

    var Cov   := GetConfigStringArray(ItemDescCfg[item.objtype],"Coverage");
    var ICov;

    foreach C in Cov
      C := Lower(C);
    endforeach

    foreach itm in ListEquippedItems(who)
      if(Itm)
        if(Itm.objtype != UOBJ_MOUNT)
          if(itm.graphic == item.graphic)
            Items.append(itm);
          else
            if(!ItemDescCfg[itm.objtype])
              LogError("ForceEquipItem: brak Itemdesc Cfg: "+itm.objtype+": "+ItemDescCfg[itm.objtype]);
            else
              ICov := GetConfigStringArray(ItemDescCfg[itm.objtype],"Coverage");
              foreach C in ICov
                if(lower(C) in Cov)
                  Items.append(itm);
                  break;
                endif
              endforeach
            endif
          endif
        endif
      endif
    endforeach

    foreach Itm in Items
      MoveItemToContainer(Itm,who.backpack);
    endforeach

  endif

  if(Items.size())
    if(EquipItem(who,item))
      sleep(1);
      return 1;
    endif
    SendSysMessage(who,"Nie mozesz tego zalozyc.",FONT_NORMAL,COLOR_RED);
  endif

  foreach Itm in Items
    EquipItem(who,Itm);
  endforeach

  sleep(1);
  return 0;

endfunction

//-----------------------------------------------------------------------------

function WasEquipingItem(who, secTime)

  if(CInt(GetObjProperty(who, "#LastEquip")) + secTime > ReadGameClock())
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
// zdejmuje wszystkie przedmioty do Cont, domyslnie plecak
//-----------------------------------------------------------------------------

function UnEquipAll(who,Except := array,Cont := 0)

  if(!Cont)
    Cont := who.backpack;
  endif

  var nr := 0,i,item;
  for(i:=1;i<=24;i:=i+1)
    if(!(i in {LAYER_PACK,LAYER_HAIR,LAYER_BEARD}))
      item := GetEquipmentByLayer(who,i);
      if(!(item.objtype in Except) and Item)
        if(!MoveItemToContainer(item,Cont))
          MoveObject(item, who.x, who.y, who.z, who.realm, MOVEITEM_FORCELOCATION);
        endif
        nr := nr + 1;
      endif
    endif
  endfor

  return nr;

endfunction

//-----------------------------------------------------------------------------

function GetShield(who)

  var Shield := GetEquipmentByLayer(who, LAYER_HAND2);

  if(!Shield or !Shield.isa(POLCLASS_ARMOR))
    Shield := GetEquipmentByLayer(who, LAYER_HAND1);
  endif

  if(shield.isa(POLCLASS_ARMOR))
    return Shield;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
