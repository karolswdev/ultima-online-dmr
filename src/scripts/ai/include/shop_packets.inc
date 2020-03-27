//=============================================================================
//-----------------------------------------------------------------------------
// Merchant Buy/Sell Packets
//-----------------------------------------------------------------------------
//=============================================================================

include "include/packets";

//-----------------------------------------------------------------------------

function PrepareShopPcItemData(byref iData)

  iData := struct;

  iData.+serial;
  iData.+graphic;
  iData.+price;
  iData.+amount;
  iData.+desc;
  iData.+color;

endfunction

//-----------------------------------------------------------------------------

function AddItemToContPc(who, byref ContPc, byref iData, MercBox)

  var Cnt := ContPc.GetInt16(3);
  ContPc.SetInt16(3,Cnt+1);

  if(IsKRPacketType(who))
    Cnt := 5+(Cnt*PC_CONT_ITM_LEN_KR);

    ContPc.SetInt32(Cnt   , iData.serial);
    ContPc.SetInt16(Cnt+4 , iData.graphic);
    ContPc.SetInt8 (Cnt+6 , 0);
    ContPc.SetInt16(Cnt+7 , iData.amount);
    ContPc.SetInt16(Cnt+9 , 0);
    ContPc.SetInt16(Cnt+11, 1);
    ContPc.SetInt8 (Cnt+13, 0);
    ContPc.SetInt32(Cnt+14, MercBox.serial);
    ContPc.SetInt16(Cnt+18, iData.color);
  else
    Cnt := 5+(Cnt*PC_CONT_ITM_LEN);

    ContPc.SetInt32(Cnt   , iData.serial);
    ContPc.SetInt16(Cnt+4 , iData.graphic);
    ContPc.SetInt8 (Cnt+6 , 0);
    ContPc.SetInt16(Cnt+7 , iData.amount);
    ContPc.SetInt16(Cnt+9 , 0);
    ContPc.SetInt16(Cnt+11, 1);
    ContPc.SetInt32(Cnt+13, MercBox.serial);
    ContPc.SetInt16(Cnt+17, iData.color);
  endif

endfunction

//-----------------------------------------------------------------------------

function AddItemToShopPc(byref ShopPc, byref iData)

  ShopPc.SetInt32(ShopPc.GetSize(), iData.price);
  ShopPc.SetInt8 (ShopPc.GetSize(), len(iData.desc)+1);
  ShopPc.SetString(ShopPc.GetSize(), iData.desc, 1);

endfunction

//-----------------------------------------------------------------------------

function SendEquipPackets(who, Npc, MercBox, OldBox)

  var cEquip := CreatePacket(0x2e,15);

  cEquip.SetInt32(1,MercBox.serial);
  cEquip.SetInt16(5,MercBox.graphic);
  cEquip.SetInt16(7,LAYER_VENDORSTOCK);
  cEquip.SetInt32(9,Npc.serial);
  cEquip.SetInt16(13,0x0);

  cEquip.SendPacket(who);

  cEquip := CreatePacket(0x2e,15);
  
  cEquip.SetInt32(1,OldBox.serial);
  cEquip.SetInt16(5,OldBox.graphic);
  cEquip.SetInt16(7,LAYER_VENDOREXTRA);
  cEquip.SetInt32(9,Npc.serial);
  cEquip.SetInt16(13,0x0);

  cEquip.SendPacket(who);
  
endfunction

//-----------------------------------------------------------------------------

function FillItemDataShopPc(byref iData, Item, Amt, Price)

  iData.serial  := Item.serial;
  iData.graphic := Item.graphic;
  iData.color   := Item.color;
  iData.desc    := Item.desc;
  iData.amount  := Amt;
  iData.price   := Price;

  var Flag := GDFIF_ONE_AMT;
  if(iData.amount > 1)
    Flag := 0;
  endif

  ClearItemDescSuffix(iData.desc, Flag);

endfunction

//-----------------------------------------------------------------------------

