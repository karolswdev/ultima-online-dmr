//-----------------------------------------------------------------------------

include "include/const/props";

//-----------------------------------------------------------------------------
// who moze byc graczem badz npc
//-----------------------------------------------------------------------------

const RIF_CHECK_ONLY := 0x01;
const RIF_MULTI_OP   := 0x02;

//-----------------------------------------------------------------------------

function RepairItem(who, Item, SkID, MatObj, Sound := 0, Anim := 0, Flags := 0, Source := 0, RepairAmt := 1)

  var ItemCfg  := ReadConfigFile(ITEMS_CFG);

  if(!ItemCfg[Item.objtype].repairable or !GetItemMaxHp(Item))
    if(IsPlayer(who))
      SendSysMessage(who,"Tego sie nie da naprawic.",FONT_NORMAL,COLOR_RED);
    elseif(!IsBitOn(Flags, RIF_MULTI_OP))
      PrintTextAbove(who,"Tego sie nie da naprawic.");
    endif
    return 0;
  endif

  if(CInt(ItemCfg[Item.objtype].repairable) != SkID)
    if(IsPlayer(who))
      SendSysMessage(who,"Tak tego nie naprawisz.",FONT_NORMAL,COLOR_RED);
    elseif(!IsBitOn(Flags, RIF_MULTI_OP))
      PrintTextAbove(who,"Tego nie potrafie naprawic.");
    endif
    return 0;
  endif

  if(!GetItemHp(Item))
    if(IsPlayer(who))
      SendSysMessage(who,"Zniszczyl"+ggm(who,3)+" ten przedmiot.",FONT_NORMAL,COLOR_RED);
    else
      PrintTextAbove(who, "Niestety, ale przedmiot "+GetDescForItem(Item)+" sie rozpadl... sam.");
    endif
    DestroyItem(Item);
    return 0;
  endif
  
  MatObj := CInt(MatObj);

  if(!MatObj or GetObjProperty(Item, PROP_UNREPAIRABLE))
    if(IsPlayer(who))
      SendSysMessage(who,"Nie mozesz tego naprawic.",FONT_NORMAL,COLOR_RED);
    elseif(!IsBitOn(Flags, RIF_MULTI_OP))
      PrintTextAbove(who,"Nie da sie tego naprawic");
    endif
    return 0;
  endif

  if(GetItemHp(Item) > GetItemMaxHp(Item))
    SetItemHp(Item,GetItemMaxHp(Item));
  endif
  
  if(GetItemHp(Item) == GetItemMaxHp(Item))
    if(IsPlayer(who))
      SendSysMessage(who, "Przedmiot nie jest zniszczony.",FONT_NORMAL,COLOR_BLUE);
    elseif(!IsBitOn(Flags, RIF_MULTI_OP))
      PrintTextAbove(who, "Przedmiot nie jest zniszczony.");
    endif
    return 0;
  endif

  var Material := FindItemInContainer(who.backpack,MatObj);

  var Skill,Dmg;   
  if(IsPlayer(who))
    if(!Material or Material.amount < RepairAmt)
      SendSysMessage(who,"Nie masz materialu do naprawy przedmiotu.",FONT_NORMAL,COLOR_RED);
      SendBad(who,"Potrzebujesz "+RepairAmt+" "+GetDescForItem(MatObj));
      return 0;
    endif

    if(!ReserveItem(Material))
      SendSysMessage(who,"Masz material w plecaku, ale nie mozna go teraz uzyc.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

    Skill := GetEffectiveSkill(who,SkID);
    SendSysMessage(who,"Probujesz naprawic przedmiot...",FONT_NORMAL,COLOR_GREEN);

    Dmg   := 90 - CInt((GetItemHp(Item) * 100) / GetItemMaxHp(Item));
    if(Dmg < 0)
      Dmg := 0;
    endif
  endif

  if(IsBitOn(Flags, RIF_CHECK_ONLY))
    if(IsPlayer(who))
      ReleaseItem(Material);
    endif
    return 1; 
  endif;
  
  if(Source)
    if(!MakeNoiseDependant(who,Source,Sound,1500,Anim))
      if(Material)
        ReleaseItem(Material);
      endif
      return 0;
    endif
  else
    if(!MakeNoise(who,Sound,1500,Anim))
      if(Material)
        ReleaseItem(Material);
      endif
      return 0;
    endif
  endif

  if(IsPlayer(who))
    ReleaseItem(Material);
    if(SubtractAmount(Material,RepairAmt))
    
      if(CheckSkill(who,SkID,Dmg,0) and Skill >= 50)
        SetItemMaxHpMod(Item, GetItemMaxHpMod(Item)-1);
        SetItemHp(Item,GetItemMaxHP(Item));
        SendSysMessage(who,"Naprawil"+ggm(who,3)+" ten przedmiot calkowicie.",FONT_NORMAL,COLOR_BLUE);
      else
        if(GetItemHp(Item) - (4 - CInt(Skill / 40)) > 0)
          SetItemHp(Item,GetItemHp(Item) - (4 - CInt(Skill / 40)));
        else
          SetItemHp(Item,0);
        endif
        
        if(GetItemHp(Item) < 1)
          DestroyItem(Item);
          SendSysMessage(who, "Nie udalo Ci sie i zniszczyl"+ggm(who,3)+" przedmiot.",FONT_NORMAL,COLOR_RED);
        else
          SendSysMessage(who, "Nie udalo Ci sie i uszkodzil"+ggm(who,3)+" przedmiot.",FONT_NORMAL,COLOR_RED);
        endif
        
      endif
      
    else
      SendSysMessage(who,"Nie masz materialu do naprawy przedmiotu.",FONT_NORMAL,COLOR_RED);
      return 0;
    endif

  else//who is npc
    SetItemMaxHpMod(Item, GetItemMaxHpMod(Item)-1);
    SetItemHp(Item,GetItemMaxHP(Item));

    if(!IsBitOn(Flags, RIF_MULTI_OP))
      case(RandomInt(3))
        0: PrintTextAbove(who,"Kilka chwil i przedmiot jest naprawiony.");
        1: PrintTextAbove(who,"Uff, gotowe!");
        2: PrintTextAbove(who,"Gdyby robienie nowych przedmiotow bylo tak proste jak naprawa starych.");
      endcase
    endif    
    
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
