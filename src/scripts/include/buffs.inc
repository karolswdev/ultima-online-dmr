//-----------------------------------------------------------------------------------//
// Constants
//-----------------------------------------------------------------------------------//
// http://www.uoguide.com/Buff_Bar
//-----------------------------------------------------------------------------------//

//---- Unused

const BFICON_EVILOMEN        := 1007; // Evil Omen
const BFICON_STEALTH         := 1012; // Stealth
const BFICON_MINDROT         := 1017; // Mindrot
const BFICON_STRANGLE        := 1019; // Strangle
const BFICON_ANIMAL_FORM     := 1034; // Animal Form
const BFICON_INVIS           := 1036; // Invisibility
const BFICON_CURSE           := 1043; // Curse
const BFICON_MASS_CURSE      := 1044; // Mass Curse

//---- Used

const BFICON_DISARM          := 1002; // Disarm
const BFICON_NIGHTSIGHT      := 1005; // Nightsight
const BFICON_REACTIVE_ARMOR  := 1028; // Reactive Armor
const BFICON_PROTECTION      := 1029; // Protection
const BFICON_MAGIC_REFLECT   := 1031; // Magic Reflection
const BFICON_INCO            := 1032; // Incognito
const BFICON_POLYMORPH       := 1035; // Polymorph
const BFICON_PARA            := 1037; // Paralyze
const BFICON_POISON          := 1038; // Poison
const BFICON_BLEED           := 1039; // Bleed
const BFICON_CLUMSY          := 1040; // Clumsy
const BFICON_FEEBLEMIND      := 1041; // Feeble Mind
const BFICON_WEAKEN          := 1042; // Weaken
const BFICON_AGILITY         := 1045; // Agility
const BFICON_CUNNING         := 1046; // Cunning
const BFICON_STRENGTH        := 1047; // Strength

//Necro
const BFICON_MONSTER_AR      := 30032; //BoneArmor

const CLILOC_EMPTY           := 3010209;
const CLILOC_VARIABLE        := 1042971;

const BFICON_LIFE            := 1010; // Divine Fury
const BFICON_MANA            := 1014; // Blood Oath caster
const BFICON_STAMINA         := 1020; // Gift of Renewal
const BFICON_DEC_MANA        := 1015; // Blood Oath curse
const BFICON_DEC_LIFE        := 1008; // ? GumpID 0x7556
const BFICON_DEC_STAMINA     := 1009; // ? GumpID 0x753A

const BFICON_MANA_REGEN      := 1013; // Active Meditation
const BFICON_HP_REGEN        := 1025; // Gift Of Life
const BFICON_STAM_REGEN      := 1026; // Arcane Empowerment

const BFICON_STUPOR          := 1001; // Dismount

const BFICON_POISON_PROT     := 1048; // Bless

const CLITMOD_HEAR_GHOSTS    := 1044092; //Spirit Speak
const BFICON_HEAR_GHOSTS     := 1024; // Ethereal Voyage

const CLITMOD_POISON_DMG     := 1060406; // poison damage
const BFICON_POISON_DMG      := 1016; // Corpse Skin

const CLITMOD_CTH            := 1060415; // An attack with a large bonus to your chance to hit.
const BFICON_CTH             := 1027; // Mortal Strike

const CLITMOD_FASTER_CAST    := 1060413; // faster casting
const BFICON_FASTER_CAST     := 1023; // Essence of Wind

const CLITMOD_FIRE_BLOOD     := 1028839; // Bleed Attack
const BFICON_FIRE_BLOOD      := 1018; // Pain Spike

const CLITMOD_FIRE_DMG       := 1060405; // fire damage
const BFICON_FIRE_DMG        := 1018; // Pain Spike

const BFICON_RETORT          := 1006; // Death Strike

const CLITMOD_CHARM_ANIMAL   := 1028232; // animal trainer
const BFICON_CHARM_ANIMAL    := 1011; // Enemy Of One

const CLITMOD_MAGIC_SHIELD   := 1075589; // Mystic Shield
const BFICON_MAGIC_SHIELD    := 1030; // Arch Protection

const BFICON_CRIT_HIT        := 1021; // Attune Weapon
const BFICON_STAM_DMG        := 1022; // Thunderstorm

//----------------------------------------------------------------------------

//Hunter
const CLITMOD_EXPLO_DMG      := 1060419; //hit fire area ~~

