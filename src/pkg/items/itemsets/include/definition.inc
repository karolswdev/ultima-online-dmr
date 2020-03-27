//-----------------------------------------------------------------------------

const GIDF_BASIC             := 0x0001;
const GIDF_MODS              := 0x0002;
const GIDF_EXTENDED          := 0x0004;
const GIDF_CHILDS            := 0x0008;
const GIDF_FULL              := 0x00FF;

// specjalna flaga, aby nie uwzglednial objtype, tylko sama grafike itema
const GIDF_STATIC            := 0x1000;
const GIDF_NO_LOCATION       := 0x2000;

//-----------------------------------------------------------------------------

function ListItemProps(Item)

  var List := GetObjPropertyNames(Item);
  var Props := dictionary;

  foreach Name in List

    if(Name[1] != "#" and Name != PROP_CREATED and Name != PROP_QUEST_ITEM and Name != PROP_OWNER)
      Props[Name] := GetObjProperty(Item, Name);
    endif

  endforeach

  return Props;

endfunction

//-----------------------------------------------------------------------------

function GetItemDefinition(Item, Flags := GIDF_FULL, MidLoc := error)

  var Data := struct;

  if(Flags & GIDF_BASIC)
    Data.+name    := GetName(Item, NAME_CLEAN_AMOUNT);
    Data.+color   := Item.color;

    if(Flags & GIDF_STATIC)
      Data.+objtype := Item.graphic;
    else
      Data.+objtype := Item.objtype;
      Data.+graphic := Item.graphic;
    endif
  endif

  if(Flags & GIDF_EXTENDED)
    Data.+amount  := Item.amount;
    Data.+props   := ListItemProps(Item);
    Data.+newbie  := Item.newbie;

    if(Item.isa(POLCLASS_CONTAINER))
      Data.+locked  := CInt(Item.locked);
    endif
  endif

  if(Flags & GIDF_MODS)
    if(Item.isa(POLCLASS_WEAPON))
      Data.+quality := Item.quality;
      Data.+dmg_mod := Item.dmg_mod;
    elseif(Item.isa(POLCLASS_ARMOR))
      Data.+quality := Item.quality;
      Data.+ar_mod  := Item.ar_mod;
    endif
  endif

  if(!Item.container and !(Flags & GIDF_NO_LOCATION))
    Data.+x := Item.x;
    Data.+y := Item.y;
    Data.+z := Item.z;

    if(MidLoc)
      Data.x -= MidLoc.x;
      Data.y -= MidLoc.y;
      Data.z -= MidLoc.z;
    endif
  endif

  if(Item.isa(POLCLASS_CONTAINER) and (Flags & GIDF_CHILDS))
    Data.+Items := array;
    FillItemContentDefinition(Data.Items, Item);
  endif

  return Data;

endfunction

//-----------------------------------------------------------------------------

function FillItemContentDefinition(byref Items, Cont)

  foreach Item in ListRootItemsInContainer(Cont)

    Items.append( GetItemDefinition(Item) );

  endforeach

endfunction

//-----------------------------------------------------------------------------
// Tworzenie 
//-----------------------------------------------------------------------------

function CreateItemFromDefinition(byref Res, Dest, ItemData)

  if(!Res.Items)
    Res := struct;
    Res.+Items := array;
  endif

  if(!ItemData.objtype or !ItemData.name)
    return error{"errortext" := "Nie prawidlowa definicja przedmiotu ["+ItemData+"]. Brak objtype lub name!"};
  endif

  var ItemDesc := GetItemDescriptor(ItemData.objtype);
  var bNoDescriptor := 1;

  if(ItemDesc)

    bNoDescriptor    := 0;

    ItemDesc.color   := ItemData.color;
    ItemDesc.graphic := ItemData.graphic;
    ItemDesc.newbie  := CInt(ItemData.newbie);

    foreach Prop in (ItemData.props)
      ItemDesc.CProps[_Prop_iter] := Prop;
    endforeach

  else
    ItemDesc := ItemData.objtype;
  endif

  var Item;
  
  if(Dest.isa(POLCLASS_CONTAINER))

    Item := CreateItemInCont(Dest, ItemDesc, ItemData.amount);

  elseif(!Dest.isa(POLCLASS_ITEM) and Dest.x and Dest.realm)
    if(ItemData.exists("x") and ItemData.exists("y"))
      Item := CreateItemAtLoc(Dest.x + ItemData.x, Dest.y + ItemData.y, Dest.z + ItemData.z, ItemDesc, ItemData.amount, Dest.realm);
      Item.movable := 0;
    else
      Item := CreateItemAtLoc(Dest.x, Dest.y, Dest.z, ItemDesc, ItemData.amount, Dest.realm);
    endif

  else
    return error{"errortext" := "Blednie podana docelowa lokacja przedmiotu! ["+Dest+"] "+ItemData.name+"/"+ItemData.objtype};
  endif

  if(!Item)
    return error{"errortext" := "Nie udalo sie stworzyc przedmiotu: "+ItemData.name+"/"+hex(ItemData.objtype)+": "+Item.errortext};
  endif


  if(!bNoDescriptor)
    Item.color   := ItemData.color;
    Item.graphic := ItemData.graphic;
    Item.newbie  := CInt(ItemData.newbie);

    foreach Prop in (ItemData.props)
      SetObjProperty(Item, _Prop_iter, Prop);
    endforeach
  endif

  if(Item.isa(POLCLASS_CONTAINER))
    Item.locked  := CInt(ItemData.locked);
  endif

  SetName(Item, ItemData.name+"%%");

  if(ItemData.quality)
    Item.quality := ItemData.quality;
  endif
  
  if(ItemData.dmg_mod)
    Item.dmg_mod := ItemData.dmg_mod;
  endif
  
  if(ItemData.ar_mod)
    Item.ar_mod := ItemData.ar_mod;
  endif
  
  if(ItemData.Items and ItemData.Items.size() > 0)
  
    foreach subData in (ItemData.Items)
      var cRes := CreateItemFromDefinition(Res, Item, subData);

      if(!cRes)
        return cRes;
      endif

    endforeach

  endif

  Res.Items.append(Item);

  return 1;

endfunction

//-----------------------------------------------------------------------------

function CreateItemsFromDefinition(Dest, byref Items)

  var Res;

  foreach ItemData in Items

    var cRes := CreateItemFromDefinition(Res, Dest, ItemData);

    if(!cRes)
      return cRes;
    endif

  endforeach

  return Res;

endfunction

//-----------------------------------------------------------------------------
