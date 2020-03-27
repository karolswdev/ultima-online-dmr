//-----------------------------------------------------------------------------

include ":banking:gold";
include "include/npcs";

//-----------------------------------------------------------------------------

function AISys_Merchant_ProcEvent(Event)

  if(!AISys_Merchant_CanTrade(Event))
    return EVRES_UNKNOWN;
  endif

  case(Event.type)
    
    EVID_DAMAGED:            return AISys_Merchant_Damaged(Event);

    EVID_ENGAGED:            return AISys_Merchant_Engaged(Event);
    
    EVID_PUSH:               return AISys_Merchant_Push(Event);

    EVID_CMERC_SOLD:         return AISys_Merchant_ProcSold(Event);

    EVID_CMERC_BOUGHT:       return AISys_Merchant_ProcBought(Event);

    EVID_ITEM_GIVEN:         return AISys_Merchant_ProcessItem(Event);

    default:                 return EVRES_UNKNOWN;

  endcase

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_ProcessItem(Event)

  if(Event.item.objtype == UOBJ_GOLDCOIN)
    if(!AISys_Merchant_TakeGold(Event.source, Event.item))
      MoveToPack(Event.item, Event.source);
      return EVRES_PROCESS;
    endif
  elseif(!AISys_Merchant_TakeItem(Event.source, Event.item))
    MoveToPack(Event.item, Event.source);
    return EVRES_PROCESS;
  endif

  return EVRES_SUCCESS;

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_TakeGold(Source, Item)

  if(RandomInt(100) < (Source.gold / 100) and Source.gold > 1000)

    AISys_Speech_Say(Source, AI_SPID_GOLD_THANKS);
    DestroyItem(Item);
    return 1;

  else

    AISys_Speech_Say(Source, AI_SPID_NONEED_GOLD);

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_TakeItem(Source, Item)

  if(_AI_Merchant.Amounts.exists(hex(Item.objtype)))
    if(RandomInt(100) < 100 - CInt((_AI_Merchant.Amounts[hex(Item.objtype)] * 100) / _AI_Merchant.MaxAmounts[hex(Item.objtype)]))
      
      AISys_Speech_Say(Source, AI_SPID_TAKE_ITEM);

      if(_AI_Merchant.Amounts[hex(Item.objtype)] + Item.amount <= _AI_Merchant.MaxAmounts[hex(Item.objtype)])
        _AI_Merchant.Amounts[hex(Item.objtype)] := _AI_Merchant.Amounts[hex(Item.objtype)] + Item.amount;
      else
        _AI_Merchant.Amounts[hex(Item.objtype)] := _AI_Merchant.MaxAmounts[hex(Item.objtype)];
      endif

      SetProperty(PROP_MERC_AMOUNTS, _AI_Merchant.Amounts);
      DestroyItem(Item);
      return 1;

    endif
    
    AISys_Speech_Say(Source, AI_SPID_NO_NEED_ITEM);

  else

    AISys_Speech_Say(Source, AI_SPID_NO_ITEM_FOR_ME);

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_ProcSold(Event)

  if(!Event.source or !IsPlayer(Event.source) or Event.source.dead)
    return EVRES_FAILED;
  endif

  if(!CanNpcSpeekWith(Self(), Event.source))
    return EVRES_FAILED;
  endif

  if(!Event.items.size() or !Event.items)
    AISys_Speech_Say(Event.source, AI_SPID_ANOTHER_TIME);
    return EVRES_FAILED;
  endif

  var aPrice  := 0;
  var RepLvl  := GetPlayerNpcGuildCityLvl(Event.source, _AI_Merchant.City);
  var dItems  := ListItemsToSell(Event.source, _AI_Merchant.City, _AI_Merchant.BuyItems);
  var Amt, sAmt, Price, ElemName, Desc, ObjType;

  foreach Item in (Event.items)
    Item[1]  := FindItemBySerial(Item[1]);
    Price    := GetItemBuyPrice(Item[1], RepLvl);
    Desc     := CutItemDesc(Item[1]);
    ElemName := Item[1].objtype+"_"+Desc+"_"+Price;
    Amt      := CInt(Item[2]);

    if(!dItems.exists(ElemName))
      AISys_Speech_JustSay(Event.source, Event.source.name+", probujesz mnie oszukac ?!");
      return EVRES_FAILED;
    endif

    if(dItems[ElemName][2] < Item[2])
      AISys_Speech_JustSay(Event.source, Event.source.name+", nie widze bys mial"+ggm(Event.source,4)+" przy sobie taka ilosc przedmiotu "+Desc+", jaka chcesz sprzedac.");
      return EVRES_FAILED;
    endif

    foreach dItem in (dItems[1])
      if(!ReserveItem(dItem))
        AISys_Speech_JustSay(Event.source, Event.source.name+", uzywasz przedmiotu "+Desc+", jak wiec chcesz go sprzedac ?!");
        return EVRES_FAILED;
      endif

      Amt := Amt - dItem.amount;
      if(Amt <= 0)
        break;
      endif
    endforeach

  endforeach

  var NewAmounts := _AI_Merchant.Amounts;
  
  foreach Item in (Event.items)
    Price    := GetItemBuyPrice(Item[1], RepLvl);
    ObjType  := Item[1].objtype;
    Desc     := CutItemDesc(Item[1]);
    ElemName := ObjType+"_"+Desc+"_"+Price;
    Amt      := CInt(Item[2]);

    if(!Item[1])
      continue;
    endif

    if(!dItems.exists(ElemName))
      AISys_Speech_JustSay(Event.source, Event.source.name+", probujesz mnie oszukac ?!");
      return EVRES_FAILED;
    endif

    foreach dItem in (dItems[ElemName][1])
      if(!dItem)
        continue;
      endif

      sAmt := dItem.amount;
      if(Amt - sAmt < 0)
        sAmt := Amt;
      endif

      if(!SubtractAmount(dItem, sAmt))
        AISys_Speech_JustSay(Event.source, Event.source.name+", mam problem z pobraniem od Ciebie przedmiotu "+Desc+", czemu nie chcesz go oddac?");
        break;
      elseif(NewAmounts.exists(hex(ObjType)))
        NewAmounts[hex(ObjType)] := NewAmounts[hex(ObjType)] + sAmt;
      endif

      if(dItem)
        ReleaseItem(dItem);
      endif

      Amt    := Amt - sAmt;
      aPrice := aPrice + (Price * sAmt);

      if(Amt <= 0)
        break;
      endif
    endforeach

    if(Amt > 0)
      AISys_Speech_JustSay(Event.source, Event.source.name+", wyglada na to, ze brakuje Ci jakims smoczym cudem "+Amt+" "+Desc+" !");
      break;
    endif

  endforeach
  
  _AI_Merchant.Amounts := NewAmounts;
  SetProperty(PROP_MERC_AMOUNTS, _AI_Merchant.Amounts);

  if(!aPrice)
    AISys_Speech_JustSay(Event.source, Event.source.name+", wyglada na to, ze nic mi nie sprzedasz...");
    return EVRES_FAILED;
  endif

  if(CreateGold(Event.source.backpack, aPrice, Event.source))
    AISys_Speech_JustSay(Event.source, Event.source.name+", sprzedal"+ggm(Event.source,3)+" przedmioty za laczna sume "+aPrice+" gp.", AI_SPF_SAY_PRIVATE);
    
    if(aPrice < 7500)
      AISys_Speech_Say(Event.source, AI_SPID_LOW_PRICE_SELL);
    elseif(aPrice < 100000)
      AISys_Speech_Say(Event.source, AI_SPID_MID_PRICE_SELL);
    else
      AISys_Speech_Say(Event.source, AI_SPID_HIGH_PRICE_SELL);
    endif

    AISys_Handler_Trigger(Event.source, AIHT_SUCCESSFUL_SELL);
  else
    AISys_Speech_JustSay(Event.source, Event.source.name+", nie masz miejsca w plecaku!", AI_SPF_SAY_PRIVATE);
  endif

  return EVRES_SUCCESS;

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_ProcBought(Event)

  if(!Event.source or !IsPlayer(Event.source) or Event.source.dead)
    return EVRES_FAILED;
  endif

  if(!Event.items or !Event.items.size())
    return EVRES_FAILED;
  endif

  if(!CanNpcSpeekWith(Self(), Event.source))
    return EVRES_FAILED;
  endif

  var Amt, iItem;
  var BuyItems := array;
  
  foreach Item in (Event.items)
    Amt  := CInt(Item[2]);
    if(Amt)
      iItem := SystemFindObjectBySerial(Item[1]);
      if(iItem)
        if(CInt(_AI_Merchant.Amounts[hex(iItem.objtype)]) - Amt <= 0)
          Amt := CInt(_AI_Merchant.Amounts[hex(iItem.objtype)]);
        endif
        if(Amt)
          Item[2] := Amt;
          BuyItems.append(Item);
        endif
      endif
    endif
  endforeach

  var RepLvl := GetPlayerNpcGuildCityLvl(Event.source, _AI_Merchant.City);
  var aPrice := CountItemsSellPrice(BuyItems, RepLvl);
  
  if(!aPrice)
    AISys_Speech_JustSay(Event.source, "Niestety spoznil"+ggm(Event.source,3)+" sie, przedmioty te zostaly wykupione.");
    return;
  elseif(!SpendGold(Event.source, aPrice))
    AISys_Speech_JustSay(Event.source, "Nie masz wystarczajaco zlota, potrzebujesz "+aPrice+" gp.");
    return;
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  
  foreach Item in BuyItems
    Amt   := CInt(Item[2]);
    iItem := FindItemBySerial(Item[1]);

    _AI_Merchant.Amounts[hex(iItem.objtype)] := _AI_Merchant.Amounts[hex(iItem.objtype)] - Amt;

    if(ItemCfg[iItem.objtype].animaltemplate)

      var i,Pet;
            
      for(i:=1;i<=Amt;i:=i+1)
        Pet := CreateRPNpc(ItemCfg[iItem.objtype].animaltemplate, Self(), Self().realm, NPCF_NO_LOOT, CRNPCF_CHECKLOS);
        Tame(Pet, Event.source);
      endfor

    else

      if(IsStackableObj(iItem.objtype))
        CreateItemInPack(Event.source, iItem.objtype,Amt);
      else
        var i;
        for(i:=1;i<=Amt;i:=i+1)
          CreateItemInPack(Event.source, iItem.objtype);
        endfor
      endif

    endif
  endforeach

  SetProperty(PROP_MERC_AMOUNTS, _AI_Merchant.Amounts);

  AISys_Speech_JustSay(Event.source, Event.source.name+", zakupil"+ggm(Event.source,3)+" przedmioty za laczna sume "+aPrice+" gp.", AI_SPF_SAY_PRIVATE);

  if(aPrice < 7500)
    AISys_Speech_Say(Event.source, AI_SPID_LOW_PRICE_BUY);
  elseif(aPrice < 100000)
    AISys_Speech_Say(Event.source, AI_SPID_MID_PRICE_BUY);
  else
    AISys_Speech_Say(Event.source, AI_SPID_HIGH_PRICE_BUY);
  endif
  
  return EVRES_SUCCESS;

endfunction

//-----------------------------------------------------------------------------