const BFICON_DEMON_SLAYER    := 1021;//Attune Weapon
const CLITMOD_DEMON_SLAYER   := 1060460 ;//demon slayer

const CLITMOD_MANA_DMG       := 1044411; // mana drain

const BFICON_MULTI_SHOT      := 1006; // Death Strike

const CLITMOD_MANA_DRAIN     := 1060427; //mana leeach

const CLITMOD_PENETRATE      := 1060408;// defense chance increase ~1_val~%

const CLITMOD_CTH_REDUCED    := 1062318; //Your defense chance has been reduced!

const CLITMOD_PENETRATE_RED  := 1062319;
//----------------------------------------------------------------------------

//Vampire

const BFICON_LIFE_STOLEN     := 1011; //Enemy of One
const CLITMOD_LIFE_STOLEN    := 1060422; //hit life leech

const BFICON_COLD_DMG        := 1021; //Attune Weapon
const CLITMOD_COLD_DMG       := 1060404; //cold dmg

const BFICON_COLOR           := 1035; // Polymorph
const CLITMOD1_COLOR         := 1075824; 
const CLITMOD2_COLOR         := 1075820;

const BFICON_ICEBARRIER      := 1025;// Gift of life
const CLITMOD_ICEBARRIER     := 1060493; //Cleans By Fire

//-----------------------------------------------------------------------------

//Paladin

const BFICON_CHAMPION        := 1021;//Attune Weapon
const CLITMOD1_CHAMPION      := 1060460 ;//demon slayer
const CLITMOD2_CHAMPION     := 1060479;//undead slayer

const BFICON_UNTOUCHABLE     := 1013; //Active Meditation
const CLITMOD1_UNTOUCHABLE   := 1044106;
const CLITMOD2_UNTOUCHABLE   := 1075106;

//-----------------------------------------------------------------------------

//DarkKnight

const BFICON_ORDER_SLAY      := 1021; //Attune Weapon
const CLITMOD_ORDER_SLAY     := 1060402;

//-----------------------------------------------------------------------------

const CLITMOD_LIFE           := 1060431; // hit point increase
const CLITMOD_MANA           := 1060439; // mana increase
const CLITMOD_STAMINA        := 1060484; // stamina increase

const CLIDMOD_INC_SKILL      := 3000260; // Your skill in ~1skillname~ has increased by ~2changeamount~.  It is now ~3newvalue~.
const CLIDMOD_DEC_SKILL      := 3000261; // Your skill in ~1skillname~ has decreased by ~2changeamount~.  It is now ~3newvalue~.

const CLITMOD_FIRERESIST     := 1062749; // fire resist
const CLITMOD_COLDRESIST     := 1062750; // cold resist
const CLITMOD_HP_REGEN       := 1062734; // hit point regeneration
const CLITMOD_MANA_REGEN     := 1062735; // mana regeneration
const CLITMOD_STAMINA_REGEN  := 1062756; // stamina regeneration
const CLIDMOD_FASTER_CAST    := 1062744; // faster casting
const CLIDMOD_MANA_COST      := 1062745; // lower mana cost

const CLITMOD_SPELL_DMG      := 1062742; // spell damage increase
const CLIDMOD_SPELL_DMG      := 1078682; // Bonus to spell damage

const CLITMOD_DISARM         := 1028842; // Disarm

const CLITMOD_DAEMON_SLAY    := 1060460; // demon slayer
const CLITMOD_UNDEAD_SLAY    := 1060479; // undead slayer

const CLIDMOD_RETORT         := 1063267; // An attack with a large bonus to your chance to hit.

const CLITMOD_STUPOR         := 1028843; // Dismount

const CLIDMOD_CHARM_ANIMAL   := 1005436; // The animal shies away from you.

const CLITMOD_RED_SPELL      := 1044086; // Resisting Spells

const CLITMOD_DISMOUNT_RES   := 1028843; // Dismount
const CLIDMOD_DISMOUNT_RES   := 1061158; // Physical Resistance

const BFICON_RED_SPELL       := 1033; // Disguised
const BFICON_DISMOUNT_RES    := 1023; // Essence of Wind

const BFICON_TREE_SUPPORT    := 1010; // Divine Fury

//-----------------------------------------------------------------------------

