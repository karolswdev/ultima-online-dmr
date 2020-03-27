//-----------------------------------------------------------------------------

include "include/objtypes/misc";
include "include/objtypes/magic";
include "include/client/fonts";
include "include/items";
include "include/log";
include "include/bits";
include "include/escrow";
include "include/tempmods";
include "include/checks/stdchecks";
include "include/finding";
include "include/items/basic";
include "ai/include/tamed";

include ":itemsets:ctrl_check";
include ":eventcore:common";
include ":eventcore:item_cont";
include ":eventcore:proc_events";
include ":renthouse:common";

//-----------------------------------------------------------------------------

function StdCanContainerInsert(who, Cont, item)

  if(Cont.locked)
    SendSysMessage(who,"Ten pojemnik jest zamkniety!", FONT_NORMAL, COLOR_RED);
    return 0;
  endif

  if(!CanUseItem(who, Cont))
    return 0;
  elseif(!CanUseItem(who, Item))
    return 0;
  endif

  if(IsPlayer(who))
    
    if(who.dead and !Cont.isa(POLCLASS_CORPSE))
      return;
    endif

    if(!who.cmdlevel)
      if(Item.objtype == UOBJ_BANKBOX_REDIR or Item.objtype == UOBJ_BANKBOX or Item.objtype == UOBJ_STOREBOX or Item.objtype == UOBJ_STORE_MAILBOX)
        SendSysMessage(who, "Nie mozesz uzyc tego przedmiotu!", FONT_NORMAL, COLOR_ORANGE);
        LogMsg(FBITEMS_LOG,who,item);
        return 0;
      endif
    endif

    if(IsSetFlag(Cont, ITEMF_NO_INSERT) and !who.cmdlevel)
      SendSysMessage(who, "Nie mozesz tego tutaj wlozyc.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

    var ownerserial := CInt(GetObjProperty(Cont, PROP_OWNER));

    if(ownerserial)
      if((who.serial != ownerserial) and !IsGM(who))
        SendSysMessage(who, "Nie mozesz tego tutaj wlozyc.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif
    endif
    
    if(GetObjProperty(Item, PROP_TRITEM_TEMP))
      if(Cont != who.backpack and Cont.objtype != UOBJ_TR_BAG)
        SendSysMessage(who, "Nie mozesz tego tutaj wlozyc.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif    
    endif

  endif

  // sprawdzenie na magazyniora

  var pCont := GetTopCont(Cont);

  if(IsAutoEventItem(Item))
    if(pCont.objtype == UOBJ_STOREBOX or pCont.objtype == UOBJ_BANKBOX or pCont.objtype == UOBJ_ESCROWBOX or pCont.objtype == UOBJ_ESCROWPACK or pCont.objtype == UOBJ_MP_BOX)
      SendBad(who, "Nie mozesz schowac przedmiotu z wydarzenia. Musisz go zwrocic na zakonczenie!");
      return 0;
    endif
  endif

  if(pCont.objtype == UOBJ_STOREBOX or Cont.objtype == UOBJ_STOREBOX)
    var ItemCfg := ReadConfigFile(ITEMS_CFG);
    if(!ItemCfg[item.objtype].StoreAble)
      SendSysMessage(who, "Nie mozesz tego tutaj wlozyc.",FONT_NORMAL,COLOR_RED);
      return 0;
    elseif(Item.isa(POLCLASS_CONTAINER))
      foreach iItem in EnumerateItemsInContainer(Item)
        if(!ItemCfg[iItem.objtype].StoreAble)
          SendSysMessage(who, "Nie mozesz tego tutaj wlozyc.",FONT_NORMAL,COLOR_RED);
          return 0;
        endif
      endforeach
    endif

  elseif(pCont.objtype == UOBJ_ESCROWBOX or Cont.objtype == UOBJ_ESCROWBOX)

    if(!IsFillingEscrow(pCont) and !IsFillingEscrow(Cont))
      if(Item.objtype != UOBJ_ESCROWPACK)
        SendSysMessage(who, "Nie mozesz wkladac przedmiotow do skrzynki powierniczej.",FONT_NORMAL,COLOR_RED);
        return 0;
      endif
    endif

  elseif(Item.objtype == UOBJ_ESCROWPACK or Cont.objtype == UOBJ_ESCROWPACK or pCont.objtype == UOBJ_ESCROWPACK)

    SendSysMessage(who, "Ten przedmiot jest dostepny tylko w skrzynce powierniczej.",FONT_NORMAL,COLOR_RED);
    return 0;

  elseif(pCont.objtype == UOBJ_MP_BOX or Cont.objtype == UOBJ_MP_BOX)
    var Container := __ifelse(pCont.objtype == UOBJ_MP_BOX, pCont, Cont);
    if( CInt(GetObjProperty(Container, PROP_STALL_OWNER)) != who.serial and !IsAdmin(who))
      SendSysMessage(who, "Nie wlozysz tutaj zadnego przedmiotu.", FONT_NORMAL, COLOR_RED);
      return 0;
    endif
  endif
 
  return 1;

endfunction

//-------------------------------------------------------------------------------

function GetTopCont(Item)

  var Cont := Item.container;

  if(!Cont.isa(POLCLASS_CONTAINER))
    return 0;
  endif

  while(Cont.container.isa(POLCLASS_CONTAINER))
    Cont := Cont.container;
  endwhile

  return Cont;

endfunction

//-----------------------------------------------------------------------------

function StdCanContainerRemove(who, Cont, item)

  __ParamUnused(Cont);
/*
  if(IsPlayer(who))
 __NONPVP__ out!
    var pCont := GetTopCont(item);
    if (pCont.isa(POLCLASS_CORPSE) or ( pCont.objtype >= UOBJ_BONES_START and pCont.objtype <= UOBJ_BONES_END))
      var ItemPvPStatus := GetItemPvPStatus(item);
      if(ItemPvPStatus)
        var who_status  := GetPvPStatus(who);
        if(ItemPvPStatus[1] == error or ItemPvPStatus[2] == error or who_status == error)
          return 1;
        elseif(ItemPvPStatus[1].acctname == who.acctname)
          return 1;
        elseif(ItemPvPStatus[2] == PVP_FORCED_YES)
          return 1;
        elseif(ItemPvPStatus[2] == PVP_NO and who_status == PVP_NO and IsFriend(ItemPvPStatus[1],who, NO_PARTY_CHECK))
          return 1;
        elseif(IsInSameParty(ItemPvPStatus[1], who) and ItemPvPStatus[1].partycanloot)
          return 1;
        elseif(ItemPvPStatus[2] == PVP_YES and who_status == PVP_NO)
          return 0;
        elseif(ItemPvPStatus[2] == PVP_NO)
          return 0;
        endif
      endif
    endif
  endif
*/
  return 1;

endfunction

//-----------------------------------------------------------------------------

function StdOnContainerRemove(who, Cont, item)

/*  if(IsPlayer(who))
    EraseItemPvPStatus(item);
  endif*/
  CheckAutoEventItemRemove(who, Cont, item);

endfunction

//-----------------------------------------------------------------------------

function StdOnContainerInsert(who, Cont, item, existing_stack)

/*  if(IsPlayer(who))
    EraseItemPvPStatus(item);
  endif*/
  CheckAutoEventItemInsert(who, Cont, item);

  CheckItemSetControl(Item);

  if(item != existing_stack and existing_stack)
    CheckItemSetControl(existing_stack);
  endif

endfunction

//-----------------------------------------------------------------------------

function MoveContainerContent(srcCont, dstCont)

  if(!srcCont.isa(POLCLASS_CONTAINER) or !dstCont.isa(POLCLASS_CONTAINER))
    return RetError("To nie jest pojemnik!");
  endif

  foreach Item in ListRootItemsInContainer(srcCont)
    var Res := MoveItemToContainer(Item, dstCont);
    if(!Res)
      return Res;
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function DropContainerContent(Cont, bForce := 0)

  if(!Cont.isa(POLCLASS_CONTAINER))
    return RetError("To nie jest pojemnik!");
  endif

  var TopCont := GetTopCont(Cont);

  if(!TopCont)
    TopCont := Cont;
  endif

  foreach Item in ListRootItemsInContainer(Cont)
    var Res := MoveObject(Item, Cont.x, Cont.y, Cont.z, Cont.realm, MOVEITEM_FORCELOCATION);
    if(!Res)
      if(bForce)
        DestroyItem(Res);
      else
        return Res;
      endif
    endif
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CanAccessContainer(who, byref pCont)

  if(!IsPlayer(who))
    return 0;
  endif

  var topMob := GetTopContMob(pCont);

  if(pCont.isa(POLCLASS_MOBILE))
    topMob := pCont;
    pCont := pCont.backpack;
  endif

  if(topMob.isa(POLCLASS_NPC))
    if(IsHuman(topMob))
      return RetError("Ta osoba trzyma przy sobie swoj plecak.");
    endif

    if(!IsTamed(topMob))
      return RetError("To stworzenie nie jest oswojone.");
    endif

    if(GetMaster(topMob) != who)
      return RetError("To nie Twoje stworzenie.");
    endif

    if(!TamedNpcHavePack(topMob))
      return RetError("To stworzenie nie posiada plecaka.");
    endif

    return 1;
  endif

  if(!pCont.isa(POLCLASS_CONTAINER))
    return RetError("To nie pojemnik.");
  endif

  if(pCont.locked)
    return RetError("Ten pojemnik jest zamkniety.");
  endif

  if(pCont.isa(POLCLASS_SPELLBOOK) or pCont.objtype == UOBJ_RUNEBOOK)
    return RetError("Nie mozesz tego uzyc.");
  endif

  var topCont := GetTopCont(pCont);

  if(!topCont)
    topCont := pCont;
  endif

  if(topCont.objtype == UOBJ_SECURED_CONT or GetObjProperty(topCont, "secure"))
    if(!CanAccessSecureCont(who, topCont))
      return RetError("Nie mozesz tego uzyc.");
    else
      return 1;
    endif
  elseif(topCont == who.backpack or pCont.objtype == UOBJ_ARENAPACK or topCont.objtype == UOBJ_TRASHBARREL)
    return 1;
  endif

  var Bank := FindBankBox(who);
  var StoreBox := FindStoreBox(who);
  var EscrBoxes := FindEscrowBoxes(who);

  //  Jest w plecaku,             //jest w banku     //jest w skrzyni pow.  //jest bezpiecznym pojemnikiem        //jest plecakiem osoby    //jest skrzynia w domku do wynajecia.

  if(topCont == Bank or topCont == StoreBox or (topCont in EscrBoxes))
    return 1;
  endif

  if(!pCont.movable)
    if(!CanAccess(who, pCont))
      return 0;
    else
      return 1;
    endif
  endif

  if(GetObjProperty(topCont, "SignSer"))
    if(!CanControlHr(who, FindItemBySerial(GetObjProperty(topCont,"SignSer")) ) )
      RetError("Nie masz dostepu do tej skrzyni.");
    endif
  endif
  
  return RetError("Nie mozesz tego uzyc.");

endfunction

//-----------------------------------------------------------------------------
