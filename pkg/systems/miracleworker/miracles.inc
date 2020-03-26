//-----------------------------------------------------------------------------

include "ritual";

//-----------------------------------------------------------------------------

const UOBJ_MIRACLE_TICKET    := 0xA602;
const MIRACLE_ORDER_EXP_TIME := 345600;
const PROP_ORDER_DATA        := "OrderData";
const PROP_ORDER_GET         := "#MiracleItemGet_";
const MIRACLE_GET_DELAY      := 60;
const MIRACLE_TICKETDONE_CLR := 999;

//-----------------------------------------------------------------------------

function CreateOrderData(who,Npc, SKID)

  var Order := struct;
  var Group;
  
  Order.+Source := Npc.serial;
  Order.+Comp   := 0;
  Order.+Owner  := who.serial;
  Order.+Used   := 0;

  Order.+Items  := array;
  
  var item;
  var cfgItems := ReadConfigFile(":miracleworker:orderitems");
  var aItems := GetConfigStringArray(cfgItems["Items"],"Item");
  
  foreach aItem in aItems
    item := SplitWords(aItem);
    AddOrderItem(Order, CInt(item[1])  , CInt(item[2]));
  endforeach
   
  if(SkID == SKILLID_WEARWOLF)
    if(IsCharClass(who, CLASS_MAGE))
      Group := "WWMage";
    else
      Group := "WWWar";
    endif
  else
    if(IsCharClass(who, CLASS_MAGE))
      Group := "VampMage";
    else
      Group := "VampWar";
    endif
  endif
  var i;
  var runesAmt := GetConfigInt(cfgItems["Items"],"Runes");
  var nr;    
  var List;
  GetRunesList(List, Group);
  for(i :=1 ; i <= runesAmt ; i := i+1 )
    nr := RandomInt(List.size())+1;
    AddOrderItem(Order, GetItemObjType(List[nr]) , 1);
    List.erase(nr);
  endfor

  Order.aTime   := ReadGameClock();
  Order.ExpTime := MIRACLE_ORDER_EXP_TIME;
  
  return Order;

endfunction

//-----------------------------------------------------------------------------

function AddOrderItem(byref Order, ObjType, Amount)

  var Item := struct;
  Item.+ObjType := ObjType;
  Item.+Amount  := Amount;
  Item.+GotAmt  := 0;
  
  Order.Items.append(Item);

endfunction

//-----------------------------------------------------------------------------

function CreateMiracleOrder(who, me, SkID)

  var Data := CreateOrderData(who,me, SkID);
  if(Data)
    var Ticket := CreateItemInPack(who, UOBJ_MIRACLE_TICKET);
    
    if(!Ticket)
      SendSysMessage(who,"Nie moge Ci teraz pomoc");
      return;
    endif
    SetObjProperty(Ticket, PROP_ORDER_DATA, Data);
    return 1;
  endif
  return 0;
endfunction

//-------------------------------------------------------------------------

function SubtractOrderItems(Item, byref Table, byref ItemList, who)

  if(Item.isa(POLCLASS_CONTAINER) and Item.item_count-1 > 0)
    foreach cItem in ListRootItemsInContainer(Item)
      if(cItem.isa(POLCLASS_CONTAINER) and cItem.item_count-1 > 0)
        SubtractOrderItems(cItem, Table, ItemList, who);
      else
        SubtractOrderItem(cItem, Table, ItemList, who);
      endif
    endforeach
  else
    SubtractOrderItem(Item, Table, ItemList, who);
  endif

endfunction

//-------------------------------------------------------------------------------

function SubtractOrderItem(Item, byref Table, byref ItemList, who)

  var Idx := Table[Item.objtype];

  if(Idx)
    if(ItemList[Idx].GotAmt < ItemList[Idx].Amount)
      var sAmt := Item.amount;

      if(Item.objtype == UOBJ_CHEQUE)
        sAmt := GetChequeVal(Item);
      endif

      if(ItemList[Idx].GotAmt + sAmt > ItemList[Idx].Amount)
        sAmt := ItemList[Idx].Amount - ItemList[Idx].GotAmt;
      endif

      var bRes := 0;

      if(Item.objtype == UOBJ_CHEQUE)
        bRes := SubtractCheque(who, Item, sAmt);
      else
        bRes := SubtractAmount(Item, sAmt);
      endif

      if(bRes)
        ItemList[Idx].GotAmt := ItemList[Idx].GotAmt + sAmt;
      endif
    endif
  endif

endfunction

//-------------------------------------------------------------------------------

function CheckOrderItemCont(Item, byref Table)

  var Res;
 
  foreach cItem in ListRootItemsInContainer(Item)
    if(cItem.isa(POLCLASS_CONTAINER) and cItem.item_count-1 > 0)
      Res := CheckOrderItemCont(cItem, Table);
      if(Res.isa(POLCLASS_ITEM))
        return Res;
      endif
    elseif(!Table[cItem.objtype])
      return cItem;
    endif
  endforeach

  return 1;

endfunction

//---------------------------------------------------------------------------

function CheckTicketComplete(Npc, byref Order)

  foreach Item in (Order.Items)
    if(Item.GotAmt != Item.Amount)
      return 0;
    endif
  endforeach

  case(RandomInt(3))
    1:       PrintTextAbove(Npc, "Kto by pomyslal, ze dasz rade...");
    2:       PrintTextAbove(Npc, "Ehh, wiec znow bede mial robote...");
    default: PrintTextAbove(Npc, "Wyglada na to, ze to juz wszystko.");
  endcase


  Order.Comp := 1;

  return 1;

endfunction

//------------------------------------------------------------------

function GetRunesList(byref List, prop)
  var GroupCfg  := ReadConfigFile(ITEMGROUPS_CFG);
  var Keys     := GetConfigStringKeys(GroupCfg);
  var Names := array;
  List := array;
  foreach Key in Keys
    
    if(GetConfigInt(GroupCfg[Key],prop))
      Names.append(Key);
    endif

  endforeach
  
  var runes;
  var run;
  foreach Name in Names
    runes := GetConfigStringArray(GroupCfg[Name],"Item");
    foreach rune in runes
      run := SplitWords(rune);
      List.append(run[2]);
    endforeach
  endforeach
  
endfunction
      