function SendAddBuff(who, idIcon, secTime, ciTitle, ciDesc, Args)
  
  if(!IsPlayer(who))
    return;
  endif

  var Packet := CreatePacket(0xDF, MSGLEN_VARIABLE);

  Packet.SetInt32(3, who.serial);
  Packet.SetInt16(7, idIcon);
  Packet.SetInt16(9, 0x1);
  Packet.SetInt32(11, 0x0);
  Packet.SetInt16(15, idIcon);
  Packet.SetInt16(17, 0x1);
  Packet.SetInt32(19, 0x0);
  Packet.SetInt16(23, CInt(secTime));
  Packet.SetInt16(25, 0);
  Packet.SetInt8(27, 0);
  Packet.SetInt32(28, ciTitle);
  Packet.SetInt32(32, ciDesc);

  if(Args and Args.size())
  
    Packet.SetInt32(36, 0);
    Packet.SetInt16(40, 1);
    Packet.SetInt16(42, 0);

    var argText := "";
    var i;

    for(i:=1; i<=Args.size(); i:=i+1)
      argText := argText + "\t"+Args[i];
    endfor

    Packet.SetUnicodeStringFlipped(44, CAscZ(argText), 1);

    Packet.SetInt16(46+len(argText)*2, 1);
    Packet.SetInt16(48+len(argText)*2, 0);
  
  else

    Packet.SetInt32(36, 0);
    Packet.SetInt32(40, 0);
    Packet.SetInt16(44, 0);

  endif

  Packet.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------------//

function SendDelBuff(who, idIcon)
  
  if(!IsPlayer(who))
    return;
  endif

  var Packet := CreatePacket(0xDF, MSGLEN_VARIABLE);

  Packet.SetInt32(3, who.serial);
  Packet.SetInt16(7, idIcon);
  Packet.SetInt16(9, 0x0);

  Packet.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------------//

function CheckBuffsOnLogon(who)

  var Mods := GetAllMods();

  foreach ModType in Mods
    if(CanMod(who, ModType) == 0)
      var duration := GetModDuration(who, ModType) - (ReadGameClock() - GetModStartTime(who, ModType));
      NotifyAddBuff(who, ModType, GetModAmount(who, ModType), duration);
    endif
  endforeach

  if(IsReflected(who))
    SendAddBuff(who, BFICON_MAGIC_REFLECT, 0, 1075817, CLILOC_EMPTY, 0);
  elseif(IsDispersed(who))
    SendAddBuff(who, BFICON_MAGIC_REFLECT, 0, 1075817, CLILOC_EMPTY, 0);
  endif

endfunction

//-----------------------------------------------------------------------------

