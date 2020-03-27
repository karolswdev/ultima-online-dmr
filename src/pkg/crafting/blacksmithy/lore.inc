//-----------------------------------------------------------------------------

function SendItemDescription(who, item, SkId)

  var Desc := "";
  
  if(SkId == SKILLID_ARMORER and item.isa(POLCLASS_ARMOR))
    desc := Get_Armor(item);
  elseif(SkId == SKILLID_BLACKSMITHY and item.isa(POLCLASS_WEAPON))
    desc := Get_Weapon(item);
  endif
  
  if(!desc)
    if(SkId == SKILLID_ARMORER)
      SendSysMessage(who, "Ta umiejetnoscia ocenic mozesz tylko zbroje!",FONT_NORMAL,COLOR_RED);
    else
      SendSysMessage(who, "Ta umiejetnoscia ocenic mozesz tylko bron!",FONT_NORMAL,COLOR_RED);
    endif
    return;
  endif

  var Condition;
  var madeby := GetItemMakerName(item);
  if(!madeby)
    madeby := "nieznana osobe";
  else
    madeby := "osobe zwana "+madeby;
  endif
  var WhoArmsLore := GetEffectiveSkill(who,SkId);

  Condition := GetItemCondition(item);

  // -- Informacje o truciznie 
  var PLvl     := GetItemPoisonLvl(item);
  var Poisoner := GetItemPoisoner(item);
  Poisoner := Poisoner.name;
  if(!Poisoner)
    Poisoner := "nieznana osobe";
  endif 

  // --

  if(CheckSkill( who, SkId, -1, 0))
    if(WhoArmsLore >= 35)
      SendSysmessage(who,"Ten przedmiot jest "+ condition + " i " + desc + ".",FONT_NORMAL,COLOR_GREEN);
    else
      SendSysmessage(who,"Ten przedmiot jest " + condition + ".",FONT_NORMAL,COLOR_GREEN);
    endif
    if(WhoArmsLore >= 70)
      SendSysMessage(who,"Zostal wykonany przez "+ madeby +".",FONT_NORMAL,COLOR_GREEN);
      SendSysMessage(who,GetQualityMsg(item),FONT_NORMAL,COLOR_BLUE);
    endif
    if(PLvl)
      if(WhoArmsLore >= 80)
        SendSysMessage(who,"Ten przedmiot zostal zatruty "+GetPoisonPotionDesc(PLvl)+".",FONT_NORMAL,COLOR_GREEN_DARK);   
      endif
      if(WhoArmsLore >= 90)
        SendSysMessage(who,"Ten przedmiot zostal zatruty przez "+Poisoner+".",FONT_NORMAL,COLOR_GREEN); 
      endif
    endif
    if(WhoArmsLore >= 80)
      SetToolTipProp(item, "Stan", Condition);
    endif
  else
    SendSysmessage (who, "Ciezko powiedziec...",FONT_NORMAL,COLOR_RED);
  endif

endfunction

//-----------------------------------------------------------------------------

Function Get_Weapon(Item);

  var weapfile := ReadConfigFile( ITEMS_CFG );

  var max_dam  := GetDiceMaxValue(weapfile[item.objtype].Damage) + CInt(item.dmg_mod);
  var min_dam  := GetDiceMinValue(weapfile[item.objtype].Damage) + CInt(item.dmg_mod);

  var average  := CInt((Cint(Max_Dam) + Cint(Min_Dam))  / 2);

  if (average > 0 and average <= 5)
    return "ledwo by drasnal twego wroga";
  elseif (average >= 6 and average <= 7)
    return "zadalby minimalne obrazenia";
  elseif (average >= 8 and average <= 13)
    return "zadalby srednie obrazenia";
  elseif (average >= 14 and average <= 19)
    return "prawdopodobnie calkiem dobrze uszkodzilby twego wroga";
  elseif (average >= 20 and average <= 25)
    return "zadalby calkiem duze obrazenia i bol";
  elseif (average >= 26 and average <= 35)
    return "jest wspaniala bronia";
  elseif (average >= 36)
    return "zadalby nadludzkie obrazenia";
  endif

endfunction

//-----------------------------------------------------------------------------

function Get_armor(item);

  var ar := item.ar;

  If(!ar)
    return "nie zapewnia zadnej obrony";
  elseif (ar >= 1 and ar <= 9)
    return "nie zapewnia prawie zadnej obrony";
  elseif (ar >= 10 and ar <= 15)
    return "zapewnia bardzo slaba obrone";
  elseif (ar >= 16 and ar <= 24)
    return "zapewnia mala obrone";
  elseif (ar >= 25 and ar <= 30)
    return "zapewnia dobra obrone";
  elseif (ar >= 31 and ar <= 40)
    return "zapewnia mocna obrone przeciw atakom";
  elseif (ar >= 41 and ar <= 49)
    return "zapewnia wspaniala obrone";
  elseif (ar >= 50)
    return "zostal wykonany by zapewniac maxymalna obrone";
  endif

endfunction

//-----------------------------------------------------------------------------

function GetQualityMsg(item)

  var quality := GetItemQuality(item) * 100;
  
  if(quality)
    if(quality < 50)
      return "Ten przedmiot nie jest za duzo warty.";
    elseif(quality >= 50  and quality < 80)
      return "Ten przedmiot jest malo warty.";
    elseif(quality >= 80  and quality < 100)
      return "Ten przedmiot jest raczej slabej jakosci.";
    elseif(quality > 100  and quality < 130)
      return "Ten przedmiot jest troszke lepszej jakosci.";
    elseif(quality >= 130 and quality < 135)
      return "Ten przedmiot jest lepszej jakosci.";
    elseif(quality >= 135 and quality < 150)
      return "Ten przedmiot jest dobrej jakosci.";
    elseif(quality == 150)
      return "Ten przedmiot jest bardzo dobrej jakosci.";
    elseif(quality > 150)
      return "Ten przedmiot jest niezwyklej jakosci.";
    else
      return "Jakosc tego przedmiotu jest przecietna.";
    endif
  else
    LogError("Przedmiot Quality: "+quality+"! Item: "+hex(item.objtype)+"/"+hex(item.serial));
   endif

endfunction

//-----------------------------------------------------------------------------
