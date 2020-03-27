//-----------------------------------------------------------------------------

include "include/getobj";
include "include/tools";
include "include/items";
include "include/repair";

//-----------------------------------------------------------------------------

const BULK_SK_NEEDED          := 80;
const BULK_TIME_COUNTER       := 300;
const BULK_MIN_COUNT          := 200000; // musi sprzedac za przynajmniej 200k
const BULK_MAX_COUNT          := 400000; // szansa zwieksza sie do 400k 
const BULK_CHANCE             := 30; // szansa interpolowana od MIN_COUNT do MAX_COUNT.
const BULK_EXP_CHANCE         := 90;
const BULK_MAX_ONETIME        := 5000;

const UOBJ_BULK               := 0xa600;

const ORDERS_CFG              := ":bulks:orders";
const REW_CFG                 := ":bulks:rewards";

const PROP_BULK_DATA          := "BulkData";

//-----------------------------------------------------------------------------

function GetBulkData(Npc, who)

  var Bulks := GetObjProperty(Npc, PROP_BULK_DATA);
  var Bulk  := array(ReadGameClock(), 0, 0, 0);

  if(Bulks)
    
    if(Bulks.exists(who.serial))
      Bulk := Bulks[who.serial];
      
      while(Bulk.size() < 6)
        Bulk[Bulk.size()+1] := 0;
      endwhile
    endif

  endif

  return Bulk;

endfunction

//-----------------------------------------------------------------------------

function SetBulkData(Npc, who, Bulk)

  var Bulks := GetObjProperty(Npc, PROP_BULK_DATA);

  if(!Bulks or !Bulks.size())
    Bulks := dictionary;
  endif

  Bulks[who.serial] := Bulk;

  SetObjProperty(Npc, PROP_BULK_DATA, Bulks);

endfunction

//-----------------------------------------------------------------------------

function SpawnBulk(Npc,who,SkID)

  PrintTextAbovePrivate(Npc,"Psst...",who);
  PrintTextAbovePrivate(Npc,"Mam zamowienie dla Ciebie, jestes zainteresowan"+ggm(who,6)+"?",who);
  
  CreateBulk(Npc, who, SkID);

endfunction

//-----------------------------------------------------------------------------

function CreateBulk(Npc,who,SkID)

  var Skill := GetBaseSkill(who,SkID);
  
  if(Skill >= 100)
    Skill := 100;
  elseif(Skill < 80)
    Skill := 80;
  endif
  
  var Lvl     := CInt((Skill - 75) / 5);
  var BulkOrd := PickBulkOrder(SkID,Lvl);

  if(BulkOrd)
    BulkOrd[5] := who.serial;
    var Bulk := CreateItemInPack(who,UOBJ_BULK);
    SetObjProperty(Bulk, "BulkOrder", BulkOrd);
    SetName(Bulk, Bulk.desc+" u "+Npc.name);
    SetObjProperty(Bulk, "BulkNpc", Npc.serial);
  endif

endfunction

//-----------------------------------------------------------------------------

function PickBulkOrder(SkID,Lvl)

  var BOCfg   := ReadConfigFile(ORDERS_CFG);
  var BElem   := BOCfg[SkID];
  var Group   := GetConfigStringArray(BOCfg[BElem.Items],"Item");
  
  var Item    := SelectRandom(Group,Lvl);
  
  if(!Item)
    Item := Group[1];
  endif
  
  Item := CInt(Item);
  
  var ItemProps := GenerateItemProps(BElem.Mods,Item,Lvl);
  
  return {Item,ItemProps,Lvl,SkID};
  
endfunction

//-----------------------------------------------------------------------------

function GenerateItemProps(Mods,Item,Lvl)

  var ItemName  := "";
  var Rand      := 0;
    
  case(Mods)
    "IngotMods": var Items := GetIngotsObjTypes();
                 Items.erase(1);
                 Rand := SelectRandom(Items,Lvl);
                 ItemName  := GetDescForItem(Item) + " " + GetDpsType(Rand) + " " + GetDps(Rand,Item);
    "HideMods":  var Items := GetLeatherObjTypes();
                 Items.erase(1);
                 Rand := SelectRandom(Items,Lvl);
                 ItemName  := GetDps(Rand,Item) + " " + GetDescForItem(Item);
    "LogMods":   var Items := GetBoardObjTypes();
                 Items.erase(1);
                 Rand := SelectRandom(Items,Lvl);
                 ItemName  := GetDps(Rand,Item) + " " + GetDescForItem(Item);
  endcase

  var Exp       := 0;

  if(Lvl >= 4 and RandomInt(100) < BULK_EXP_CHANCE)
    Exp := 1;
  endif

  return {ItemName,Rand,Exp};

endfunction

//-----------------------------------------------------------------------------

function SelectRandom(Group,Lvl)

  var Minim := CInt(Group.size() / 4) * (Lvl - 1);
  var Rnd;
  
  if(Lvl == 4)
    Rnd := Group.size() - Minim;
  else
    Rnd := CInt(Group.size() / 4);
  endif

  var Rand,M := 0;
  
  while(!Rand and M < 30)
    Rand := Group[Minim + (RandomInt(Rnd)+1)];
    M := M + 1;
  endwhile

  if(!Rand)
    Rand := Group[Minim];
  endif
  
  return Rand;

