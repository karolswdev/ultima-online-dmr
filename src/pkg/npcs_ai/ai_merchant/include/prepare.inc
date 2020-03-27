//-----------------------------------------------------------------------------

include "const";
include "proc_event";
include "process";
include "combat";
include "push";
include "checks";
include "utils/misc";

include "include/findcity";
include "include/items";

//-----------------------------------------------------------------------------

var _AI_Merchant;

//-----------------------------------------------------------------------------

function AISys_Merchant_Init()

  SetBitOn(_AI_Data.sysFlags, AI_SYS_MERCHANT);

  AI_AddEvent(EVID_ITEM_GIVEN);

  return AISys_Merchant_Prepare();

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_Prepare()

  _AI_Merchant := struct;

  _AI_Merchant.+BuyItems     := dictionary;
  _AI_Merchant.+SellItems    := array;
  _AI_Merchant.+Amounts      := dictionary;
  _AI_Merchant.+MaxAmounts   := dictionary;
  _AI_Merchant.+City         := FindCity(Self());
  _AI_Merchant.+clockRestock := CInt(GetProperty(PROP_MERC_RESTOCK_TIME));
  _AI_Merchant.+clockPush    := ReadGameClock();

  if(!_AI_Merchant.clockRestock)
    _AI_Merchant.clockRestock := ReadGameClock();
  endif

  return AISys_Merchant_PrepareItems();

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_PrepareItems()

  var Npc     := Self();

  var MercCfg := ReadConfigFile(MRCSPAWN_CFG);
  var MercBox := GetMercBox(Npc);

  if(!MercBox)
    LogError("[AI] Wystapil blad podczas pobierania skrzynki dla merchanta! Npc: "+Npc.serial+", Template: "+Npc.npctemplate+" Error: "+MercBox);
    return 0;
  endif

  var Groups  := GetConfigStringArray(MercCfg[GetMercType(Npc)],"Buys");
  var ObjType;

  foreach Group in Groups
    foreach Item in GetConfigStringArray(MercCfg[Group],"Item")
      Item    := SplitWords(Item);
      ObjType := GetItemObjType(Item[1]);
      if(!ObjType)
        LogNotice("[DefineBuyItems] Bledny Buy ObjType w grupie ["+Group+"]: "+Item[1], LOGF_SCRIPTS);
        continue;
      endif
      _AI_Merchant.BuyItems[hex(ObjType)] := 1;
    endforeach
  endforeach

  // Define buy items

  Groups      := GetConfigStringArray(MercCfg[GetMercType(Npc)],"Sells");
  var Amounts := GetProperty(PROP_MERC_AMOUNTS);
  var MercItem;

  _AI_Merchant.SellItems := array;
  
  if(Amounts)
    _AI_Merchant.Amounts := Amounts;
  else
    _AI_Merchant.Amounts := dictionary;
  endif

  foreach Group in Groups
    foreach Item in GetConfigStringArray(MercCfg[Group],"Item")
      Item    := SplitWords(Item);
      ObjType := GetItemObjType(Item[1]);
      if(!ObjType)
        LogNotice("[DefineItems] Bledny Sell ObjType w grupie ["+Group+"]: "+Item[1], LOGF_SCRIPTS);
        continue;
      endif

      MercItem := FindItemInMercBox(MercBox, ObjType);

      if(MercItem and !(MercItem in _AI_Merchant.SellItems))
        if(!_AI_Merchant.Amounts.exists(hex(MercItem.objtype)))
          _AI_Merchant.Amounts[hex(MercItem.objtype)] := CInt(Item[2]);
        elseif(CInt(_AI_Merchant.Amounts[hex(MercItem.objtype)]) > CInt(Item[2]))
          _AI_Merchant.Amounts[hex(MercItem.objtype)] := CInt(Item[2]);
        endif
        _AI_Merchant.MaxAmounts[hex(MercItem.objtype)] := CInt(Item[2]);
        _AI_Merchant.SellItems.append( MercItem );
      endif
    endforeach
  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------
