//-----------------------------------------------------------------------------

include "include/player/friends";

//-----------------------------------------------------------------------------

const S_SUCCESS_ID     := "SSuccess";
const S_FAIL_ID        := "SFail";
const S_BATTLE_ID      := "SBattle";
const S_PEACE_ID       := "SPeace";
const S_DRAMA_ID       := "SDrama";

const MUSIC_CFG        := ":musicianship:music";

//-----------------------------------------------------------------------------

function GetSound(InstObj,Id)

  var Type := GetInstrType(InstObj);
  
  if(Type)
    var MusicCfg := ReadConfigFile(MUSIC_CFG);
    return CInt(GetConfigInt(MusicCfg[Type],Id));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function play(character, Diff, item,Points := 0, SoundType := 0)

  if(IsPlayer(Character) and item.isa(POLCLASS_ITEM))
    if(!CheckToolWear (character, item, SKILLID_MUSICIANSHIP))
      return 0;
    endif
  endif

  if(CheckSkill(character, SKILLID_MUSICIANSHIP, Diff, Points))
    if(!SoundType)
      PlaySoundEffect( character, GetSound(item.graphic,GetSavedSound(character)));
    else
      PlaySoundEffect( character, GetSound(item.graphic, SoundType));
    endif
    BardBonus(character, item);
    return 1;
  else
    PlaySoundEffect( character, GetSound(item.graphic,S_FAIL_ID));
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function FindInstrument(character)

  var Instr := GetInstruments();
  
  foreach item in EnumerateItemsInContainer(character.backpack)
    if(item.graphic in Instr)
      return item;
    endif
  endforeach
  
  return 0;

endfunction

//-----------------------------------------------------------------------------

function can_sing(who)

  if(GetEquipmentByLayer(who,LAYER_HAND1) or GetEquipmentByLayer(who,LAYER_HAND2))
    SendSysMessage(who,"Musisz miec puste rece.",FONT_NORMAL,COLOR_RED);
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GetInstruments()

  var MusicCfg := ReadConfigFile(MUSIC_CFG);
  var Instr    := GetConfigStringArray(MusicCfg["Instruments"],"Inst");

  foreach Ins in Instr
    Ins := CInt(Ins);
  endforeach
  
  return Instr;

endfunction

//-----------------------------------------------------------------------------

function GetInstrType(Obj)

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  return ItemCfg[Obj].MusicType;

endfunction

//-----------------------------------------------------------------------------

function GetSavedSound(who)

  if(!GetObjProperty(who,"BardSound"))
    SetObjProperty(who,"BardSound",S_SUCCESS_ID);
  endif
  
  return GetObjProperty(who,"BardSound");
  
endfunction

//-----------------------------------------------------------------------------

function SaveBardSound(who,SiD)

  SetObjProperty(who,"BardSound",SiD);

endfunction

//-----------------------------------------------------------------------------

