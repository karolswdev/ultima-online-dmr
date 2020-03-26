//-----------------------------------------------------------------------------

use storage;

//-----------------------------------------------------------------------------

include "include/objtypes/misc";
include "include/const/misc";
include "include/misc";
include "include/names";
include "include/logerror";

//--------------------------------------------------------------------------------

const ESCROW_PAY_AMOUNT                := 5000;

//================================================================================
//--------------------------------------------------------------------------------
// Escrow
//--------------------------------------------------------------------------------
//================================================================================

function FindEscrowBoxes( character )

  if(!character or !character.isa(POLCLASS_MOBILE) or !character.serial)
    return 0;
  endif

  var worldEscr     := OpenWorldEscrow();
  var bank_obj_name := ESCROWBOX_PREFIX + character.serial;
  var EscrBox       := FindRootItemInStorageArea( worldEscr, bank_obj_name );

  if(!EscrBox)
    return error;
  endif

  var EscrBoxes := array;
  var Num := 1;

  while(EscrBox)
    EscrBoxes.append(EscrBox);

    bank_obj_name := ESCROWBOX_PREFIX + character.serial + " - " + Num;
    EscrBox := FindRootItemInStorageArea( worldEscr, bank_obj_name );
    Num += 1;
  endwhile

  // sprawdzmy, czy nasz box nie jest przypadkiem do usuniecia.

  return EscrBoxes;

endfunction

//--------------------------------------------------------------------------------