endfunction

//-----------------------------------------------------------------------------

function CreateReward(who, Lvl, SkID)

  var RewCfg := ReadConfigFile(REW_CFG);

  var Items  := GetConfigStringArray(RewCfg[SkID],"Item");
  var Item;
  
  if(Lvl >= 5)
	var glyph := CreateItemInPack(who, 0x3391, 1);
	if(glyph)
		SendSysMessage(who, "Za Twoje trudy glif pojawil sie w Twoim plecaku.", FONT_NORMAL, COLOR_BLUE);
	endif
  endif
  
  
  if(GetGlobalProperty("XmasTime"))
	if(Lvl == 4)
		var bonusItem;
		if(HaveChance(85))
			bonusItem := CreateItemInPack(who, 0x22c7, 1);
				if(bonusItem)
					SendSysMessage(who, "Za Twoje trudy podczas tego magicznego czasu otrzymales prezent.", FONT_NORMAL, COLOR_GREEN);
				endif
		endif
		
		if(HaveChance(15))
			bonusItem := CreateItemInPack(who, 0x22c8, 1);
				if(bonusItem)
					SendSysMessage(who, "Za Twoje trudy podczas tego magicznego czasu otrzymales sredni prezent.", FONT_NORMAL, COLOR_GREEN);
				endif
		endif
		
	endif
	
	if(Lvl >= 5)
		var bonusItem;
		if(HaveChance(80))
			bonusItem := CreateItemInPack(who, 0x22c8, 1);
			if(bonusItem)
				SendSysMessage(who, "Za Twoje trudy podczas tego magicznego czasu otrzymales sredni prezent.", FONT_NORMAL, COLOR_GREEN);
			endif
		endif
		
		if(HaveChance(20))
			bonusItem := CreateItemInPack(who, 0x22c9, 1);
			if(bonusItem)
				SendSysMessage(who, "Za Twoje potezne trudy otrzymales duzy prezent.", FONT_NORMAL, COLOR_GREEN);
				PrintTextAbove(who, "Magia swiat!", FONT_NORMAL, COLOR_GREEN);
			endif
		endif
	endif
  endif
  
  while(!Item)
    Item := Items[RandomInt(Items.size())+1];
  endwhile
  
  Item := SplitWords(Item);
  
  if(CInt(Item[2]) > 1)
    Item[2] := Lvl * Cint(Item[2]);
  endif
  
  var cItem := CreateItemInPack(who,CInt(Item[1]),CInt(Item[2]));
  
  if(CInt(Item[3]))
    cItem.color := CInt(Item[3]);
  elseif(Item[3] != "-")
    var CGrp := GetConfigStringArray(RewCfg[Item[3]],"color");
    if(CGrp.size() and CGrp)
      Item[3] := 0;
      while(!Item[3])
        Item[3] := CGrp[RandomInt(CGrp.size())+1];
      endwhile
    endif
  endif
  
  if(CInt(Item[3]))
    cItem.color := CInt(Item[3]);
  endif
  
  if(Item[4] != "-" and Item[4])
    SetName(cItem,Item[4] + " " + cItem.desc);
  endif

  if(Item[5] != "-" and Item[5])
    SetName(cItem,cItem.desc + " " + Item[5]);
  endif
  
  if(Item[6] != "-")
  
    var DurMod := Lvl * 6;

    case(Item[6])
       "IngotDur":
       "LogDur":   Lvl := Max(Lvl - 1, 1);

       "IngotMod":
       "LogMod":   DurMod := CInt(DurMod / 2);
    endcase

    var Mod := CInt(Lvl * 4.5);

    SetObjProperty(cItem, "BulkLevel", Lvl);

    if(cItem.isa(POLCLASS_WEAPON))
      cItem.dmg_mod := Mod;
    endif

    case(Item[6])

      "IngotDur": 
      "IngotMod": SetMade(0, cItem, MTYPE_INGOT, Mod);
                  SetItemQuality(cItem, 1.5);
                  SetItemBulkDur(cItem, DurMod);

      "LogDur":   
      "LogMod":   SetMade(0, cItem, MTYPE_LOG, Mod);
                  SetItemQuality(cItem, 1.5);
                  SetItemBulkDur(cItem, DurMod);

    endcase
    
    SetItemHp(cItem, GetItemMaxHp(cItem));
  endif

endfunction

//-----------------------------------------------------------------------------

function SetItemBulkDur(cItem, Mod)

  SetItemMaxHpMod(cItem, Mod);

endfunction

//-----------------------------------------------------------------------------

function CheckBulkItemName(BulkItem,Item)

  var IName := lower(Item.desc);
  var BulkName := lower(BulkItem[2][1]);
  
  if(IName[BulkName])
    if(BulkItem[2][3])
      if(IName["wyjatkowej jakosci"])
        return 1;
      else
        return -1;
      endif
    else
      return 1;
    endif
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------
