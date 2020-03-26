//-----------------------------------------------------------------------------

include "include/misc";
include ":glyphs:glyphs";

//-----------------------------------------------------------------------------

function GetExceptionalMaterials(who, SkID, Group := 0)

  var List := array;
  var BaseSkill := GetBaseSkill(who, SkID);
  
  if(BaseSkill < SKILL_TO_MAKE_EXP)
    return List;
  endif

  var ExpCfg := ReadConfigFile(":*:exp_materials");
  var AttrId := GetAttributeIdBySkillId(SkId);

  var expElem;

  if(Group)
    expElem := ExpCfg[AttrId+"_"+Group];
  else
    expElem := ExpCfg[AttrId];
  endif

  if(!expElem)
    LogError("Nie znaleziono elementu w exp_materials dla AttrId: ["+AttrId+"] SkId: "+SkId+" Grupa: "+Group);
    return 0;
  endif

  var i, ItemObj;
  var ItemCfg := ReadConfigFile(ITEMS_CFG);

  for(i:=1; i<=4; i+=1)
    ItemObj := GetConfigString(expElem, "Level"+i);
    ItemObj := SplitWords(ItemObj);
    ItemObj := GetItemObjType(ItemObj[1]);

    List.append(struct{"objtype" := ItemObj, "graphic" := ItemCfg[ItemObj].Graphic, "color" := ItemCfg[ItemObj].Color});
  endfor
  
  return List;

endfunction

//-----------------------------------------------------------------------------

function CraftExceptional(who, SkID, ExpMod, Level, Group := 0)

  var SkElem := GetSkillConfigElem(SkID);
  
  if(SkElem.CityGain)
    if(FindCity(who))
      if(SkElem.CityGain == "building")
        if(IsInBuilding(who.x, who.y, who.realm))
          var MercTypes := GetConfigStringArray(SkElem, "MercType");

          foreach Type in MercTypes
            if(IsNpcNear(who.x, who.y, who.realm, 10, Type, 1))
              ExpMod := ExpMod + EXP_CITY_GAIN;
              break;
            endif
          endforeach
        endif
      elseif(SkElem.CityGain == "whole")
        ExpMod += EXP_CITY_GAIN;
      endif
    endif
  endif

  var AttrId := GetAttributeIdBySkillId(SkId);

  if(ExpMod > 0)
    ModifyGainByNutr(who, ExpMod, AttrId);
  endif
   
  var BaseSkill := GetBaseSkill(who, SkID);
  var ExpCfg := ReadConfigFile(":*:exp_materials");

  var expElem;
  var ExpData := struct;
  ExpData.+makeExp := 0;
  ExpData.+makeNamed := 0;
  ExpData.+inscribe := 0;
  ExpData.+changeColor := 0;

  if(Group)
    expElem := ExpCfg[AttrId+"_"+Group];
  else
    expElem := ExpCfg[AttrId];
  endif

  if(!expElem)
    LogError("Nie znaleziono elementu w exp_materials dla AttrId: ["+AttrId+"] SkId: "+SkId+" Grupa: "+Group);
    return 0;
  endif

  if(IsDebug(who,"ExpDebug") || GetObjProperty(who, "GlyphTests"))
    ExpData.makeExp := 1;
    ExpData.makeNamed := 1;
	ExpData.inscribe := 1;
	ExpData.changeColor := 1;
    return ExpData;
  endif

  var Rand := RandomInt(1000);
  var Overflowed := HaveChance(GetCraftOverflowChance(who));

  if(IsDebug(who,"ExpChance"))
    if(Rand < ExpMod)
      if(Overflowed)
        SendSysMessage(who, "ExpOverflow: "+ExpMod+" ("+Rand+")", FONT_SMALL_LIGHT, COLOR_RED);
      else
        SendSysMessage(who, "ExpSuccess: "+ExpMod+" ("+Rand+")", FONT_SMALL_LIGHT, COLOR_WHITE);
      endif
    else
      SendSysMessage(who, "ExpMissed: "+ExpMod+" ("+Rand+")", FONT_SMALL_LIGHT, COLOR_ORANGE);
    endif
  endif
  
  // Goqsane: For future - we might have items... spells.. etc that make it possible for a 'guaranteed' exp
  var guaranteedExpNumber := CInt(GetObjProperty(who, "MakeExp"));
  var guaranteedExp := 0;
  if(guaranteedExpNumber > 0)
	guaranteedExp := 1;
	SetObjProperty(who, "MakeExp", guaranteedExpNumber - 1);
  endif
  
  var glyphExpBonus := GetGlyphMod(who, "MasterArtisan");
  Rand := Rand - glyphExpBonus;
  
  var glyphColorBonus := GetGlyphMod(who, "ColorChanger");
  var glyphInscribe := GetGlyphMod(who, "CustomInscription");
  

  if(BaseSkill >= SKILL_TO_MAKE_EXP and (Rand < ExpMod or guaranteedExp))
    if(!Overflowed)
      if(SubtractExpMatForLevel(who, expElem, Level, AttrId))
        ExpData.makeExp := 1;
		
		if(glyphInscribe > 0)
			if(HaveChance(glyphInscribe))
				ExpData.inscribe := 1;
			endif
		endif
		
		if(glyphColorBonus > 0)
			if(HaveChance(glyphColorBonus))
				ExpData.changeColor := 1;
			endif
		endif
		
        if(BaseSkill >= SKILL_TO_NAMED_EXP)
          if(SubtractExpMaterials(who, expElem, "ExpNamed", AttrId))
            ExpData.makeNamed := 1;
          endif
        endif  
      else
        SendSysMessage(who, "Brakuje Ci skladnikow na wykonanie wyjatkowej jakosci przedmiotu!", FONT_NORMAL, COLOR_ORANGE);
      endif
    else
      SendSysMessage(who, "Prawie zrobil"+ggm(who,3)+" wyjatek... ale pomylil"+ggm(who,3)+" sie. Zbyt wiele mysli na raz...", FONT_NORMAL, COLOR_ORANGE);
    endif
  endif

  return ExpData;

