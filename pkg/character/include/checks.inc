//-----------------------------------------------------------------------

function GetAlignForPlayer(who)

  if(who.murderer)
    return CITYAL_EVIL;
  elseif(who.criminal)
    return CITYAL_NEUTRAL;
  endif

  return CITYAL_GOOD;

endfunction

//-----------------------------------------------------------------------

function IsCharClass(who, ClassId)

  var Class := GetCharClass(who);

  if(!Class) 
    case(ClassId)
      CLASS_WARRIOR:
        if(IsWarriorBySkill(who))
          return 1;
        endif
      CLASS_ARCHER:
        if(IsArcherBySkill(who))
          return 1;
        endif
      CLASS_MAGE:
        if(IsMageBySkill(who))
          return 1;
        endif
    endcase
  endif

  if(ClassId == Class)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------

function IsVampire(who)

  return CInt(GetObjProperty(who,"IsVampire"));

endfunction

//-----------------------------------------------------------------------------

function IsWearWolf(who)

  return CInt(GetObjProperty(who,"IsWearWolf"));

endfunction

//--------------------------------------------------------------------------------

function IsNaked(who)

  if(!GetEquipmentByLayer(who,LAYER_ROBE))
    if(!GetEquipmentByLayer(who,LAYER_PANTS))
      if(!GetEquipmentByLayer(who,LAYER_SHIRT))
        if(!GetEquipmentByLayer(who,LAYER_WAIST))
          if(!GetEquipmentByLayer(who,LAYER_CHEST))
            if(!GetEquipmentByLayer(who,LAYER_TUNIC))
              if(!GetEquipmentByLayer(who,LAYER_SKIRT))
                if(!GetEquipmentByLayer(who,LAYER_LEGS))
                  return 1;
                endif
              endif
            endif
          endif
        endif
      endif
    endif
  endif

  return 0;

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Cmdlevel Check
//-----------------------------------------------------------------------------
//=============================================================================

function IsAdmin(who)

  if(who.cmdlevel > 3)
    return 1;
  endif
  
  if(!who.acct)
    return 0;
  endif
  
  if(who.acct.GetProp("RealAdmin") == 1)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsRealAdmin(who)

  if(who.acct.GetProp("RealAdmin") == 1)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsDev(who)

  if(who.cmdlevel == 5)
    return 1;
  endif

  if(who.acct.GetProp("RealAdmin") == 1)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsGm(who)

  if(who.cmdlevel > 2)
    return 1;
  endif
  
  if(!who.acct)
    return 0;
  endif
  
  if(who.acct.GetProp("RealGm") == 1)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function IsStaff(who)

  return IsPlayer(who) and (who.cmdlevel > 0);

endfunction

//-----------------------------------------------------------------------------

function IsPlayer(who)

  return who.acctname;
  
endfunction

//-----------------------------------------------------------------------------

function IsQuestMaster(who)

  return CanUseCmd(who, "items q");

endfunction

//-----------------------------------------------------------------------------

function IsLoggedIn(who)

  if(!IsPlayer(who))
    return 0;
  endif

  return CInt(GetObjProperty(who,"#LoggedIn"));

endfunction

//-----------------------------------------------------------------------------

function IsFullyLogout(who)

  if(!IsPlayer(who))
    return 1;
  endif

  return !IsLoggedIn(who) and !CInt(GetObjProperty(who,"#LoggingOut"));

endfunction

//---------------------------------------------------------------------------------------

function IsHuman(who)

  case(who.graphic)
    CID_HUMAN_MALE:
    CID_ELF_MALE:
    CID_HUMAN_FEMALE:
    CID_ELF_FEMALE:
    CID_GHOST:
    CID_ELF_GHOST:
    CID_FEMALE_GHOST:
    CID_ELF_FGHOST:
    CID_GM:           return 1;
  endcase

  return 0;

endfunction

//---------------------------------------------------------------------------------------

function IsHairs(Obj)

  if(Obj.isa(POLCLASS_ITEM))
    Obj := Obj.objtype;
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  if(ItemCfg[Obj].LongHairs or ItemCfg[Obj].ShortHairs or ItemCfg[Obj].LongBeard or ItemCfg[Obj].ShortBeard or ItemCfg[Obj].Mustach)
    return 1;
  endif

  return 0;

endfunction

//---------------------------------------------------------------------------------------
// Professions
//---------------------------------------------------------------------------------------

function IsWarriorBySkill(who)

  if(!IsMageBySkill(who))
    if(!IsArcherBySkill(who))
      if(GetBaseSkill(who,SKILLID_WRESTLING) > 0)
        return 1;
      elseif(GetBaseSkill(who,SKILLID_SWORDSMANSHIP) > 0)
        return 1;
      elseif(GetBaseSkill(who,SKILLID_MACEFIGHTING) > 0)
        return 1;
      elseif(GetBaseSkill(who,SKILLID_FENCING) > 0)
        return 1;
      elseif(GetBaseSkill(who,SKILLID_AXESMANSHIP) > 0)
        return 1;
      elseif(GetBaseSkill(who,SKILLID_SWORDSMANSHIP) > 0)
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//---------------------------------------------------------------------------------------

function IsArcherBySkill(who)

  if(GetBaseSkill(who,SKILLID_ARCHERY) > 0)
    return 1;
  endif
  return 0;

endfunction

//---------------------------------------------------------------------------------------

function IsMageBySkill(who)

  if(GetBaseSkill(who,SKILLID_MAGERY) > 0)
    return 1;
  elseif(GetBaseSkill(who,SKILLID_NECRO) > 0)
    return 1;
  elseif(GetBaseSkill(who,SKILLID_DRUID) > 0)
    return 1;
  endif
  return 0;

endfunction

//---------------------------------------------------------------------------------------

function IsThief(who)

  if(GetBaseSkill(who,SKILLID_LOCKPICKING) > 30 or GetBaseSkill(who, SKILLID_STEALING) > 30)
    return 1;
  endif

  return 0;

endfunction

//---------------------------------------------------------------------------------------

function IsBard(who)

  if(GetBaseSkill(who,SKILLID_MUSICIANSHIP) > 30)
    if(GetBaseSkill(who, SKILLID_ENTICEMENT) > 30 or GetBaseSkill(who, SKILLID_PEACEMAKING) > 30 or GetBaseSkill(who, SKILLID_PROVOCATION) > 30)
      return 1;
    endif
  endif

  return 0;

endfunction

//---------------------------------------------------------------------------------------

function IsRealMage(who)

  return GetBaseSkill(who,SKILLID_MAGERY) > 0;

endfunction

//---------------------------------------------------------------------------------------

function IsDruid(who)

  return GetBaseSkill(who,SKILLID_DRUID) > 0;

endfunction

//---------------------------------------------------------------------------------------

function IsNecro(who)

  return GetBaseSkill(who,SKILLID_NECRO) > 0;

endfunction

//-----------------------------------------------------------------------
