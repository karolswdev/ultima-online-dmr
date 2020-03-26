//-----------------------------------------------------------------------------

include ":banking:gold";
include "include/logerror";
include "ai/include/shop_packets";
include "ai/include/ainpcs";
include "ai/include/tamed";
include "ai/include/npch";
include "include/player/target";
include ":artifacts:artifacts";
include ":tinkering:common";
include ":bulks:common";
include ":magicitems:magicitems";

//-----------------------------------------------------------------------------

const MERC_RESTOCKM          := 30;
const RESTOCK_MINUTES        := 30;

const UOBJ_MERCHANT_BOX      := 0xa297;
const UOBJ_MERCHANT_POUCH    := 0x966a;

const PROP_MERC_AMOUNTS      := "_Amounts";

//-----------------------------------------------------------------------------

var _BuyItems, _SellItems, _Amounts, _MaxAmounts, _RepLvl, _MercBox;

//=============================================================================
//-----------------------------------------------------------------------------
// Std Functions
//-----------------------------------------------------------------------------
//=============================================================================

function GetMercBox(Temp)

  return GetStorageItem(MERC_STORAGE,Temp+MERC_FSBOX);
  
endfunction

//-----------------------------------------------------------------------------

function GetOldBox()

  return GetStorageItem(MERC_STORAGE,MERC_PBBOX);

endfunction

//-----------------------------------------------------------------------------