endfunction

//-----------------------------------------------------------------------------

function SubtractExpMatForLevel(who, expElem, Level, AttrId)

  var i;

  for(i:=Level; i<=4; i+=1)
    if(SubtractExpMaterials(who, expElem, "Level"+i, AttrId))
      return 1;
    endif
  endfor

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetCraftOverflowChance(who)

  var Chance := 0;
  var SkCount := 0;
  var Cfg := ReadConfigFile(ATTRIBUTES_CFG);

  foreach AttrId in GetConfigStringKeys(Cfg)
    if(Cfg[AttrId].AttrGroup == "crafting")
      var SkId := CInt(Cfg[AttrId].SkillID);

      if(GetBaseSkillBaseValue(who, SkId) > 100)
        SkCount += 1;
      endif
    endif
  endforeach

  if(SkCount > 2)
    Chance := (SkCount-2) * 5;
  endif

  return CInt(Chance/2);

endfunction

//-----------------------------------------------------------------------------

function SubtractExpMaterials(who, expElem, expGroup, AttrId)

  var Materials := GetConfigStringArray(expElem, expGroup);
  var ObjType, Amount;

  if(!Materials or !Materials.size())
    LogError("Nie znaleziono listy skladnikow w exp_materials dla "+AttrId+" Grupa: "+expGroup);
    return 0;
  endif

  foreach Material in Materials
    Material := SplitWords(Material);
    ObjType := GetItemObjType(Material[1]);
    Amount  := CInt(Material[2]);

    if(!ObjType)
      LogError("Blednie zdefiniowany material na wyjatek!!! Material "+Material[1]+" nie istnieje! Grupa: "+expGroup);
      continue;
    endif

    var Item := FindItemInContainer(who.backpack, ObjType);

    if(Item)
      if(SubtractAmount(Item, 1))
        return 1;
      endif
    endif

  endforeach

  return 0;

endfunction

//-----------------------------------------------------------------------------
