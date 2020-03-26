//-------------------------------------------------------------------------------

include "include/equipment";
include ":artifacts:artifacts";
//================================================================================
//--------------------------------------------------------------------------------
// Tool Item / Hp / Quality / Type (Crafting)
//--------------------------------------------------------------------------------
//================================================================================

function CheckToolWear(who, tool, SkID , times := 2.0)

  if(tool.objtype == UOBJ_WRESTLE or !tool or !tool.movable)
    return 1;
  endif
  
  if(IsArtifact(tool))
    return 1;
  endif

  if(IsSetFlag(tool, ITEMF_INDESTRUCTABLE))
    return 1;
  endif

  var ItemsCfg := ReadConfigFile(ITEMS_CFG);
  
  if(ItemsCfg[tool.objtype].Indestructable)
    return 1;
  endif

  var Hp := GetItemHp(tool);

  if(Hp > 0)
    var swings := CDbl(GetObjProperty(tool, PROP_SWINGS));
    if(!swings)
      swings := GetMaxItemSwings(tool);
    endif

    swings := swings - times;
    if(swings <= 0)
      swings  := GetMaxItemSwings(tool);
      if(who and SkID != error)
        swings := swings + GetEffectiveSkill(who, SkID);
      endif
      Hp := Hp - 1;
      if(Hp <= 5)
        SendSysMessage(who, CutItemDesc(tool) + " prawie sie rozpada.",FONT_NORMAL,COLOR_ORANGE);
      elseif(Hp <= 3)
        SendSysMessage(who, CutItemDesc(tool) + " prawie sie rozpada!",FONT_NORMAL,COLOR_RED);
      endif
    endif
    
    SetObjProperty(tool,PROP_SWINGS,swings);
    SetItemHp(tool,Hp);
	
  else
		SendSysMessage(who, CutItemDesc(tool) + " rozpada sie!",FONT_NORMAL,COLOR_RED);
		DestroyItem(tool);
		return 0;
	
   endif
  
  return 1;
  
endfunction

//-------------------------------------------------------------------------------

function GetItemSwingsPerc(tool)

  var swings := CInt(GetObjProperty(tool, PROP_SWINGS));
  if(!swings)
    return 100;
  endif

  var maxSwings := GetMaxItemSwings(tool);

  return CInt(CDbl(swings) / CDbl(maxSwings) * 100.0);

endfunction

//-------------------------------------------------------------------------------

function GetContForTool(who, Tool)

  var Cont := Tool.container;

  if(!Cont or !Cont.isa(POLCLASS_CONTAINER))
    Cont := who.backpack;
  endif

  return Cont;

endfunction

//-------------------------------------------------------------------------------

function GetMaxItemSwings(Tool)

  var iCfg      := ReadConfigFile(ITEMS_CFG);
  var MaxSwings := CInt(iCfg[tool.objtype].MaxSwings);

  if(!MaxSwings)
    if(iCfg[tool.objtype].Cloth)
      MaxSwings := 600;
    elseif(tool.isa(POLCLASS_ARMOR))
      MaxSwings := 1000;
    elseif(tool.isa(POLCLASS_WEAPON))
      MaxSwings := 150;
    else
      MaxSwings := 150;
    endif
  endif

  return MaxSwings;

endfunction

//-------------------------------------------------------------------------------

function IsToolEquipped(character, tool)

  if(!tool.isa(POLCLASS_EQUIPMENT))
    return 1;
  endif

  if((!Accessible(character , tool)) or (Distance(character , tool) > 1) )
    return 0;
  endif
  
  foreach item in ListEquippedItems (character)
    if(tool.serial == item.serial)
      return 1;
    endif
  endforeach
  
  ForceEquipItem(character , tool);
  return 0;

endfunction

//-------------------------------------------------------------------------------

function ItemHpExists(tool)

  if(tool.hp != error or GetObjProperty(tool, PROP_ITEMHP) != error)
    return 1;
  endif

  return 0;

endfunction

//-------------------------------------------------------------------------------

function GetItemHp(tool)

  if(!tool.isa(POLCLASS_ITEM))
    print("Tool does is not a polclass item");
    return 0;
  endif
  
  if(tool.hp == error)
    print("Tool has no HP... attempting to read from ObjProperties");
    var Hp := GetObjProperty(tool,PROP_ITEMHP);
    if(Hp == error)
	print("No property found. Setting the property");
      Hp := DEF_ITEMHP;
      SetObjProperty(tool,PROP_ITEMHP,DEF_ITEMHP);
    endif
    return CInt(Hp);
  else
    var Hp := GetObjProperty(tool, PROP_ITEMHP);
	if(tool.hp == 0 && tool.maxhp == 1 && Hp == error)
		print("Found vendor tool. Going to set its HP");
		Hp := DEF_ITEMHP;
		SetObjProperty(tool,PROP_ITEMHP,DEF_ITEMHP);
		return CInt(Hp);
	endif
    return tool.hp;
  endif

endfunction

//-------------------------------------------------------------------------------

function SetItemHp(byref tool,Hp)

  if(tool.isa(POLCLASS_ITEM))
    if(Hp > GetItemMaxHp(tool))
      Hp := GetItemMaxHp(tool);
    endif
    
    if(tool.hp == error)
      SetObjProperty(tool,PROP_ITEMHP,Hp);
    else
      tool.hp := Hp;
    endif
    EraseToolTipProp(tool,"Stan");
  endif

endfunction

//-------------------------------------------------------------------------------

function GetItemMaxHp(Item)

  if(!Item.isa(POLCLASS_ITEM))
    return 0;
  endif

  if(Item.maxhp == error)
    return DEF_ITEMHP + CInt(GetObjProperty(item,PROP_ITEM_MAXHP_MOD));
  else
    return Item.maxhp;
  endif

endfunction

//-------------------------------------------------------------------------------

function GetItemMaxHpMod(Item)
  
  if(!Item.isa(POLCLASS_ITEM))
    return 0;
  endif

  if(Item.maxhp_mod == error)
    return Cint(GetObjProperty(item, PROP_ITEM_MAXHP_MOD));
  else
    return Item.maxhp_mod;
  endif

endfunction

//-------------------------------------------------------------------------------

function SetItemMaxHpMod(Item, maxhp_mod)
  
  if(Item.isa(POLCLASS_ITEM))

    if(Item.maxhp_mod == error)
      SetObjProperty(item, PROP_ITEM_MAXHP_MOD, CInt(maxhp_mod));
    else
      Item.maxhp_mod := CInt(maxhp_mod);
    endif
    IncRevision(Item);
  endif

endfunction

//-------------------------------------------------------------------------------