function SendBuyPacket(who, Npc, MercBox, OldBox, byref sItems)

  if(!IsPlayer(who))
    return error;
  endif

  if(who.dead or Distance(Npc, who) > 2 or !CheckLineOfSight(Npc, who) or DistZ(Npc, who) >= 20)
    return error;
  endif

  if(!sItems.size() or !sItems)
    return 0;
  endif
  
  TurnToward(who);
  
  SendEquipPackets(who, Npc, MercBox, OldBox);

  var ContPc := CreatePacket(0x3c,MSGLEN_VARIABLE);
  var ShopPc := CreatePacket(0x74,MSGLEN_VARIABLE);
  var OpenPc := CreatePacket(0x24,7);
  var Cnt    := 0;
  var iData, RevItem;

  PrepareShopPcItemData(iData);

  ContPc.SetInt16(3,0);
  ShopPc.SetInt32(3,MercBox.serial);
  ShopPc.SetInt8 (7,0);

  foreach Item in sItems
    if(Item.size() != 3)
      continue;
    endif

    RevItem := sItems[sItems.size()-(_Item_iter-1)];

    FillItemDataShopPc(iData, RevItem[1], RevItem[2], Item[3]);
    AddItemToContPc(who, ContPc, iData, MercBox);
    AddItemToShopPc(ShopPc, iData);
    SendClilocUpdate(who, RevItem[1]);
    Cnt := Cnt + 1;

  endforeach

  if(!Cnt)
    return 0;
  endif
  
  ShopPc.SetInt8(7,Cnt);

  OpenPc.SetInt32(1,Npc.serial);
  OpenPc.SetInt16(5,0x30);

  ContPc.SendPacket(who);
  ShopPc.SendPacket(who);

  ContPc := CreatePacket(0x3c,MSGLEN_VARIABLE);
  ShopPc := CreatePacket(0x74,MSGLEN_VARIABLE);

  ContPc.SetInt16(3,0);
  ShopPc.SetInt32(3,OldBox.serial);
  ShopPc.SetInt8(7,0);

  ContPc.SendPacket(who);
  ShopPc.SendPacket(who);
  OpenPc.SendPacket(who);

  UpdateStatus(who);

  return Cnt;

endfunction

//-----------------------------------------------------------------------------

function SendSellPacket(who, Npc, MercBox, OldBox, byref sItems)

  if(!IsPlayer(who))
    return error;
  endif

  if(who.dead or Distance(Npc, who) > 2 or !CheckLineOfSight(Npc, who) or DistZ(Npc, who) >= 20)
    return error;
  endif

  if(!sItems.size() or !sItems)
    return 0;
  endif

  TurnToward(who);
  SendEquipPackets(who, Npc, MercBox, OldBox);

  var SellPc := CreatePacket(0x9e,MSGLEN_VARIABLE);
  
  SellPc.SetInt32(3, Npc.serial);
  
  var Cnt    := 0;
  var Pos    := 9;
  var Desc;
  
  foreach Item in sItems
    if(Item.size() != 3 or !Item[1])
      continue;
    endif

    Desc := CutItemDesc(Item[1]);

    SellPc.SetInt32(Pos   ,Item[1].serial);
    SellPc.SetInt16(Pos+4 ,Item[1].graphic);
    SellPc.SetInt16(Pos+6 ,Item[1].color);
    SellPc.SetInt16(Pos+8 ,Item[2]);
    SellPc.SetInt16(Pos+10,Item[3]);
      
    SellPc.SetInt16(Pos+12,len(Desc));
    SellPc.SetString(Pos+14,Desc,0);
    Cnt := Cnt + 1;
    Pos := Pos + 14 + len(Desc);
    SendClilocUpdate(who, Item[1]);
  endforeach
  
  if(!Cnt)
    return 0;
  endif

  SellPc.SetInt16(7,Cnt);
  SellPc.SendPacket(who);

  return Cnt;

endfunction

//-----------------------------------------------------------------------------
