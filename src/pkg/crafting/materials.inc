//-----------------------------------------------------------------------------

function DefineCraftMaterials()

  var Materials := struct;
  
  Materials.+sumAmount := 0;
  Materials.+items     := array;
  Materials.+maxIterAmount := CRAFT_MAX_AMOUNT;

  return Materials;

endfunction

//-----------------------------------------------------------------------------

function DefineCraftMaterialsFromCfg(who, cfgElem, elemName, byref Cont)

  var Materials := DefineCraftMaterials();
  var Res := AddCraftMaterialsFromCfg(Materials, who, cfgElem, elemName, Cont);
    
  if(Res == error)
    SendSysMessage(who, "Materialy nie sa skompletowane...", FONT_NORMAL, COLOR_BAD);
    return 0;
  elseif(!Res)
    return 0;
  endif

  return Materials;

endfunction

//-----------------------------------------------------------------------------

function DefineMainMaterial(byref Materials, MainItem)

  Materials.+mainMat := MainItem;
  Materials.+mainObj := MainItem.objtype;

  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  if(ItemCfg[MainItem.objtype].MaterialColor)
    Materials.+mainColor := CInt(ItemCfg[MainItem.objtype].MaterialColor);
  else
    Materials.+mainColor := MainItem.color;
  endif

  if(ItemCfg[MainItem.objtype].MaterialSuffix)
    Materials.+mainSuffix := ItemCfg[MainItem.objtype].MaterialSuffix;
  else
    Materials.+mainSuffix := 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function AddCraftMaterialsFromCfg(byref Materials, who, cfgElem, elemName, byref Cont)

  var List := GetConfigStringArray(cfgElem, elemName);

  if(!List or !List.size())
    LogError("AddCraftMaterialsFromCfg: Probowano pobrac materialy z grupy, ktora nie ma ich zdefiniowanych! Nazwa wlasciwosci: ["+elemName+"]");
    return error;
  endif

  foreach Entry in List
    
    Entry := SplitWords(Entry);

    if(!Entry[1])
      LogError("AddCraftMaterialsFromCfg: Blednie zdefiniowany ObjType materialu ["+Entry[1]+"] dla wlasciwosci "+elemName);
      return error;
    endif

    var Amount  := Max(CInt(Entry[2]), 1);
    var Type    := Entry[3];
    var Flags   := 0;
    var ObjType;

    if(Entry[3] == "main")
      Flags := Flags | SCMGF_FORCE_CHOOSE;
    endif

    var TgtMat := SelectCraftMatFromGroup(who, Entry[1], Flags);

    if(!TgtMat)
      return TgtMat;
    endif

    if(TgtMat.isa(POLCLASS_ITEM))
      if(Type == "main")
        DefineMainMaterial(Materials, TgtMat);
      endif
      ObjType := TgtMat.objtype;

      if(TgtMat.container.isa(POLCLASS_ITEM))
        Cont := TgtMat.container;
      endif
    else
      ObjType := TgtMat;
    endif

    if(!AddCraftMaterial(Materials, Cont, ObjType, Amount, __IfElse(TgtMat.isa(POLCLASS_ITEM), TgtMat.color, -1)))
      LogError("AddCraftMaterialsFromCfg: Blednie zdefiniowany material: "+ObjType+" w grupie materialow: "+elemName);
      return error;
    endif

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function ModifyCraftMaterialAmounts(byref Materials, modAmount)

  Materials.sumAmount := 0;
  
  foreach Material in (Materials.items)
    Material.amount := Max(CInt(Material.amount * modAmount), 1);
    Material.left := Max(CInt(Material.left * modAmount), 1);
    Materials.sumAmount += Material.amount;
  endforeach

endfunction 

//-----------------------------------------------------------------------------

function GetCraftProductDesc(ObjType, Amount)

  var Desc := GetDescForItem(ObjType);

  if(!Desc)
    Desc := "przedmiot";
  endif

  if(Amount > 1)
    return Amount + " " + Desc;
  else
    return Desc;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetCraftMaterialDesc(ObjType, Amount)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var Desc := GetDescForItem(ObjType);

  if(!Desc)
    Desc := "nieznany";
  endif

  if(ItemCfg[ObjType].MaxUses)
    return Amount + " "+GetPortionDesc(ObjType, Amount)+" " + Desc;
  elseif(Amount)
    return Amount + " " + Desc;
  else
    return Desc;
  endif

endfunction

//-----------------------------------------------------------------------------