function GetStorageItem(StorName,BoxName)

  var Storage := FindStorageArea(StorName);
  if(!Storage)
    Storage := CreateStorageArea(StorName);
    if(Storage)
      LogNotice("[Merchant] Stworzono Storage ["+StorName+"].", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
    endif
  endif

  var Box := FindRootItemInStorageArea(Storage, BoxName);
  if(!Box)
    Box := CreateRootItemInStorageArea( Storage, BoxName, UOBJ_MERCHANT_BOX );
    if(Box)
      LogNotice("[Merchant] Stworzono kontener ["+BoxName+"] w Storage ["+StorName+"].", LOGF_NOTE_INFO | LOGF_NO_NEED | LOGF_SCRIPTS);
    endif
  endif

  return Box;

endfunction

//-----------------------------------------------------------------------------

function FindItemInMercBox(ObjType)

  ObjType  := hex(ObjType);

  var Item := FindObjtypeInContainer(_MercBox, Objtype);
  if(!Item)
    Item := CreateItemInCont(_MercBox,ObjType,1);
  endif

  return Item;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Merchant Items
//-----------------------------------------------------------------------------
//=============================================================================

function MerchantInit()

  _MercBox := GetMercBox(me.npctemplate);

  DefineItems();
  DefineBuyItems();

endfunction

//-----------------------------------------------------------------------------

function GetMercType()

  return GetNpcConfig(me).MercType;

endfunction

//-----------------------------------------------------------------------------

function DefineBuyItems()

  _BuyItems   := dictionary;
  var MercCfg := ReadConfigFile(MRCSPAWN_CFG);
  var Groups  := GetConfigStringArray(MercCfg[GetMercType()],"Buys");
  var ObjType;

  foreach Group in Groups
    foreach Item in GetConfigStringArray(MercCfg[Group],"Item")
      Item    := SplitWords(Item);
      ObjType := GetItemObjType(Item[1]);
      if(!ObjType)
        LogNotice("[Merchant] DefineBuyItems: Bledny Buy ObjType w grupie ["+Group+"]: "+Item[1], LOGF_SCRIPTS);
        continue;
      endif
      _BuyItems[hex(ObjType)] := 1;
    endforeach
  endforeach

endfunction

//-----------------------------------------------------------------------------

function DefineItems()

  var MercCfg := ReadConfigFile(MRCSPAWN_CFG);
  var Groups  := GetConfigStringArray(MercCfg[GetMercType()],"Sells");
  _Amounts    := GetObjProperty(me, PROP_MERC_AMOUNTS);
  var MercItem, ObjType;

  _SellItems := array;
  
  if(!_Amounts)
    _Amounts := dictionary;
  endif
  
  _MaxAmounts := dictionary;

  foreach Group in Groups
    foreach Item in GetConfigStringArray(MercCfg[Group],"Item")
      Item    := SplitWords(Item);
      ObjType := GetItemObjType(Item[1]);
      if(!ObjType)
        LogNotice("[Merchant] DefineItems: Bledny Sell ObjType w grupie ["+Group+"]: "+Item[1], LOGF_SCRIPTS);
        continue;
      endif
      MercItem := FindItemInMercBox(ObjType);

      if(MercItem and !(MercItem in _SellItems))
        if(!_Amounts.exists(hex(MercItem.objtype)))
          _Amounts[hex(MercItem.objtype)] := CInt(Item[2]);
        elseif(CInt(_Amounts[hex(MercItem.objtype)]) > CInt(Item[2]))
          _Amounts[hex(MercItem.objtype)] := CInt(Item[2]);
        endif
        _MaxAmounts[hex(MercItem.objtype)] := CInt(Item[2]);
        _SellItems.append( MercItem );
      endif
    endforeach
  endforeach
  
  if(!_Amounts[1])
    _Amounts[1] := ReadGameClock() + MERC_RESTOCKM * 60;
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckMercRestock()

  if(_Amounts[1] < ReadGameClock())
    MercRestock();
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function MercRestock()

  var ObjTypes := _MaxAmounts.keys();

  foreach ObjType in ObjTypes
    _Amounts[ObjType] := _MaxAmounts[ObjType];
  endforeach
  
  _Amounts[1] := ReadGameClock() + MERC_RESTOCKM * 60;
  SetObjProperty(me, PROP_MERC_AMOUNTS, _Amounts);

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Build & Send Packet
//-----------------------------------------------------------------------------
//=============================================================================

function MercSendBuy(who)

  if(!_MercBox)
    _MercBox := GetMercBox(me.npctemplate);
  endif

  var OldBox := GetOldBox();

  _RepLvl     := GetPlayerNpcGuildCityLvl(who, City);
  var sItems := array;
  var Amt, i;

  for(i:=1;i<=_SellItems.size();i:=i+1)
    Amt := _Amounts[hex(_SellItems[i].objtype)];
    if(Amt > 0)
      sItems.append( array(_SellItems[i], Amt, GetItemPrice(_SellItems[i], 1)) );
    endif
  endfor

  return SendBuyPacket(who, me, _MercBox, OldBox, sItems);

endfunction

//-----------------------------------------------------------------------------

function MercSendSell(who)

  if(!_MercBox)
    _MercBox := GetMercBox(me.npctemplate);
  endif

  var OldBox := GetOldBox();

  var sItems   := array;
  var dItems   := dictionary;
  
  DefineSoldItems(who, dItems);

  foreach Item in dItems
    sItems.append( array(Item[1][1], Item[2], Item[3]) );
  endforeach

  var Res := SendSellPacket(who, me, _MercBox, OldBox, sItems);

  if(Res == 0 and !ev.source.concealed)
    case(RandomInt(3))
      0:       PrintTextAbove(me,ev.source.name+", nie masz nic co by mnie interesowalo.");
      1:       PrintTextAbove(me,ev.source.name+", nie widze bys mial jakies przedmioty dla mnie.");
      default: PrintTextAbove(me,"Dzieki "+ev.source.name+", ale wydaje mi sie ze nie masz nic ciekawego.");
    endcase
  endif

  return Res;

endfunction

//-----------------------------------------------------------------------------

function DefineSoldItems(who, byref dItems)

  _RepLvl := GetPlayerNpcGuildCityLvl(who, City);
  dItems := dictionary;

  var Price, ElemName;

  foreach Item in EnumerateItemsInContainer(who.backpack)
    if(WillIBuy(Item))
      Price    := GetItemPrice(Item);
      ElemName := Item.objtype+"_"+CutItemDesc(Item)+"_"+Price;

      if(dItems.exists(ElemName))
        dItems[ElemName][2] := dItems[ElemName][2] + Item.amount;
        dItems[ElemName][1].append(Item);
      else
        dItems[ElemName] := array(array(Item), Item.amount, Price);
      endif
    endif
  endforeach

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Receive Functions
//-----------------------------------------------------------------------------
//=============================================================================

function Bought(byref ev)

  if(!ev.source or !ev.items or !ev.items.size())
    return 0;
  endif
  
  if(!IsPlayer(ev.source) or ev.source.dead)
    return 0;
  endif

  if((Distance(me,ev.source) > 2 or !CheckLineOfSight(me,ev.source) or DistZ(me,ev.source) >= 20) and !ev.source.cmdlevel)
    return 0;
  endif

  var Amt, iItem;

  ev.+buyitems;
  ev.buyitems := array;
  
  foreach Item in (ev.items)
    Amt  := CInt(Item[2]);
    if(Amt)
      iItem := SystemFindObjectBySerial(Item[1]);
      if(iItem)
        if(CInt(_Amounts[hex(iItem.objtype)]) - Amt <= 0)
          Amt := CInt(_Amounts[hex(iItem.objtype)]);
        endif
        if(Amt)
          Item[2] := Amt;
          ev.buyitems.append(Item);
        endif
      endif
    endif
  endforeach

  var Gold := CountItemsPrice(ev.source,ev.buyitems,1);
  
  if(!Gold)
    Say("Niestety spoznil"+ggm(ev.source,3)+" sie, przedmioty te zostaly wykupione.");
    return;
  elseif(!SpendGold(ev.source, Gold))
    Say("Nie masz wystarczajaco zlota, potrzebujesz "+Gold+" gp.");
    return;
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  
  foreach Item in (ev.buyitems)
    Amt   := CInt(Item[2]);
    iItem := SystemFindObjectBySerial(Item[1]);

    _Amounts[hex(iItem.objtype)] := _Amounts[hex(iItem.objtype)] - Amt;

    if(ItemCfg[iItem.objtype].animaltemplate)

      var i,Pet;
            
      for(i:=1;i<=Amt;i:=i+1)
        Pet := CreateRPNpc(ItemCfg[iItem.objtype].animaltemplate, me, me.realm, NPCF_NO_LOOT, CRNPCF_CHECKLOS);
        Tame(Pet, ev.source);
      endfor

    else

      if(IsStackableObj(iItem.objtype))
        CreateItemInPack(ev.source,iItem.objtype,Amt);
      else
        var i;
        for(i:=1;i<=Amt;i:=i+1)
          CreateItemInPack(ev.source,iItem.objtype);
        endfor
      endif

    endif
  endforeach

  if(Gold < 2500)
    Say("Cena twych zakupow wyniosla tylko "+Gold+" gp, zapraszam ponownie.");
  elseif(Gold < 7500)
    Say("Cena twych zakupow wyniosla "+Gold+" gp, zapraszam ponownie.");
  elseif(Gold < 20000)
    case(RandomInt(4))
      0:       Say("Ciesze sie ze mogl"+ggm(me,8)+" pomoc, cena twoich zakupow wyniosla "+Gold+" gp, zapraszam ponownie o kazdej porze!");
      1:       Say("Jestem zadowolon"+ggm(me,6)+" ze postanowil"+ggm(ev.source,3)+" zrobic u mnie zakupy, ktorych cena wyniosla "+Gold+" gp, zapraszam ponownie!");
      default: Say("Zapraszam ponownie, cena twych zakupow wyniosla "+Gold+" gp, zycze milego dnia.");
    endcase
  elseif(Gold < 125000)
    case(RandomInt(4))
      1:       Say("Jest mi niezmiernie milo, cena twych zakupow u mnie wyniosla "+Gold+" gp, zapraszam ponownie !");
      2:       Say("Cena twych zakupow u mnie wyniosla "+Gold+" gp, zapraszam ponownie i milego dnia zycze!");
      default: Say("Ciesze sie ze wybral"+ggm(ev.source,3)+" moj sklep, cena twych zakupow u mnie wyniosla "+Gold+" gp, zapraszam o kazdej porze!");
    endcase
  else
    Say("Zapraszam o kazdej porze! Mam nadzieje ze jestes zadowolon"+ggm(ev.source,6)+" z zakupow u mnie, ktore wyniosly az "+Gold+" gp!");
  endif
  
  SetObjProperty(me, PROP_MERC_AMOUNTS, _Amounts);

endfunction

//-----------------------------------------------------------------------------

function Sold(byref ev)

  if(!ev.source)
    return 0;
  endif

  if(!ev.items.size() or !ev.items)
    case(RandomInt(3))
      0:       Say(ev.source.name+", nie masz nic co by mnie interesowalo.");
      1:       Say(ev.source.name+", nie widze bys mial jakies przedmioty dla mnie.");
      default: Say("Dzieki "+ev.source.name+", ale wydaje mi sie ze nie masz nic ciekawego.");
    endcase
    return 0;
  endif

  if(!IsPlayer(ev.source) or ev.source.dead)
    return 0;
  endif

  if((Distance(me,ev.source) > 2 or !CheckLineOfSight(me,ev.source) or DistZ(me,ev.source) >= 20) and !ev.source.cmdlevel)
    return 0;
  endif

  var aPrice := 0;
  var dItems := dictionary;
  var Amt, sAmt, Price, ElemName, Desc, ObjType;

  DefineSoldItems(ev.source, dItems);

  foreach Item in (ev.items)
    Item[1]  := FindItemBySerial(Item[1]);
    Price    := GetItemPrice(Item[1]);
    Desc     := CutItemDesc(Item[1]);
    ElemName := Item[1].objtype+"_"+Desc+"_"+Price;
    Amt      := CInt(Item[2]);

    if(!dItems.exists(ElemName))
      PrintTextAbove(me,ev.source.name+", probujesz mnie oszukac ?!");
      return 0;
    endif

    if(dItems[ElemName][2] < Item[2])
      PrintTextAbove(me,ev.source.name+", nie widze bys mial"+ggm(ev.source,4)+" przy sobie taka ilosc przedmiotu "+Desc+", jaka chcesz sprzedac.");
      return 0;
    endif

    foreach dItem in (dItems[1])
      if(!ReserveItem(dItem))
        PrintTextAbove(me, ev.source.name+", uzywasz przedmiotu "+Desc+", jak wiec chcesz go sprzedac ?!");
        return 0;
      endif

      ReleaseItem(dItem);

      Amt := Amt - dItem.amount;
      if(Amt <= 0)
        break;
      endif
    endforeach

  endforeach

  var SelfCraftedPrice := 0;
  
  foreach Item in (ev.items)
    Price    := GetItemPrice(Item[1]);
    ObjType  := Item[1].objtype;
    Desc     := CutItemDesc(Item[1]);
    ElemName := ObjType+"_"+Desc+"_"+Price;
    Amt      := CInt(Item[2]);

    if(GetItemMakerSerial(Item[1]) == ev.source.serial)
      SelfCraftedPrice += Price * Amt;
    endif

    if(!Item[1])
      continue;
    endif

    if(!dItems.exists(ElemName))
      PrintTextAbove(me,ev.source.name+", probujesz mnie oszukac ?!");
      return 0;
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
        PrintTextAbove(me, ev.source.name+", mam problem z pobraniem od Ciebie przedmiotu "+Desc+", czemu nie chcesz go oddac?");
        break;
      elseif(_Amounts.exists(hex(ObjType)))
        _Amounts[hex(ObjType)] := _Amounts[hex(ObjType)] + sAmt;
      endif

      Amt    := Amt - sAmt;
      aPrice := aPrice + (Price * sAmt);

      if(Amt <= 0)
        break;
      endif
    endforeach

    if(Amt > 0)
      PrintTextAbove(me, ev.source.name+", wyglada na to, ze brakuje Ci jakims smoczym cudem "+Amt+" "+Desc+" !");
      break;
    endif

  endforeach
  
  SetObjProperty(me, PROP_MERC_AMOUNTS, _Amounts);

  if(!aPrice)
    PrintTextAbove(me, ev.source.name+", wyglada na to, ze nic mi nie sprzedasz...");
    return 0;
  endif

  if(CreateGold(ev.source.backpack, aPrice, ev.source))
    PrintTextAbovePrivate(me,"Sprzedal"+ggm(ev.source,3)+" przedmioty za laczna sume "+aPrice+" gp.",ev.source);
    
    if(aPrice < 2500)
      case(RandomInt(3))
        0:       Say("Musze przyznac ze to nie byl interes twojego zycia.");
        1:       Say("Ostatni moj klient zarobil znacznie wiecej.");
        default: Say("Chociaz tym razem wiele nie zaplacil"+ggm(me,8)+".");
      endcase
    elseif(aPrice < 7500)
      case(RandomInt(3))
        0:       Say("Ehhh czy to juz wszystko?");
        1:       Say("W ten sposob napewno nie zarobisz na dom.");
        default: Say("To byl jeden ze slabszych interesow jaki zrobil"+ggm(me,8)+".");
      endcase
    elseif(aPrice < 20000)
      case(RandomInt(4))
        0:       Say("Wiesz, wlasnie stal"+ggm(ev.source,3)+" sie moim ulubionym klientem.");
        1:       Say("No no no musze przyznac ze ladny ubilismy interes.");
        default: Say("W koncu nauczyl"+ggm(ev.source,3)+" sie targowac.");
      endcase
    elseif(aPrice < 125000)
      case(RandomInt(4))
        1:       Say("Ehh a matka ostrzegala nie kupuj wszystkiego co widzisz.");
        2:       Say("Lepiej szybko udaj sie do banku... Niebezpiecznie jest chodzic z taka kwota po ulicach.");
        default: Say("Uhhh mogl"+ggm(me,8)+" lepiej wytargowac cene.");
      endcase
    else
      case(RandomInt(4))
        1:       Say("Ktore miasto chcesz kupic?");
        2:       Say("Mam nadzieje ze juz wiecej z czyms takim u mnie nie zawitasz bo splajtuje.");
        default: Say("Trzymajcie mnie bo zemdleje...");
      endcase
    endif

    if(SelfCraftedPrice > 0)
      var Data := struct;
      Data.+npc := me;
      Data.+source := ev.source;
      Data.+price := SelfCraftedPrice;
      start_script(":bulks:bulk_check", Data);
    endif
  else
    PrintTextAbovePrivate(me,"Nie masz miejsca w plecaku!",ev.source);
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function WillIBuy(Item)

  if(CInt(_BuyItems[hex(Item.objtype)]) and !Item.newbie and !Item.container.newbie and Item.item_count == 1)
    return 1;
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function SellAll(who)

  if(who.dead or !IsPlayer(who))
    return 0;
  endif

  if(Distance(who,me) > 2)
    return 0;
  endif

  var Res := SendNpcHandleEvent(EVID_NH_YESNO_GUMP,who,me,"Wszystko?");
  if(!Res or !Res.res)
    PrintTextAbovePrivate(me,"Innym razem wiec",who);
    return 0;
  endif

  if(who.dead or !IsPlayer(who))
    return 0;
  endif

  if(Distance(who,me) > 2)
    return 0;
  endif

  var sItems := array;
  
  foreach Item in EnumerateItemsInContainer(who.backpack)
    if(Item.container == who.backpack)
      if(WillIBuy(Item))
        sItems.append({Item.serial,Item.amount});
      endif
    endif
  endforeach
  
  var sev     := struct;
  sev.+source := who;
  sev.+items  := sItems;
  
  return Sold(sev);
  
endfunction

//-----------------------------------------------------------------------------

function SellAllCont(who)

  if(who.dead or !IsPlayer(who))
    return 0;
  endif

  if(Distance(who,me) > 2)
    return 0;
  endif

  PrintTextAbovePrivate(me,"Z ktorego pojemnika?",who);
  var Cont := ObjTarget(who);

  if(!Cont.isa(POLCLASS_CONTAINER))
    PrintTextAbovePrivate(me,"To nie pojemnik",who);
    return;
  endif

  if(Cont.container != who.backpack)
    PrintTextAbovePrivate(me,"Ten pojemnik nie znajduje sie w twoim plecaku",who);
    return;
  endif

  var Items := array;

  foreach Item in EnumerateItemsInContainer(Cont)
    if(Item.container == Cont)
      if(WillIBuy(Item))
        Items.append({Item.serial,Item.amount});
      endif
    endif
  endforeach
  
  if(!Items.size())
    Say("Nie masz w tym pojemniku nic co by mnie interesowalo.");
    return;
  endif

  var Res := SendNpcHandleEvent(EVID_NH_YESNO_GUMP,who,me,"Wszystko?");
  if(!Res or !Res.res)
    PrintTextAbovePrivate(me,"Innym razem wiec",who);
    return 0;
  endif

  if(who.dead or !IsPlayer(who))
    return 0;
  endif

  if(Distance(who,me) > 2)
    return 0;
  endif

  var mEv     := struct;
  mEv.+source := who;
  mEv.+items  := Items;

  return Sold(mEv);

endfunction

//-----------------------------------------------------------------------------

function CountItemsPrice(who,Items,RepRev := 0)

  var Price := 0,Amt;
  _RepLvl    := GetPlayerNpcGuildCityLvl(who, City);
  
  foreach Item in Items
    Amt  := CInt(Item[2]);
    Item := SystemFindObjectBySerial(Item[1]);
    if(Item)
      Price := Price + (GetItemPrice(Item,RepRev) * Amt);
    endif
  endforeach
  
  return Price;

endfunction

//-----------------------------------------------------------------------------

function GetItemPrice(Item, isBuy := 0)

  var cost := 0;
  var RLvl := _RepLvl;

  if(!City)
    RLvl := 0;
  endif
  
  if(!isBuy)
    cost := GetItemBuyPrice(Item, RLvl);
  else
    cost := GetItemSellPrice(Item, RLvl);
  endif

  return CInt(cost);

endfunction

//-----------------------------------------------------------------------------

function MercTakeItem(me, who, Item)

  var Data := struct;
  Data.+npc := me;
  Data.+source := who;
  Data.+param := Item;

  if(run_script(":bulks:process_item", Data))
    return;
  else
    if(CheckNpcTakeItem(me, who, Item))
      return;
    endif
  endif

  if(_Amounts.exists(hex(Item.objtype)))
    if(RandomInt(100) < 100 - CInt((_Amounts[hex(Item.objtype)] * 100) / _MaxAmounts[hex(Item.objtype)]))
      case(RandomInt(4))
        1:        Say("Dziekuje, przyda sie moze jednak.");
        2:        Say("Jesli nalegasz, to nie bede odmawial"+ggm(me,4)+".");
        default:  Say("Nie pogardze, jesli Ci jest to zbedne.");
      endcase

      if(_Amounts[hex(Item.objtype)] + Item.amount <= _MaxAmounts[hex(Item.objtype)])
        _Amounts[hex(Item.objtype)] := _Amounts[hex(Item.objtype)] + Item.amount;
      else
        _Amounts[hex(Item.objtype)] := _MaxAmounts[hex(Item.objtype)];
      endif
      SetObjProperty(me, PROP_MERC_AMOUNTS, _Amounts);
      DestroyItem(Item);
      return;
    endif
    
    case(RandomInt(20))
      0:
      1:
      2:
      3:
      4:       Say("Dziekuje ale nie trzeba, mam tego pod dostatkiem.");
      5:       Say("Nie mecz mnie... nie jest mi to potrzebne.");
      default: Say("Sam to sprzedaje, wiec nie trzeba mi wiecej niz mam.");
    endcase
  else
    case(RandomInt(3))
      0: Say("Dziekuje, ale nie potrzebuje tego.");
      2: Say("Nie jest mi to potrzebne.");
      default: Say("Po co mi te smieci...");
    endcase
  endif

  MoveToPack(Item,who);
  
endfunction

//-----------------------------------------------------------------------------

function StdMercHandleEvent(byref ev)
 
  if(!ev)
    return 0;
  endif
  
  case(ev.type)
    EVID_CMERC_BOUGHT:       Bought(ev);
                             ev := 0;
                             return 1;

    EVID_CMERC_SOLD:         Sold(ev);
                             ev := 0;
                             return 1;

    EVID_ITEM_GIVEN:         MercTakeItem(me, ev.source, ev.item);
                             ev := 0;
                             return 1;
    
    EVID_ENTEREDAREA:        
                             if(!CanGreet(ev))
                               ev := 0;
                               return;
                             endif

                             TurnToward(ev.source);

                             case(RandomInt(4))
                               0:       Say("Witaj wedrowcze! W czym moge pomoc?");
                               1:       Say("Witaj " + ev.source.name + "! Zapraszam do obejrzenia moich towarow");
                               2:       Say("Witaj " + ev.source.name + "! Obejrzyj me towary.");
                               3:       Say("Witaj " + ev.source.name + "! Sluze pomoca.");
                             endcase

                             SetObjProperty(me, "#GreetDelay", ReadGameClock() + 10);

  endcase

  return StdCityNpcHandleEvent(ev);

endfunction

//-----------------------------------------------------------------------------

function CanGreet(byref ev)
  
  var GreetDelay := Cint(GetObjProperty(me, "#GreetDelay"));
  
  if(!GreetDelay)
	SetObjProperty(me, "#GreetDelay", ReadGameClock() + 10);
  endif
  
  if(ReadGameClock() <= GreetDelay)
    SetObjProperty(me, "#GreetDelay", ReadGameClock() + 10);
    return 0;
  endif

  if(!IsPlayer(ev.source) or ev.source.dead or ev.source.concealed or ev.source.hidden)
    return 0;
  endif

  if(!CheckLineOfSight(me, ev.source))
    return 0;
  endif
  
  return 1;
  
endfunction

//-----------------------------------------------------------------------------

function StdMercEventLoop()

  if(IsWalking())
    if(!IsWalkPause())
      WalkToDest();
    endif
  endif

  if(ReadGameClock() >= NextWander)
    DefWander();
    CheckMercRestock();
    CityNpcLookAround();
  endif

endfunction

//-----------------------------------------------------------------------------

function rapair(who,evaluateonly := 0)
  
  var MercCfg := GetNpcMercConfig(me);
  var skill := GetSkillIdByAttributeId(MercCfg.Attribute);

  if(skill == error)
    Say("Przykro mi, ale nie umiem nic naprawiac.");
    return;
  endif

  if(evaluateonly)
    Say("Co ocenic?");
  else
    Say("Co naprawic?");
  endif

  var Item := ObjTarget(who);

  if(!Item.isa(POLCLASS_ITEM))
    Say("*zdziwion" + ggm(me,4) + "*");
    sleep(1);
    Say("Tego chyba nie naprawiam");
    return;
  endif

  if(who.backpack != GetTopCont(Item))
    Say("Przedmiot musi byc w plecaku");
    return;
  endif

  if(!ReserveItem(Item))
    Say("Hmm, co z tym robisz?");
    return;
  endif

  Item.invisible := 1;

  TurnToward(who);
  TryToRepairItem(Item, who, skill, evaluateonly);

  Item.invisible := 0;
  // BARDZO WAZNE by zrobic release z AI! JA JEBIE!
  ReleaseItem(Item);

endfunction

//-----------------------------------------------------------------------------

function RapairAll(who,evaluateonly := 0)
  
  var MercCfg := GetNpcMercConfig(me);
  var skill := GetSkillIdByAttributeId(MercCfg.Attribute);

  if(skill == error)
    Say("Przykro mi, ale nie umiem nic naprawiac.");
    return;
  endif

  Say("Wskaz pojemnik z przedmiotami.");

  var Item := ObjTarget(who);

  if(!Item.isa(POLCLASS_ITEM))
    Say("*zdziwion" + ggm(me,4) + "*");
    sleep(1);
    Say("Tego nie naprawiam");
    return;
  elseif(!Item.isa(POLCLASS_CONTAINER))
    Say("Jak chcesz bym naprawil"+ggm(me,4)+" kilka przedmiotow, to wskaz pojemnik gdzie te przedmioty sa!");
    return;
  endif

  if(who.backpack != GetTopCont(Item) and Item != who.backpack)
    Say("Przedmiot musi byc w plecaku");
    return;
  endif

  TurnToward(who);

  var Price := CountRepairCostInContainer(who, Item, skill);

  if(!Price)
    Say("W tym pojemniku nie ma nic co bym mogl"+ggm(me,4)+" naprawic!");
    return;
  endif

  if(evaluateonly)

    case(RandomInt(3))
      0: Say("Naprawie to wszystko za " + Price +"gp.");
      1: Say("Wliczajac materialy i robocizne, naprawa tego wszystkiego bedzie kosztowala " +Price+"gp.");
      2: Say("Jak dla Ciebie to " + Price +"gp za te wszystkie przedmioty.");
    endcase

  else
    
    Say("Naprawa wszystkich przedmiotow bedzie kosztowac "+Price+" gp");
    Say("Czy mam kontynuowac?");

    if(!NpcYesNo(who))
      if(RandomInt(50) < 100)
        Say("Po co mi zawracasz glowe wiec");
      else
        Say("W takim razie przyjdz jak bedziesz bardziej zdecydowan"+ggm(who,6));
      endif
      return;
    endif

    foreach SubItem in EnumerateItemsInContainer(Item)
      if(ReserveItem(SubItem))
        SubItem.invisible := 1;
        var Cost := TryToRepairItem(SubItem, who, skill, 0, 1);
        SubItem.invisible := 0;

        if(Cost == -1)
          ReleaseItem(SubItem);
          break;
        endif
        
        ReleaseItem(SubItem);
      endif
    endforeach

  endif

endfunction

//-----------------------------------------------------------------------------

function CountRepairCostInContainer(who, Cont, skill)

  var Cost := 0;

  foreach Item in EnumerateItemsInContainer(Cont)
    var Price := TryToRepairItem(Item, who, skill, 2, 1); 
    Cost += Price;
  endforeach

  return Cost;

endfunction

//-----------------------------------------------------------------------------

function TryToRepairItem(Item, who, skill, evaluateonly, IgnoreNonRepair := 0)

  var anim,sound,ObjType;

  case(skill)
    SKILLID_BLACKSMITHY:
    SKILLID_ARMORER:
      anim    := 0x9;
      sound   := SFX_ANVIL;
      ObjType := GetIngObjByColor(Item.color);
      if(Item.objtype == UOBJ_PD_SHIELD or Item.objtype == UOBJ_DK_SHIELD or IsArtifact(Item) or GetBulkLevel(Item))
        ObjType := UOBJ_MYSTIC_INGOT;
      endif

    SKILLID_CARPENTRY:
      anim    := 0xA;
      sound   := 0x23E;
      var LogType  := GetItemMakeMod(Item);
      if(!LogType)
        LogType := 1;
      endif
      ObjType := GetBoardObjTypes()[LogType];
      if(IsArtifact(Item) or GetBulkLevel(Item))
        ObjType := UOBJ_MYSTICBOARD;
      endif

    SKILLID_BOWCRAFT:
      anim    := 0;
      sound   := 0x21;

      var LogType  := GetItemMakeMod(Item);
      if(!LogType)
        LogType := 1;
      endif
      ObjType := GetLogObjTypes()[LogType];
      if(IsArtifact(Item) or GetBulkLevel(Item))
        ObjType := UOBJ_MYSTICLOG;
      endif
    
    SKILLID_TAILORING:
      anim    := 0;
      sound   := SCISSORS_SOUND;

      var MainCfg := ReadConfigFile(":tailoring:tailoring");

      var Mat := SplitWords(MainCfg[Item.objtype].material);
      if(Mat[1] == "leathers")
        ObjType := GetLeatherObjByColor(Item.color);
      elseif(Mat[1] == "clothpiece")
        ObjType := UOBJ_TKANINA;
      else
        ObjType := GetItemObjType(Mat[1]);
      endif

      if(IsArtifact(Item) or GetBulkLevel(Item))
        ObjType := UOBJ_MYSTICLEATHER;
      endif

    SKILLID_TINKERING:
      anim    := 0;
      sound   := SFX_ANVIL;
      ObjType := GetTinkerMatObj(item);      

    default:
      if(!IgnoreNonRepair)
        if(evaluateonly != 2)
          Say("Przykro mi, ale nie umiem naprawic tego przedmiotu.");
        endif
      endif
      
      return 0;
  endcase 

  if(RepairItem(me, Item, skill, ObjType, sound, anim, RIF_CHECK_ONLY | __IfElse(IgnoreNonRepair, RIF_MULTI_OP, 0)))
    var lPrice := CDbl(GetItemSellPrice(Item));
    var Perc := GetItemMakeModPerc(Item);
    lPrice := (lPrice*0.75 + (lPrice * 2 * Perc)) / 2.0;
    var cost := CInt(200 + lPrice * (3.0 - CDbl(100.0*CDbl(GetItemHp(Item))/CDbl(GetItemMaxHp(Item))/33.33)));
    
    if(Item.objtype == UOBJ_PD_SHIELD or Item.objtype == UOBJ_DK_SHIELD or IsArtifact(Item) or GetBulkLevel(Item))
      cost := cost*5;
    elseif(IsMagicItem(Item))
      cost := CInt(cost*2.5);
    endif
  
    if(!evaluateonly)
      if(SpendGold(who, cost))
        if(!RepairItem(me, Item, skill, ObjType, sound, anim, __IfElse(IgnoreNonRepair, RIF_MULTI_OP, 0), who))
          return -1;
        endif

        if(IgnoreNonRepair)
          PrintTextAbovePrivate(me, "Przedmiot '"+Item.desc+"' naprawiony!", who);
        endif
      else
        if(IgnoreNonRepair)
          case(RandomInt(3))
            0: Say("Nie bede sie targowac, " + cost +"gp.");
               Say("Wroc jak bedziesz mial tyle zlota by naprawic '"+Item.desc+"'.");
            1: Say("Naprawa '"+Item.desc+"' kosztuje " + cost +"gp ani monety mniej."); 
               Say("Przyjdz jak bedziesz je miec.");
            2: Say("Nie stac Cie na moje uslugi.");
               Say("Naprawie '"+Item.desc+"' za " + cost +"gp.");
          endcase
        else
          case(RandomInt(3))
            0: Say("Nie bede sie targowac, " + cost +"gp. Wroc jak bedziesz mial tyle zlota.");
            1: Say("Naprawa kosztuje " + cost +"gp ani monety mniej. Przyjdz jak bedziesz je miec.");
            2: Say("Nie stac Cie na moje uslugi. Naprawie to za " + cost +"gp.");
          endcase
        endif

        return -1;
        
      endif

      return cost;

    elseif(evaluateonly == 2)

      return cost;

    else
      case(RandomInt(3))
        0: Say("Naprawie to za " + cost +"gp.");
        1: Say("Wliczajac materialy i robocizne, naprawa bedzie kosztowala " + cost +"gp.");
        2: Say("Jak dla Ciebie to " + cost +"gp.");
      endcase
      
      return cost;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