function NotifyAddBuff(who, type, amt, duration)

  if(!IsPlayer(who))
    return;
  endif

  duration := Max(duration, 1); // lepiej nie wysylac z minusowym durationem bo wypierdoli wspanialego clienta..

  case(type)

    MOD_DEX:            if(amt < 0)
                          SendAddBuff(who, BFICON_CLUMSY, duration, 1075831, 1075832, array(amt * -100 / GetBaseDexterity(who)));
                        else
                          SendAddBuff(who, BFICON_AGILITY, duration, 1075841, 1075842, array(amt * 100 / GetBaseDexterity(who)));
                        endif

    MOD_INT:            if(amt < 0)
                          SendAddBuff(who, BFICON_FEEBLEMIND, duration, 1075833, 1075834, array(amt * -100 / GetBaseIntelligence(who)));
                        else
                          SendAddBuff(who, BFICON_CUNNING, duration, 1075843, 1075844, array(amt * 100 / GetBaseIntelligence(who)));
                        endif

    MOD_STR:            if(amt < 0)
                          SendAddBuff(who, BFICON_WEAKEN, duration, 1075837, 1075838, array(amt * -100 / GetBaseStrength(who)));
                        else
                          SendAddBuff(who, BFICON_STRENGTH, duration, 1075845, 1075846, array(amt * 100 / GetBaseStrength(who)));
                        endif

    MOD_LIFE:           if(amt < 0)
                          SendAddBuff(who, BFICON_DEC_LIFE, duration, CLILOC_EMPTY, CLITMOD_LIFE, array(amt));
                        else
                          SendAddBuff(who, BFICON_LIFE, duration, CLILOC_EMPTY, CLITMOD_LIFE, array(amt));
                        endif

    MOD_MANA:           if(amt < 0)
                          SendAddBuff(who, BFICON_DEC_MANA, duration, CLILOC_EMPTY, CLITMOD_MANA, array(amt));
                        else
                          SendAddBuff(who, BFICON_MANA, duration, CLILOC_EMPTY, CLITMOD_MANA, array(amt));
                        endif

    MOD_STAMINA:        if(amt < 0)
                          SendAddBuff(who, BFICON_DEC_STAMINA, duration, CLILOC_EMPTY, CLITMOD_STAMINA, array(amt));
                        else
                          SendAddBuff(who, BFICON_STAMINA, duration, CLILOC_EMPTY, CLITMOD_STAMINA, array(amt));
                        endif

    MOD_LIGHT:          SendAddBuff(who, BFICON_NIGHTSIGHT, duration, 1075643, CLILOC_EMPTY, 0);

    MOD_PARA:           SendAddBuff(who, BFICON_PARA, duration, 1075827, 1075828, 0);

    MOD_INCO:           SendAddBuff(who, BFICON_INCO, duration, 1075819, 1075820, 0);

    MOD_AR:             SendAddBuff(who, BFICON_PROTECTION, duration, 1075814, CLILOC_EMPTY, 0);

    MOD_RA:             SendAddBuff(who, BFICON_REACTIVE_ARMOR, duration, 1075812, CLILOC_EMPTY, 0);

    MOD_GRAPHIC:        SendAddBuff(who, BFICON_POLYMORPH, duration, 1075824, CLILOC_EMPTY, 0);

    MOD_DISARM:         SendAddBuff(who, BFICON_DISARM, duration, CLITMOD_DISARM, CLILOC_EMPTY, 0);

    MOD_HP_REGEN:       SendAddBuff(who, BFICON_HP_REGEN, duration, CLITMOD_HP_REGEN, CLILOC_EMPTY, 0);

    MOD_MANA_REGEN:     SendAddBuff(who, BFICON_MANA_REGEN, duration, CLITMOD_MANA_REGEN, CLILOC_EMPTY, 0);

    MOD_STAM_REGEN:     SendAddBuff(who, BFICON_STAM_REGEN, duration, CLITMOD_STAMINA_REGEN, CLILOC_EMPTY, 0);

    MOD_STUPOR:         SendAddBuff(who, BFICON_STUPOR, duration, CLITMOD_STUPOR, CLILOC_EMPTY, 0);

    MOD_POISON_PROT:    SendAddBuff(who, BFICON_POISON_PROT, duration, 1061161, 1072385, array(amt*10));

    MOD_HGHOSTS:        SendAddBuff(who, BFICON_HEAR_GHOSTS, duration, CLITMOD_HEAR_GHOSTS, CLILOC_EMPTY, 0);

    MOD_POISON_DMG:     SendAddBuff(who, BFICON_POISON_DMG, duration, CLILOC_EMPTY, CLITMOD_POISON_DMG, array(amt));

    MOD_CTH:            if(amt > 0)
                          SendAddBuff(who, BFICON_CTH, duration, CLILOC_EMPTY, CLITMOD_CTH_REDUCED, 0);
                        else
                          SendAddBuff(who, BFICON_CTH, duration, CLILOC_EMPTY, CLITMOD_PENETRATE, array(amt/10));
                        endif

    MOD_FASTER_CAST:    SendAddBuff(who, BFICON_FASTER_CAST, duration, CLILOC_EMPTY, CLITMOD_FASTER_CAST, array((amt*250)+"ms"));

    MOD_FIRE_BLOOD:     SendAddBuff(who, BFICON_FIRE_BLOOD, duration, CLITMOD_FIRE_BLOOD, CLILOC_EMPTY, 0);

    MOD_RETORT:         SendAddBuff(who, BFICON_RETORT, duration, CLILOC_EMPTY, CLILOC_VARIABLE, array("Otrzymane obrazenia zamieniaja sie na szanse na oddanie silniejszego ciosu"));

    MOD_CRIT_HIT:       SendAddBuff(who, BFICON_CRIT_HIT, duration, CLILOC_EMPTY, CLILOC_VARIABLE, array("Zwieksza szanse na krytyczne uderzenie"));

    MOD_STAM_DMG:       SendAddBuff(who, BFICON_STAM_DMG, duration, CLILOC_EMPTY, CLILOC_VARIABLE, array("Niszczy wytrzymalosc wroga"));

    MOD_CHARM_ANIMAL:   SendAddBuff(who, BFICON_CHARM_ANIMAL, duration, CLITMOD_CHARM_ANIMAL, CLIDMOD_CHARM_ANIMAL, 0);

    MOD_MAGIC_SHIELD:   SendAddBuff(who, BFICON_MAGIC_SHIELD, duration, CLITMOD_MAGIC_SHIELD, CLILOC_EMPTY, 0);

    MOD_DISMOUNT_RES:   SendAddBuff(who, BFICON_DISMOUNT_RES, duration, CLITMOD_DISMOUNT_RES, CLIDMOD_DISMOUNT_RES, 0);

    MOD_RED_SPELL:      SendAddBuff(who, BFICON_RED_SPELL, duration, CLITMOD_RED_SPELL, CLILOC_EMPTY, 0);

    MOD_FIRE_DMG:       SendAddBuff(who, BFICON_FIRE_DMG, duration, CLILOC_EMPTY, CLITMOD_FIRE_DMG, array(amt));

    MOD_MONSTER_AR:     SendAddBuff(who, BFICON_MONSTER_AR , duration, 1075814, CLILOC_EMPTY, 0); // cliloc z prota

    MOD_TREE_SUPPORT:   SendAddBuff(who, BFICON_TREE_SUPPORT , duration, 1075814, CLILOC_EMPTY, 0); // cliloc z prota

    MOD_LIFE_STOLEN:    SendAddBuff(who, BFICON_LIFE_STOLEN , duration , CLILOC_EMPTY, CLITMOD_LIFE_STOLEN , array(amt));

    MOD_COLD_DMG:       SendAddBuff(who, BFICON_COLD_DMG , duration, CLILOC_EMPTY, CLITMOD_COLD_DMG, array(amt));

    MOD_COLOR:          SendAddBuff(who, BFICON_COLOR , duration , CLITMOD1_COLOR, CLITMOD2_COLOR, 0);

    MOD_LIGHTRESIST:    SendAddBuff(who, BFICON_ICEBARRIER , duration , CLITMOD_ICEBARRIER, CLILOC_EMPTY, 0);

    MOD_UNDEAD_SLAY:    SendAddBuff(who, BFICON_CHAMPION , duration , CLITMOD1_CHAMPION, CLITMOD2_CHAMPION, 0);

    MOD_UNTOUCHABLE:    SendAddBuff(who, BFICON_UNTOUCHABLE , duration , CLITMOD1_UNTOUCHABLE , CLITMOD2_UNTOUCHABLE , 0);

    MOD_ORDER_SLAY:     SendAddBuff(who, BFICON_ORDER_SLAY , duration , CLILOC_EMPTY , CLITMOD_ORDER_SLAY , array(amt));

    MOD_EXPLO_DMG:      SendAddBuff(who, BFICON_FIRE_DMG , duration , CLILOC_EMPTY , CLITMOD_EXPLO_DMG , array(amt));

    MOD_DAEMON_SLAY:     SendAddBuff(who, BFICON_ANIMAL_FORM , duration , CLITMOD_DEMON_SLAYER , CLITMOD_ORDER_SLAY , array(amt));

    MOD_MANA_DMG:       SendAddBuff(who, BFICON_FEEBLEMIND , duration , CLILOC_EMPTY , CLITMOD_MANA_DRAIN , array(amt));

    MOD_MULTI_SHOT:     SendAddBuff(who, BFICON_MULTI_SHOT , duration , CLILOC_EMPTY , CLILOC_VARIABLE , array("Rozproszenie strzal")); 

    MOD_PENETRATE:      if(amt > 0)
                          SendAddBuff(who, BFICON_ICEBARRIER , duration , CLILOC_EMPTY , CLITMOD_CTH , array(amt/10)); 
                        else
                          SendAddBuff(who, BFICON_ICEBARRIER , duration , CLILOC_EMPTY , CLITMOD_PENETRATE_RED , array(amt/10));
                        endif
    
  
  endcase

