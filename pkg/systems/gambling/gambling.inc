//-----------------------------------------------------------------------------

include ":magicitems:magicitems";

//-----------------------------------------------------------------------------

const GAMBLER_RESET_MIN_TIME   := 7200;
const GAMBLER_RESET_MAX_TIME   := 14400;
const GAMBLE_PLAYER_TIME       := 21600;
const GAMBLE_PLAYER_MAX_COUNT  := 3;

const GAMBLER_MIN_ITEMS        := 8;
const GAMBLER_MAX_ITEMS        := 16;

const GAMBLER_MIN_PRICE        := 40000;
const GAMBLER_MAX_PRICE        := 90000;

//-----------------------------------------------------------------------------

function GetRandomGambleItems()

  var Items := array;
  var i := 1;
  var Size := GAMBLER_MIN_ITEMS + RandomInt((GAMBLER_MAX_ITEMS - GAMBLER_MIN_ITEMS) + 1);
  var Cnt := 0;

  while(i <= Size and Cnt < 100)
    var ObjType := GetRandomGambleItem();

    if(ObjType)
      if(CanBeMagicObjType(ObjType))
        var Price := GAMBLER_MIN_PRICE + (CInt(RandomInt(GAMBLER_MAX_PRICE - GAMBLER_MIN_PRICE) / 10) * 10);

        var Item  := struct;
        Item.+objtype := ObjType;
        Item.+price   := Price;
        Item.+desc    := GetDescForItem(ObjType);
        Items.Append(Item);
        i := i + 1;
      endif
    endif

    Cnt := Cnt + 1;
  endwhile

  return Items;

endfunction

//-----------------------------------------------------------------------------

function GetRandomGambleItem()

  var GroupCfg  := ReadConfigFile(ITEMGROUPS_CFG);
  var GroupName := GetGamblerGroupName();
  var Elem      := GroupCfg[GroupName];

  if(!Elem)
    LogError("Gambler: Grupa przedmiotow "+GroupName+" nie istnieje!");
    return 0;
  endif

  var Items     := GetConfigStringArray(Elem, "Item");
  var Idx       := RandomInt(Items.size())+1;
  var Item      := Items[Idx];

  if(Item)
    Item := SplitWords(Item);
    return GetObjTypeByName(Item[2]);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetGamblerGroupName()

  var GroupCfg := ReadConfigFile(ITEMGROUPS_CFG);
  var Keys     := GetConfigStringKeys(GroupCfg);
  var Name, Names := array;

  foreach Key in Keys
    
    if(GroupCfg[Key].CanGamble)
      Names.append(Key);
    endif

  endforeach

  Name := Names[RandomInt(Names.size())+1];

  return Name;

endfunction

//-----------------------------------------------------------------------------

function CanPlayerGamble(who)

  var LastTime  := CInt(GetObjProperty(who, "LastGamble"));
  var ItemsCnt  := CInt(GetObjProperty(who, "GambleCount"));

  if(LastTime + GAMBLE_PLAYER_TIME < ReadGameClock())
    SetObjProperty(who, "LastGamble", ReadGameClock());
    ItemsCnt := 0;
    SetObjProperty(who, "GambleCount", ItemsCnt);
  endif

  if(ItemsCnt >= GAMBLE_PLAYER_MAX_COUNT)
    var Res := error;
    Res.+errortext := "Nie mozesz kupic wiecej przedmiotow. Musisz odczekac pare godzin, nie mozna tracic glowy!";
    return Res;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function IncPlayerGambleCount(who)

  SetObjProperty(who, "GambleCount", CInt(GetObjProperty(who, "GambleCount")) + 1);

endfunction

//-----------------------------------------------------------------------------

function CreateGambleItem(who, ItemDef)

  var Item := CreateItemInPack(who, ItemDef.objtype);

  var MagicLvl := 9;

  if(RandomInt(100) < 60)
    MagicLvl := MagicLvl + RandomInt(4);
  endif

  if(RandomInt(100) < 40)
    MagicLvl := MagicLvl + 2 + RandomInt(3);
  endif

  if(RandomInt(100) < 14)
    MagicLvl := 16;
  endif

  if(RandomInt(100) < 4)
    MagicLvl := 18;
  endif
  
  if(TryToMakeItemMagic(Item, 85, MagicLvl))
    IdentifyMagicItem(who, Item);
    SendSysMessage(who, "Trafil"+ggm(who,3)+" magiczny przedmiot "+Item.desc+"!", FONT_NORMAL, COLOR_WHITE);
    SendMagicItemDesc(who, Item);

    if(MagicLvl == 16)
      SendSysMessage(who, "Trafil"+ggm(who,3)+" niezwyklej jakosci przedmiot!", FONT_NORMAL, COLOR_ORANGE);
    endif

    if(MagicLvl == 18)
      SendSysMessage(who, "Trafil"+ggm(who,3)+" przedmiot cudownej jakosci!", FONT_NORMAL, COLOR_ORANGE);
    endif

  else
    SendSysMessage(who, "Trafil"+ggm(who,3)+" zwykly przedmiot "+Item.desc+".", FONT_NORMAL, COLOR_GREEN);
  endif

  return Item;

endfunction

//-----------------------------------------------------------------------------