function __CreateEscrowBox(character)

  var EscrBoxes := FindEscrowBoxes(character);
  var BoxName;

  if(EscrBoxes)
    var i;
    for(i:=1; i<=EscrBoxes.size(); i+=1)
      var EscrBox := EscrBoxes[i];

      if(CheckEscrowBoxEmptiness(EscrBox) == 1)
        BoxName := GetName(EscrBox);
        WipeEscrowBox(EscrBox);
        break;
      endif
    endfor

    if(!BoxName)
      BoxName := ESCROWBOX_PREFIX + character.serial + " - " + EscrBoxes.size();
    endif
  else
    BoxName := ESCROWBOX_PREFIX + character.serial;
  endif

  var worldEscr := OpenWorldEscrow();
  var EscrBox := CreateRootItemInStorageArea( worldEscr, BoxName, UOBJ_ESCROWBOX );
  
  if(!EscrBox)
    LogError("[CreateEscrowBox] "+GetRealName(character)+"/"+character.serial+" Nie udalo sie stworzyc skrzyni powierniczej ("+BoxName+"): "+EscrBox.errortext, LOGF_SCRIPTS);
  else
    LogNotice("[CreateEscrowBox] "+GetRealName(character)+"/"+character.serial+" Tworzenie nowej skrzynki powierniczej ("+BoxName+").", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
  endif
  
  return EscrBox;
  
endfunction

//--------------------------------------------------------------------------------

function GetEscrowData(character)

  var Data := struct;
  Data.+Boxes := GetEscrowBoxes(character);

  if(!Data.Boxes)
    return error;
  endif

  Data.+curBox := Data.Boxes[1];
  Data.+curBoxIndex := 1;
  Data.+character := character;

  return Data;

endfunction

//--------------------------------------------------------------------------------

function GetEscrowBoxes(character)

  var EscrBoxes := FindEscrowBoxes(character);

  if(!EscrBoxes)
    var EscrBox := __CreateEscrowBox(character);
    if(!EscrBox)
      return 0;
    endif
    EscrBoxes := array(EscrBox);
  else
    var i;
    for(i:=1; i<=EscrBoxes.size(); i+=1)
      var EscrBox := EscrBoxes[i];

      if(CheckEscrowBoxEmptiness(EscrBox) == 1 and EscrBoxes.size() > 1)
        WipeEscrowBox(EscrBox, 1); // wyczysc sam content
        // trzeba zrobic porzadek, by syfu nie bylo.
        if(i < EscrBoxes.size())
          var LastBox := EscrBoxes[EscrBoxes.size()];
          if(MoveContainerContent(LastBox, EscrBox))
            WipeEscrowBox(LastBox);
          else
            continue;
          endif
          i -= 1; // weryfikujemy czy oby na pewno cos bylo w ostatniej...
        else
          WipeEscrowBox(EscrBox);
        endif
        EscrBoxes.erase(EscrBoxes.size());
      endif
    endfor
  endif
  
  return EscrBoxes;

endfunction 

//--------------------------------------------------------------------------------

function ShowEscrowBoxes(who, ofWho)

  var Boxes := FindEscrowBoxes(ofWho);

  if(Boxes)
    foreach Box in Boxes
      // specjalnie aby kazdy box mial inny pid
      start_script(":housing:show_escrow", array(who, Box));
    endforeach

    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function CheckEscrowData(byref Data, LastItemWeight)

  while(Data.curBox.weight + LastItemWeight >= 65535)
    
    if(Data.curBoxIndex >= Data.Boxes.size())
       // no to siup, nowy box !!
      Data.curBox := __CreateEscrowBox(Data.character);
      SetObjProperty(Data.curBox, "#FillingEscrow", ReadGameClock());
      Data.Boxes.append(Data.curBox);
      Data.curBoxIndex := Data.Boxes.size();
      return 1;
    endif

    Data.curBoxIndex += 1;
    Data.curBox := Data.Boxes[Data.curBoxIndex];
  endwhile

  return 0;

endfunction

//--------------------------------------------------------------------------------

function MoveItemToPlayerEscrow(who, Item)

  if(!IsPlayer(who) or !Item.isa(POLCLASS_ITEM) or !Item.movable)
    return 0;
  endif

  var EscrData := GetEscrowData(who);

  if(!EscrData)
    return 0;
  endif

  // tutaj ladujemy do specjalnych plecakow itemy - jak nie da rady to tworzymy nowy i tak dalej
  var curBackpack := GetEscrowBackpack(EscrData);

  if(!curBackpack)
    return 0;
  endif

  if(!MoveItemToContainer(Item, curBackpack))
    CheckEscrowData(EscrData, Item.weight);
    curBackpack := GetEscrowBackpack(EscrData);

    if(!MoveItemToContainer(Item, curBackpack))
      return 0;
    endif
  endif

  return 1;

endfunction

//--------------------------------------------------------------------------------

function GetEscrowBackpack(byref EscrData)

  var curPack := FindItemBySerial(GetObjProperty(EscrData.curBox, "curBackpack"));

  if(!curPack or curPack.item_count >= DEF_CONTAINER_MAX_ITEMS)
    curPack := 0;
    foreach Item in ListRootItemsInContainer(EscrData.curBox)
      if(!curPack and Item.objtype == UOBJ_ESCROWPACK and Item.item_count < (DEF_CONTAINER_MAX_ITEMS - 1))
        curPack := Item;
      elseif(Item.item_count == 1)
        DestroyItem(Item);
      endif
    endforeach

    if(curPack)
      SetObjProperty(EscrData.curBox, "curBackpack", curPack.serial);
      return curPack;
    endif

    if(EscrData.curBox.item_count == (DEF_CONTAINER_MAX_ITEMS - 1))

      curPack := CreateItemInCont(EscrData.curBox, UOBJ_ESCROWPACK);

      if(!curPack)
        if(!CheckEscrowData(EscrData, 1))
          LogError("[GetEscrowBackpack] Nie udalo sie stworzyc nowego plecaka EscrowPack: ["+curPack.errortext+"] dla skrzynki: "+EscrData.curBox.serial+"/"+GetName(EscrData.curBox), LOGF_SCRIPTS);
          return curPack;
        endif

        curPack := CreateItemInCont(EscrData.curBox, UOBJ_ESCROWPACK);

        if(!curPack)
          LogError("[GetEscrowBackpack] Nie udalo sie stworzyc nowego plecaka EscrowPack: ["+curPack.errortext+"] dla skrzynki: "+EscrData.curBox.serial+"/"+GetName(EscrData.curBox), LOGF_SCRIPTS);
          return curPack;
        endif
      endif

      foreach item in EnumerateItemsInContainer(EscrData.curBox, ENUMERATE_ROOT_ONLY)

        if(curPack != item)
          var res := MoveItemToContainer(item, curPack);
          if(!res)
            LogError("[GetEscrowBackpack] Nie udalo sie uporzadkowac plecakow wkladajac do plecaka: "+curPack+" dla skrzynki: "+EscrData.curBox.serial+"/"+GetName(EscrData.curBox) + " MoveItem: " + res.errortext, LOGF_SCRIPTS);
            return curPack;
          endif
        endif

      endforeach

    endif
    
    curPack := CreateItemInCont(EscrData.curBox, UOBJ_ESCROWPACK);
    
    if(!curPack)
      LogError("[GetEscrowBackpack] Nie udalo sie stworzyc nowego plecaka EscrowPack: "+curPack+" dla skrzynki: "+EscrData.curBox.serial+"/"+GetName(EscrData.curBox), LOGF_SCRIPTS);
      return curPack;
    endif
  endif

  SetObjProperty(EscrData.curBox, "curBackpack", curPack.serial);

  return curPack;

endfunction

//--------------------------------------------------------------------------------

function CheckEscrowBoxEmptiness( EscrBox )

  if(!EscrBox)
    return -1;
  endif

  var leftCnt := 0;

  foreach Item in ListRootItemsInContainer(EscrBox)

    if(Item.objtype == UOBJ_ESCROWPACK)
      
      // czy jest pusty masz escrow pack?
      if(Item.item_count == 1)
        // jak tak, to destroy!
        DestroyItem(Item);
      else
        leftCnt+=1;
      endif

    else

      leftCnt+=1;

    endif

  endforeach

  if(leftCnt == 0)
    return 1;
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function WipeEscrowBoxes(who)

  var Boxes := FindEscrowBoxes(who);

  LogNotice("[WipeEscrowBoxes] Czyszczenie skrzynek powierniczych "+hex(who.serial)+"/"+GetName(who)+".", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);

  foreach EscrBox in Boxes
    WipeEscrowBox(EscrBox);
  endforeach

endfunction

//--------------------------------------------------------------------------------

function WipeEscrowBox( EscrBox, bContent := 0 )

  if(bContent)
    foreach Item in ListRootItemsInContainer(EscrBox)
      if(!DestroyItem(Item))
        return 0;
      endif
    endforeach

    return 1;
  else

    var BoxName := GetName(EscrBox);
    var worldEscr     := OpenWorldEscrow();

    if(DestroyRootItemInStorageArea( worldEscr, BoxName))
      LogNotice("[WipeEscrowBox] "+BoxName+" Czyszczenie skrzynki powierniczej.", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function OpenWorldEscrow()

  var bank := FindStorageArea( ESCROW_STORAGE );

  if (!bank)
    bank := CreateStorageArea( ESCROW_STORAGE );
  endif

  if (!bank)
    LogError("[OpenWorldEscrow] Nie udalo sie stworzyc globalnego banku powierniczego!", LOGF_SCRIPTS);
  endif

  return bank;

endfunction

//-----------------------------------------------------------------------------

function SetEscrowFilling(byref Data, bState)

  if(bState)
    var Boxes := array;
    var i := Data.Boxes.size();

    while(i > 1)
      if(CheckEscrowBoxEmptiness(Data.Boxes[i]) == 1)
        WipeEscrowBox(Data.Boxes[i]);
      else
        break;
      endif

      i -= 1;
    endwhile

    while(i > 0)
      Boxes.append(Data.Boxes[i]);
      i -= 1;
    endwhile

    Data.Boxes := Boxes;

    if(!Data.curBox)
      Data.curBox := Data.Boxes[1];
      Data.curBoxIndex := 1;
    endif

    SetEscrowPayed(Data.Boxes, 0); 
  endif

  foreach EscrBox in (Data.Boxes)
    if(bState)
      // jesli automat cos doklada, to czyscimy zaplate - niech zaplaci jeszcze raz. 
      // przypadek: gdy drugi dom jest niszczony, a pierwszy nie zostal odzyskany (EscrBox istnieje nadal)

      SetObjProperty(EscrBox, "#FillingEscrow", ReadGameClock());
    else
      EraseObjProperty(EscrBox, "#FillingEscrow");
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function IsFillingEscrow(EscrBox)

  return GetObjProperty(EscrBox, "#FillingEscrow") != error;

endfunction

//-----------------------------------------------------------------------------

function GetNotPayedEscrow(EscrBoxes)

  var Num := 0;

  foreach EscrBox in EscrBoxes
    if(GetObjProperty(EscrBox, "Payed") == error)
      Num += 1;
    endif
  endforeach

  return Num;

endfunction

//-----------------------------------------------------------------------------

function SetEscrowPayed(EscrBoxes, bState := 1)

  foreach EscrBox in EscrBoxes
    if(bState)
      SetObjProperty(EscrBox, "Payed", ReadGameClock());
    else
      EraseObjProperty(EscrBox, "Payed");
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