function AddCraftMaterial(byref Materials, Cont, ObjType, Amount, Color := -1)

  if(!ObjType)
    return RetError("Blednie zdefiniowany skladnik!");
  endif

  var Items := FindItemsInContainer(Cont, ObjType, 0, Color);

  foreach Item in Items
    ReserveItem(Item);
  endforeach

  var Material := struct;
  Material.+items   := Items;
  Material.+amount  := Amount;
  Material.+objtype := ObjType;
  Material.+left    := Amount;

  Materials.sumAmount += Amount;
  Materials.items.append(Material);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function AddCraftMaterialItem(byref Materials, Item, Amount)

  var Material := struct;
  Material.+items   := array(Item);
  Material.+amount  := Amount;
  Material.+objtype := Item.objtype;
  Material.+left    := Amount;

  Materials.sumAmount += Amount;
  Materials.items.append(Material);

  ReserveItem(Material);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetProductAmountForMaterials(byref Materials)

  var ProdAmount := -1;

  foreach Material in (Materials.items)
  
    var ItemsAmount := GetSumItemsUses(Material.items);
    var MatAmount := CInt(ItemsAmount / Material.amount);
    
    if(ProdAmount == -1)
      ProdAmount := MatAmount;
    else
      ProdAmount := Min(MatAmount, ProdAmount);
    endif

  endforeach

  return ProdAmount;

endfunction

//-----------------------------------------------------------------------------

function CheckCraftMaterials(byref Materials, byref ProdAmount)

  if(!Materials.items.size())
    return RetError("Ups, lista materialow nie zostala spisana!");
  endif

  var AvailAmount := GetProductAmountForMaterials(Materials);

  ProdAmount := MinMax(ProdAmount, 1, Max(AvailAmount, 1));

  foreach Material in (Materials.items)
  
    var ItemsAmount := GetSumItemsUses(Material.items);
    var FinalAmount := Material.amount * ProdAmount;

    if(!Material.items.size())
      return RetError("Potrzebujesz "+GetCraftMaterialDesc(Material.objtype, FinalAmount)+".");
    elseif(ItemsAmount < FinalAmount)
      return RetError("Brakuje Ci "+GetCraftMaterialDesc(Material.objtype, (FinalAmount - ItemsAmount))+".");
    endif

  endforeach

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetCraftMaterialList(byref Materials)

  var List := array;

  foreach Material in (Materials.items)
    List.append(GetCraftMaterialDesc(Material.objtype, Material.amount));
  endforeach

  return List;

endfunction

//-----------------------------------------------------------------------------

function SendCraftMaterialList(who, byref Materials)

  var List := GetCraftMaterialList(Materials);

  if(List.size())
    SendSysMessage(who, "Lista materialow:", FONT_NORMAL, COLOR_GREEN);

    foreach Entry in List
      SendSysMessage(who, Entry, FONT_NORMAL, COLOR_ORANGE);
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------

function ResetCraftMaterials(byref Materials, ProdAmount := 1)

  ProdAmount := Max(ProdAmount, 1);

  foreach Material in (Materials.items)
    Material.left := Material.amount * ProdAmount;
  endforeach

endfunction

//-----------------------------------------------------------------------------

function SubtractFromItems(byref Items, ObjType, Amount)

  var ItemsAmount := GetSumItemsUses(Items);

  if(ItemsAmount < Amount)
    return RetError("Brakuje Ci "+GetCraftMaterialDesc(ObjType, Amount)+".");
  endif

  foreach Item in (Items)

    if(!Item)
      continue;
    endif

    var subAmount := Min(Amount, GetItemUses(Item));
  
    if(!SubItemUses(Item, subAmount))
      return RetError("Zabraklo Ci "+GetCraftMaterialDesc(ObjType, subAmount)+".");
    endif

    Amount -= subAmount;

    if(Amount == 0)
      return 1;
    endif

  endforeach

  return RetError("Zabraklo Ci "+GetCraftMaterialDesc(ObjType, Amount)+".");

endfunction

//-----------------------------------------------------------------------------

function SubtractCraftMaterial(byref Materials, Amount)

  if(!Materials.items)
    return RetError("Ops, cos jest nie tak z lista materialow!");
  endif

  foreach Material in (Materials.items)

    if(Material.left > 0)
      
      var subAmount := Min(Amount, Material.left);
      var Res := SubtractFromItems(Material.items, Material.objtype, subAmount);

      if(!Res)
        return Res;
      endif

      Material.left -= subAmount;
      Amount -= subAmount;
    endif

    if(Amount == 0)
      break;
    endif

  endforeach

  if(Amount != 0)
    return RetError("Zabraklo Ci materialow!");
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetMaterialsTimesForTool(byref Materials)

  return Max(CInt(Materials.sumAmount * 0.18), 1);

endfunction

//-----------------------------------------------------------------------------
