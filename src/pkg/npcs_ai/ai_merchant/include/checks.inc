//-----------------------------------------------------------------------------

function AISys_Merchant_CheckPetsInside(Source)

  if(Source.cmdlevel or !_AI_Merchant.City)
    return;
  endif

  if(GetEquipmentByLayer(Source, LAYER_MOUNT) or IsTamed(Source))
    if(IsInBuilding(Source.x,Source.y,Source.realm))
      if(IsTamed(Source))
        if(Distance(GetMaster(Source), Self()) < 15)
          Source := GetMaster(Source);
        endif
      endif
      
      if(Source.cmdlevel)
        return;
      endif
      
      if(!InformGuards(Self(), Source, _AI_Merchant.City, "Straz! Brac tego z tym brudnym zwierzem!", GMR_ANIMAL_INB))
        Say("Wynos sie z tym zwierzem bo pozalujesz!");
      endif

      var CityAl := GetCityAlignment(_AI_Merchant.City);

      if(CityAl == CITYAL_NEUTRAL)
        SetGuardChase(Source, _AI_Merchant.City, GDCF_ALLCITIES);
      else
        SetGuardChase(Source, _AI_Merchant.City);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_CanTrade(who)

  if(!_AI_Merchant.City)
    return 1;
  endif

  AISys_Merchant_CheckPetsInside(who);

  foreach Npc in ListMobilesNearLocation(who.x,who.y,who.z,3,who.realm)
    if(IsTamed(Npc))
      AISys_Merchant_CheckPetsInside(Npc);
    endif
  endforeach

  var Vamp := IsVampireCity(_AI_Merchant.City) and (GetEffectiveSkill(who,SKILLID_VAMPIRE) >= 40);
  var Wolf := IsWearwolfCity(_AI_Merchant.City) and (GetEffectiveSkill(who,SKILLID_WEARWOLF) >= 40);

  var CityAl := GetCityAlignment(_AI_Merchant.City);

  case(CityAl)
    CITYAL_EVIL: if(!IsRealMurderer(who))
                   AISys_Speech_Say(who, AI_SPID_GET_OUT);
                   return 0;
                 endif
                 
    CITYAL_GOOD: if(!Vamp and !Wolf)
                   if(IsRealMurderer(who))
                     AISys_Speech_Say(who, AI_SPID_GET_OUT);
                     return 0;
                   endif
                 endif
  endcase

  if(IsGuardChasing(who, _AI_Merchant.City))
    AISys_Speech_Say(who, AI_SPID_GET_OUT);
    return 0;
  endif

  if(IsNaked(who))
    AISys_Speech_Say(who, AI_SPID_GET_OUT_NAKED);
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------