endfunction

//-----------------------------------------------------------------------------

function NotifyDelBuff(who, type, amt := 0)

  if(!IsPlayer(who))
    return;
  endif

  case(type)

    MOD_DEX:            if(amt < 0)
                          SendDelBuff(who, BFICON_CLUMSY);
                        else
                          SendDelBuff(who, BFICON_AGILITY);
                        endif

    MOD_INT:            if(amt < 0)
                          SendDelBuff(who, BFICON_FEEBLEMIND);
                        else
                          SendDelBuff(who, BFICON_CUNNING);
                        endif

    MOD_STR:            if(amt < 0)
                          SendDelBuff(who, BFICON_WEAKEN);
                        else
                          SendDelBuff(who, BFICON_STRENGTH);
                        endif

    MOD_LIFE:           if(amt < 0)
                          SendDelBuff(who, BFICON_DEC_LIFE);
                        else
                          SendDelBuff(who, BFICON_LIFE);
                        endif

    MOD_MANA:           if(amt < 0)
                          SendDelBuff(who, BFICON_DEC_MANA);
                        else
                          SendDelBuff(who, BFICON_MANA);
                        endif

    MOD_STAMINA:        if(amt < 0)
                          SendDelBuff(who, BFICON_DEC_STAMINA);
                        else
                          SendDelBuff(who, BFICON_STAMINA);
                        endif

    MOD_LIGHT:          SendDelBuff(who, BFICON_NIGHTSIGHT);

    MOD_PARA:           SendDelBuff(who, BFICON_PARA);

    MOD_INCO:           SendDelBuff(who, BFICON_INCO);

    MOD_AR:             SendDelBuff(who, BFICON_PROTECTION);

    MOD_RA:             SendDelBuff(who, BFICON_REACTIVE_ARMOR);

    MOD_GRAPHIC:        SendDelBuff(who, BFICON_POLYMORPH);

    MOD_DISARM:         SendDelBuff(who, BFICON_DISARM);

    MOD_MANA_REGEN:     SendDelBuff(who, BFICON_MANA_REGEN);

    MOD_HP_REGEN:       SendDelBuff(who, BFICON_HP_REGEN);

    MOD_STAM_REGEN:     SendDelBuff(who, BFICON_STAM_REGEN);

    MOD_STUPOR:         SendDelBuff(who, BFICON_STUPOR);

    MOD_POISON_PROT:    SendDelBuff(who, BFICON_POISON_PROT);

    MOD_HGHOSTS:        SendDelBuff(who, BFICON_HEAR_GHOSTS);

    MOD_POISON_DMG:     SendDelBuff(who, BFICON_POISON_DMG);

    MOD_CTH:            SendDelBuff(who, BFICON_CTH);

    MOD_FASTER_CAST:    SendDelBuff(who, BFICON_FASTER_CAST);

    MOD_FIRE_BLOOD:     SendDelBuff(who, BFICON_FIRE_BLOOD);

    MOD_RETORT:         SendDelBuff(who, BFICON_RETORT);

    MOD_CRIT_HIT:       SendDelBuff(who, BFICON_CRIT_HIT);

    MOD_STAM_DMG:       SendDelBuff(who, BFICON_STAM_DMG);

    MOD_CHARM_ANIMAL:   SendDelBuff(who, BFICON_CHARM_ANIMAL);

    MOD_DISMOUNT_RES:   SendDelBuff(who, BFICON_DISMOUNT_RES);

    MOD_RED_SPELL:      SendDelBuff(who, BFICON_RED_SPELL);

    MOD_MAGIC_SHIELD:   SendDelBuff(who, BFICON_MAGIC_SHIELD);
    
    MOD_FIRE_DMG:       SendDelBuff(who, BFICON_FIRE_DMG);
    
    MOD_MONSTER_AR:     SendDelBuff(who, BFICON_MONSTER_AR);      
    
    MOD_TREE_SUPPORT:   SendDelBuff(who, BFICON_TREE_SUPPORT);  
    
    MOD_LIFE_STOLEN:    SendDelBuff(who, BFICON_LIFE_STOLEN);

    MOD_COLD_DMG:       SendDelBuff(who, BFICON_COLD_DMG);

    MOD_COLOR:          SendDelBuff(who, BFICON_COLOR);

    MOD_LIGHTRESIST:    SendDelBuff(who, BFICON_ICEBARRIER);

    MOD_UNDEAD_SLAY:    SendDelBuff(who, BFICON_CHAMPION);

    MOD_UNTOUCHABLE:    SendDelBuff(who, BFICON_UNTOUCHABLE);

    MOD_ORDER_SLAY:     SendDelBuff(who, BFICON_ORDER_SLAY);

    MOD_EXPLO_DMG:      SendDelBuff(who, BFICON_FIRE_DMG);

    MOD_DAEMON_SLAY:    SendDelBuff(who, BFICON_ANIMAL_FORM);

    MOD_MANA_DMG:       SendDelBuff(who, BFICON_FEEBLEMIND);

    MOD_MULTI_SHOT:     SendDelBuff(who, BFICON_MULTI_SHOT); 

    MOD_PENETRATE:      SendDelBuff(who, BFICON_ICEBARRIER);

  endcase

endfunction

//-----------------------------------------------------------------------------------//