function BardBonus(who,item)
  
  var SkBMusic    := GetBaseSkill(who, SKILLID_MUSICIANSHIP);
  var SkEfMusic   := GetEffectiveSkill(who, SKILLID_MUSICIANSHIP);
  var SkBProv     := GetBaseSkill(who,SKILLID_PROVOCATION);
  var SkEfProv    := GetEffectiveSkill(who,SKILLID_PROVOCATION);
  var SkBEnti     := GetBaseSkill(who,SKILLID_ENTICEMENT);
  var SkEfEnti    := GetEffectiveSkill(who,SKILLID_ENTICEMENT);
  var SkBPeace    := GetBaseSkill(who,SKILLID_PEACEMAKING);
  var SkEfPeace   := GetEffectiveSkill(who,SKILLID_PEACEMAKING);
  
  if(SkBMusic >= 100 and SkBProv >=  100 and SkBEnti >= 100 and SkBPeace >= 100)
    
    var CanGiveMod;
    var ModAmtBonus := CInt(ToMin(SkBMusic-104,0)) + CInt(ToMin(SkEfMusic-100,0)/10) + CInt(ToMin(SkBProv-104,0)) + CInt(ToMin(SkEfProv-100,0)/10) + CInt(ToMin(SkBEnti-104,0)) + CInt(ToMin(SkEfEnti-100,0)/10) + CInt(ToMin(SkBPeace-104,0)) + CInt(ToMin(SkEfPeace-100,0)/10);    
    var Range    := 20 + ModAmtBonus;
    var Duration := 180 + RandomInt(120);
   
    var ModMusicMod  :=     Max(CInt(2 + CInt(CheckItemQuality(item)) / 3 + CInt(Max(SkBMusic-100,0)/2) + CInt(Max(SkEfMusic-100,0)/10)+  RandomInt(8)),14);
    var PureMusicMod :=     CInt(Max(SkEfMusic-100,0)/10);
    var ModAmtFromProv :=   Max(CInt(CInt(CheckItemQuality(item)) / 3 + CInt(Max(SkBProv-100,0)/2) + CInt(Max(SkEfProv-100,0)/10) + RandomInt(6) + PureMusicMod) ,10);
    var ModAmtFromEnti :=   Max(Cint(CInt(CheckItemQuality(item)) / 3 + CInt(Max(SkBEnti-100,0)/2) + CInt(Max(SkEfEnti-100,0)/10) + RandomInt(6) + PureMusicMod),10);
    var ModAmtFromPeace :=  Max(Cint(CInt(CheckItemQuality(item)) / 3 + CInt(Max(SkBPeace-100,0)/2) + CInt(Max(SkEfPeace-100,0)/10) + RandomInt(6) + PureMusicMod + 1),12); // heal stamina

  
    foreach Mob in ListMobilesNearLocation(who.x,who.y,who.z,Range,who.realm)
      if(IsFriend(who,Mob)  or Mob == who)
        if(CanGetCriminal(who,Mob,TGTOPT_HELPFUL))
          if(!who.murderer and GetObjProperty(who, "RestrictedStAttack"))
            continue;
          endif
          SetCriminal(who, 1, "0", "Bless barda na krim/redzie.");
        endif
        if(GetSavedSound(who) == S_SUCCESS_ID)
        
          if(GetEffectiveSkill(Mob, SKILLID_ANATOMY) >= 50)
            if(CanSkillMod(Mob,SKILLID_ANATOMY))
              DoSkillMod(Mob, SKILLID_ANATOMY, ModMusicMod, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_EVALINT) >= 50)
            if(CanSkillMod(Mob,SKILLID_EVALINT))
              DoSkillMod(Mob, SKILLID_EVALINT, ModMusicMod, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_SPIRITSPEAK) >= 50)
            if(CanSkillMod(Mob,SKILLID_SPIRITSPEAK))
              DoSkillMod(Mob, SKILLID_SPIRITSPEAK, ModMusicMod, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_ANIMALLORE) >= 50)
            if(CanSkillMod(Mob,SKILLID_ANIMALLORE))
              DoSkillMod(Mob, SKILLID_ANIMALLORE, ModMusicMod, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_ZEAL) >= 50)
            if(CanSkillMod(Mob,SKILLID_ZEAL))
              DoSkillMod(Mob, SKILLID_ZEAL, ModMusicMod, Duration);
              CanGiveMod :=1;
            endif
          endif
          
          if(CanGiveMod == 1)
            PrintTextAbove(Mob,"*czujesz sie madrzejszy*");
          endif  
          
        endif
          
        if(GetSavedSound(who) == S_PEACE_ID)
          
          if(GetHP(Mob) < GetMaxHP(Mob))
            HealMobile(Mob,ModAmtFromPeace, who);
            CanGiveMod :=1;
          endif
          if(GetStamina(Mob) < GetMaxStamina(Mob))
            HealStamina(Mob,ModAmtFromPeace, who);
            CanGiveMod :=1;
          endif
          if(GetMana(Mob) < GetMaxMana(Mob))
            HealMana(Mob,ModAmtFromPeace+3, who);
            CanGiveMod :=1;
          endif
          
          if(CanGiveMod == 1)
            PrintTextAbove(Mob,"*czujesz sie lepiej*");
          endif
    
  
        endif
          
        if(GetSavedSound(who) == S_DRAMA_ID)
      
          if(CanMod(Mob,MOD_POISON_PROT)) 
            DoMod(Mob, MOD_POISON_PROT, ModAmtFromEnti, Duration);
          endif;

          if (CanMod(Mob,MOD_AR) == 0)
            if( Mob == who)
              ModifyMod(Mob, MOD_AR, CInt(ModAmtFromEnti), Duration);
            else
              ModifyMod(Mob, MOD_AR, ModAmtFromEnti, Duration);
            endif
          elseif(CanMod(Mob,MOD_AR) == 1)
            if( Mob == who) 
              DoMod(Mob, MOD_AR, CInt(ModAmtFromEnti*1.5), Duration);
            else
              DoMod(Mob, MOD_AR, ModAmtFromEnti, Duration);
            endif
          endif;

          PrintTextAbove(Mob,"*czujesz sie chroniony*"); // bylo MOD_AR
          
        endif
  
        if(GetSavedSound(who) == S_BATTLE_ID)
          
          if(GetEffectiveSkill(Mob, SKILLID_PARRY) >= 50)
            if(CanSkillMod(Mob,SKILLID_PARRY))
              DoSkillMod(Mob, SKILLID_PARRY, ModAmtFromProv, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_ARCHERY) >= 50)
            if(CanSkillMod(Mob,SKILLID_ARCHERY))
              DoSkillMod(Mob, SKILLID_ARCHERY, ModAmtFromProv, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_SWORDSMANSHIP) >= 50)
            if(CanSkillMod(Mob,SKILLID_SWORDSMANSHIP))
              DoSkillMod(Mob, SKILLID_SWORDSMANSHIP, ModAmtFromProv, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_MACEFIGHTING) >= 50)
            if(CanSkillMod(Mob,SKILLID_MACEFIGHTING))
              DoSkillMod(Mob, SKILLID_MACEFIGHTING, ModAmtFromProv, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_FENCING) >= 50)
            if(CanSkillMod(Mob,SKILLID_FENCING))
              DoSkillMod(Mob, SKILLID_FENCING, ModAmtFromProv, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_WRESTLING) >= 50)
            if(CanSkillMod(Mob,SKILLID_WRESTLING))
              DoSkillMod(Mob, SKILLID_WRESTLING, ModAmtFromProv, Duration);
              CanGiveMod :=1;
            endif
          endif
          if(GetEffectiveSkill(Mob, SKILLID_AXESMANSHIP) >= 50)
            if(CanSkillMod(Mob,SKILLID_AXESMANSHIP))
              DoSkillMod(Mob, SKILLID_AXESMANSHIP, ModAmtFromProv, Duration);
              CanGiveMod :=1;
            endif
          endif
          
          if(CanGiveMod == 1)
            PrintTextAbove(Mob,"*czujesz sie mocniejszy*");
          endif
          
        endif
      endif  
    endforeach
  endif

endfunction

//-----------------------------------------------------------------------------








