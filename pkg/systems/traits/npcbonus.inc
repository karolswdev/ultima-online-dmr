//-----------------------------------------------------------------------------

include "common";

//-----------------------------------------------------------------------------
// Trait Npc Bonus
//-----------------------------------------------------------------------------

const TNB_DAMAGE := 1;
const TNB_ARMOR  := 2;

//-----------------------------------------------------------------------------

function GetTraitForNpc(Npc)

  var NpcGroup := GetNpcConfig(Npc).NpcGroup;

  if(!NpcGroup)
    return 0;
  endif

  var NpcTraits := ReadConfigFile(CFG_NPCTRAITS);
  var Trait := NpcTraits[NpcGroup].Trait;

  if(!Trait)
    return 0;
  endif

  var Cfg := ReadConfigFile(CFG_TRAITS);
  return Cfg[Trait];

endfunction

//-----------------------------------------------------------------------------

function GetTraitNpcBonus(Player, TraitGroup, BonusId)

  var Bonus := GetObjProperty(Player, "TNB_"+TraitGroup+"_"+BonusId);
  
  if(Bonus)
    return CDbl(Bonus);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function SetTraitNpcBonus(Player, TraitGroup, BonusId, Amt)

  SetObjProperty(Player, "TNB_"+TraitGroup+"_"+BonusId, CDbl(Amt));

endfunction

//-----------------------------------------------------------------------------

function EraseTraitNpcBonus(Player, TraitGroup, BonusId)

  EraseObjProperty(Player, "TNB_"+TraitGroup+"_"+BonusId);

endfunction

//-----------------------------------------------------------------------------

function GetDamageBonusForNpc(Player, Npc)

  var Trait := GetTraitForNpc(Npc);

  if(!Trait)
    return 0;
  endif

  return GetTraitNpcBonus(Player, Trait.Name, TNB_DAMAGE);

endfunction

//-----------------------------------------------------------------------------

function GetArmorBonusForNpc(Player, Npc)

  var Trait := GetTraitForNpc(Npc);

  if(!Trait)
    return 0;
  endif

  return GetTraitNpcBonus(Player, Trait.Name, TNB_ARMOR);

endfunction

//-----------------------------------------------------------------------------
