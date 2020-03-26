//-----------------------------------------------------------------------------

function AISys_Merchant_Process()

  if(_AI_Merchant.clockRestock + AI_MERCHANT_RESTOCK_TIME < ReadGameClock())
    AISys_Merchant_Restock();
  endif

  AI_Merchant_LookAround();

endfunction

//-----------------------------------------------------------------------------

function AISys_Merchant_Restock()

  var ObjTypes := _AI_Merchant.MaxAmounts.keys();

  foreach ObjType in ObjTypes
    _AI_Merchant.Amounts[ObjType] := _AI_Merchant.MaxAmounts[ObjType];
  endforeach
  
  _AI_Merchant.clockRestock := ReadGameClock();
  SetProperty(PROP_MERC_AMOUNTS, _AI_Merchant.Amounts);
  SetProperty(PROP_MERC_RESTOCK_TIME, _AI_Merchant.clockRestock);

endfunction

//-----------------------------------------------------------------------------

function AI_Merchant_LookAround()

  var CityAl := GetCityAlignment(_AI_Merchant.City);

  foreach Mob in ListMobilesNearLocationEx(Self().x, Self().y, Self().z, 5, LISTEX_FLAG_HIDDEN + LISTEX_FLAG_NORMAL, Self().realm)

    AISys_Merchant_CheckPetsInside(Mob);

    if(CityAl == CITYAL_GOOD)
      if(CInt(GetProperty("#NextMurdYell")) < ReadGameClock())

        if(Mob.murderer and IsPlayer(Mob))
          if(GetBaseSkill(Mob, SKILLID_VAMPIRE) >= 40)
            if(IsVampireCity(_AI_Merchant.City))
              continue;
            endif
          endif
          
          if(GetBaseSkill(Mob, SKILLID_WEARWOLF) >= 40)
            if(IsWearwolfCity(_AI_Merchant.City))
              continue;
            endif
          endif

          if(!InformGuards(Self(), Mob, "Straz! Tu jest morderca!", GMR_MURD))
            AISys_Speech_Say(Mob, AI_SPID_GET_OUT);
          else
            break;
          endif

          SetProperty("#NextMurdYell",ReadGameClock() + MURD_YELL_TIME);
        endif
      endif
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------
