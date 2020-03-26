include "include/client/fonts";
include "include/player/target";
include "include/logerror";
include ":banking:gold";
include "include/npcs";

const MP_FILE := "stalls";
const MP_STALL_RANGE := "range";
const MP_STALL_PRICE := "price";
const MP_STALL_OWNER := "owner";
const MP_STALL_LOCKED := "locked";
const MP_STALL_RENTED := "rentedtime";
const MP_STALL_BOX := "box";
const MP_STALL_VENDOR := "vendor";
const MP_STALL_ENABLED := "enabled";
const MP_STALL_CASH := "cash";
const MP_STALL_NOTICE := "notice";
const MP_RENT_TIME := 2419200; //28 dni
const MP_TIME_TO_RERENT := 604800; //7 dni
const MP_MAX_LOCKED_ITEMS := 2;
const MP_MAX_VENDORS := 1;
const MP_MAX_BOXES := 1;
const MP_TTP_PRICE := "Cena";
const MP_TTP_STACKPRICE := "Cena za sztuke";

const MP_SIGN_NS := 0xBDB;
const MP_SIGN_WE := 0xBDC;

const EVID_MP_MSG := 0x5D5D0000;

const MP_VENDOR_TEMPLATE := ":marketplace:mpplayervendor";

const PROP_MP_GUMP_PID := "#MPGumpPid";

var DataFile;

//------------------------------------------------------------------------------------------------

function GetDataFile(fileloc)
  
    var file := OpenDataFile(fileloc);

    if(!file)
      CreateDataFile(fileloc,DF_KEYTYPE_INTEGER);
      LogNotice("[MarketPlace]: Stworzono DataFile");
    endif

    return file;

endfunction

//------------------------------------------------------------------------------------------------

function InStall(StallKey,x,y,realm)
    var Range := GetDataFile(MP_FILE).FindElement(StallKey).GetProp(MP_STALL_RANGE);
    if(!range)
      return error;
    endif
    
    if( x < Range[1] or y < Range[2] or x > Range[3] or y > Range[4] or realm != Range[5])
      return 0;
    endif

    return 1;
endfunction

//------------------------------------------------------------------------------------------------

function LockItems(box)

  foreach item in EnumerateItemsInContainer(box)
    if(item.movable)
      item.movable := 0;
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function EraseSellProps(Item)

  Item.movable := 1;
  EraseToolTipProp(Item, MP_TTP_STACKPRICE);
  EraseToolTipProp(Item, MP_TTP_PRICE);

endfunction

//------------------------------------------------------------------------------------------------

function UnLockItems(box)

  var arr := array;
  var conts := dictionary;

  foreach item in EnumerateItemsInContainer(box)
    if(GetToolTipProp(item, MP_TTP_PRICE))
      if(item.movable)
        item.movable := 0;
      endif
      arr.append(item);
    elseif(item.isa(POLCLASS_CONTAINER))
      conts[item.serial] := item;
    elseif(!item.movable)
      item.movable := 1;
    endif
  endforeach

  foreach item in arr
    while (item.container)
      item := item.container;

      if(item.movable) // potrzebny jest check, bo inaczej odpala cala operacje ustawienia movable=0, przez co zamyka graczowi pojemniki jak sa otwarte!
        item.movable := 0;
      endif

      if(conts.exists(item.serial))
        conts.erase(item.serial);
      endif
    endwhile
  endforeach

  // a tutaj kontenery, ktore nie dotycza posiadanego zablokowanego itema, takze do unblocka.
  foreach cont in conts
    if(!cont.movable) // potrzebny jest check, bo inaczej odpala cala operacje ustawienia movable=0, przez co zamyka graczowi pojemniki jak sa otwarte!
      cont.movable := 1;
    endif
  endforeach

endfunction

//------------------------------------------------------------------------------------------------

function  DestroyStall(StallKey,bywhom := 0)
 
  var arr := array;
  var Stall := GetDataFile(MP_FILE).FindElement(StallKey);
  var Owner := FindPlayerBySerial(Stall.GetProp(MP_STALL_OWNER));
  if(bywhom.cmdlevel)
    if(owner)
      LogNotice("GM "+GetRealName(bywhom)+" / "+bywhom.acctname+" zniszczyl stragan "+GetRealName(Owner)+"/"+Owner.acctname+" [" + StallKey + "]");
    else
      LogNotice("GM "+GetRealName(bywhom)+" / "+bywhom.acctname+" zniszczyl stragan [" + StallKey + "]");
    endif
  endif
  var Box := SystemFindObjectBySerial(Stall.GetProp(MP_STALL_BOX));
  foreach item in EnumerateItemsInContainer(Box)
    if(item.container == box)
      arr.append(item);
    endif
    EraseSellProps(Item);
  endforeach

  var Locked := Stall.GetProp(MP_STALL_LOCKED);
 
  foreach item in Locked
    arr.append(SystemFindObjectBySerial(item));
    item.movable := 1;
  endforeach

  var Vendor := SystemFindObjectBySerial(Stall.GetProp(MP_STALL_VENDOR));
  var Cash := CInt(Stall.GetProp(MP_STALL_CASH));
  if(Cash)
    var BankBox := FindBankBox(owner);
    var cheque := CreateItemInContainer(BankBox,UOBJ_CHEQUE,1);
    SetChequeVal(Cheque, cash);
    if(!owner.cmdlevel)
       RemoveObjCreator(cheque);
    endif
  endif
   
  var EscrData := GetEscrowData(Owner);

  SetEscrowFilling(EscrData, 1);
  foreach item in arr
    MoveItemToEscrow(item, EscrData, 1);
  endforeach

  SetEscrowFilling(EscrData, 0);
  Stall.EraseProp(MP_STALL_LOCKED);
  DestroyItem(box);
  Stall.EraseProp(MP_STALL_BOX);
  KillNPC(vendor);
  STall.EraseProp(MP_STALL_VENDOR);
  STall.EraseProp(MP_STALL_OWNER);
  STall.EraseProp(MP_STALL_ENABLED);
  STall.EraseProp(MP_STALL_RENTED);
  Stall.EraseProp(MP_STALL_NOTICE);
  Stall.EraseProp(MP_STALL_CASH);

  if(ByWhom)
    GetDataFile(MP_FILE).DeleteElement(StallKey);
    DestroyItem(SystemFindObjectBySerial(StallKey));
    exit;
  endif
endfunction
//-------------------------