//-----------------------------------------------------------------------------

function NpcHaveFlag(Npc, Flag)

  var cfgElem := GetNpcConfig(Npc);

  if(!cfgElem)
    return cfgElem;
  endif

  return GetConfigInt(cfgElem, Flag);

endfunction

//-----------------------------------------------------------------------------

function GetNpcCfgProp(Npc, PropName)

  var cfgElem := GetNpcConfig(Npc);

  if(!cfgElem)
    return cfgElem;
  endif

  return GetConfigString(cfgElem, PropName);

endfunction

//-----------------------------------------------------------------------------

function ParseNpcTemplate(Template)

	if ( Template.IsA(POLCLASS_NPC) )
		Template := Template.npctemplate;
	endif

	//Find positions of ":" characters
	var colon_a := Find(Template, ":", 1);
	var colon_b := Find(Template, ":", 2);

	var parsed := struct{"package", "template"};

	if ( !colon_a || !colon_b )
		//Not a complete package name - use default npcdesc.cfg
		parsed.package := "";
		parsed.template := Template;
	else
		parsed.package := Template[colon_a+1, colon_b-2];
		parsed.template := Template[colon_b+1, Len(Template)];
	endif

	return parsed;

endfunction

//--------------------------------------------------------------------------------

function GetNpcConfig(Template)

	if ( Template.IsA(POLCLASS_NPC) )
		Template := Template.npctemplate;
	endif

  var Parsed := ParseNpcTemplate(Template);
  var NpcCfg := ReadConfigFile(":"+Parsed.package+":npcdesc");

  if(NpcCfg[Parsed.template])
    return NpcCfg[Parsed.template];
  endif

  return error{"errortext" := "Template "+Template+" nie istnieje!"};
  
endfunction

//--------------------------------------------------------------------------------

function GetNpcMercConfig(Template)

	if ( Template.IsA(POLCLASS_NPC) )
		Template := Template.npctemplate;
	endif

  var cfgElem := GetNpcConfig(Template);
  var mercCfg := ReadConfigFile(":mcfg:merchants");

  if(!cfgElem.MercType)
    return error{"errortext" := "Template "+Template+" nie posiada MercType!"};
  endif

  return mercCfg[cfgElem.MercType];
  
endfunction

//--------------------------------------------------------------------------------

function GetAllNpcTemplates()

	var elem_names := array{};

	elem_names := elem_names + GetConfigStringKeys(ReadConfigFile("::npcdesc"));

	foreach package in (Packages())
		if ( package.npcdesc )
			var pkg_name := ":"+package.name+":";
			foreach template_name in (GetConfigStringKeys(ReadConfigFile(pkg_name+"npcdesc")))
				elem_names.Append(pkg_name+template_name);
			endforeach
		endif
	endforeach

	return elem_names;

endfunction

//-----------------------------------------------------------------------------

function GetNpcGrapElem(Grap)

  var nCfg := ReadConfigFile(NPCGRAP_CFG);
  return nCfg[Grap];

endfunction

//--------------------------------------------------------------------------------

function NpcAnchorControl(me, bStop)

  if(bStop)
    EraseObjProperty(me, "#AnchorStop");
  else
    SetObjProperty(me, "#AnchorStop", ReadGameClock());
  endif  

endfunction 

//-----------------------------------------------------------------------------

function CheckNpcAttackSkill(me)

  if(!me.isa(POLCLASS_NPC) or !IsHuman(me))
    return;
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var SkID := FindSkillidByIdentifier(ItemCfg[me.weapon.objtype].Attribute);

  if(GetBaseSkill(me, SkID))
    return;
  endif

  var FightSkID := GetFightSkId(me);
  
  var SkVal := GetBaseSkill(me, FightSkID);

  if(!SkVal)
    LogError("Istnieje npc '"+me.npctemplate+"'/"+me.serial+" bez istniejacego skilla bojowego! Skill broni: "+GetSkillConfigElem(SkID).Name, LOGF_SCRIPTS);
    return;
  endif

  SetBaseSkill(me, SkID, SkVal);

endfunction

//--------------------------------------------------------------------------------
