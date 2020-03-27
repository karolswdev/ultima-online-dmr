//================================================================================
//--------------------------------------------------------------------------------
// Music
//--------------------------------------------------------------------------------
//================================================================================

const SFX_MUSIC_LUTE_GOOD              := 0x4D;
const SFX_MUSIC_LUTE_FAIL              := 0x4E;
const SFX_MUSIC_DRUM_GOOD              := 0x39;
const SFX_MUSIC_DRUM_FAIL              := 0x3A;
const SFX_MUSIC_HARP_LARGE_GOOD        := 0x44;
const SFX_MUSIC_HARP_LARGE_FAIL        := 0x45;
const SFX_MUSIC_HARP_LAP_GOOD          := 0x46;
const SFX_MUSIC_HARP_LAP_FAIL          := 0x47;
const SFX_MUSIC_TAMBORINE_GOOD         := 0x53;
const SFX_MUSIC_TAMBORINE_FAIL         := 0x54;

//================================================================================
//--------------------------------------------------------------------------------
// Spells & Magic Sounds
//--------------------------------------------------------------------------------
//================================================================================

const SFX_SPELL_CLUMSY                 := 0x1E0;
const SFX_SPELL_CREATE_FOOD            := 0x1E3;
const SFX_SPELL_FEEBLEMIND             := 0x1E5;
const SFX_SPELL_HEAL                   := 0x1F3;
const SFX_SPELL_MAGIC_ARROW            := 0x1E6;
const SFX_SPELL_NIGHT_SIGHT            := 0x1E4;
const SFX_SPELL_REACTIVE_ARMOR         := 0x1F3;
const SFX_SPELL_WEAKEN                 := 0x1E7;
const SFX_SPELL_AGILITY                := 0x28F;
const SFX_SPELL_CUNNING                := 0x1EC;
const SFX_SPELL_CURE                   := 0x1E1;
const SFX_SPELL_HARM                   := 0x1F2;
const SFX_SPELL_MAGIC_TRAP             := 0x1F0;
const SFX_SPELL_MAGIC_UNTRAP           := 0x1F1;
const SFX_SPELL_PROTECTION             := 0x1EE;
const SFX_SPELL_STRENGTH               := 0x1EF;
const SFX_SPELL_BLESS                  := 0x1EB;
const SFX_SPELL_FIREBALL               := 0x160;
const SFX_SPELL_MAGIC_LOCK             := 0x1F5;
const SFX_SPELL_TELEKINESIS            := 0x1F6;
const SFX_SPELL_TELEPORT               := 0x1FF;
const SFX_SPELL_POISON                 := 0x247;
const SFX_SPELL_UNLOCK                 := 0x200;
const SFX_SPELL_WALL_OF_STONE          := 0x1F7;
const SFX_SPELL_ARCH_CURE              := 0x1E9;
const SFX_SPELL_ARCH_PROTECTION        := 0x1F8;
const SFX_SPELL_CURSE                  := 0x1E2;
const SFX_SPELL_FIRE_FIELD             := 0x226;
const SFX_SPELL_GREATER_HEAL           := 0x203;
const SFX_SPELL_LIGHTNING              := 0x02A;
const SFX_SPELL_MANA_DRAIN             := 0x1F9;
const SFX_SPELL_RECALL                 := 0x1FD;
const SFX_SPELL_BLADE_SPIRITS          := 0x213;
const SFX_SPELL_ENERGY_FIELD           := 0x211; // used to say dispel field?
const SFX_SPELL_INCOGNITO              := 0x1ED;
const SFX_SPELL_MAGIC_REFLECTION       := 0x1E8;
const SFX_SPELL_MAGIC_REFLECT          := 0x1EA;
const SFX_SPELL_MIND_BLAST             := 0x214;
const SFX_SPELL_PARALYZE               := 0x205;
const SFX_SPELL_POISON_FIELD           := 0x20D; // was 227 (??)
const SFX_SPELL_SUMMON_CREATURE        := 0x216;
const SFX_SPELL_DISPEL                 := 0x202;
const SFX_SPELL_ENERGY_BOLT            := 0x20B;
const SFX_SPELL_EXPLOSION              := 0x208;
const SFX_SPELL_INVISIBILITY           := 0x204;
const SFX_SPELL_MARK                   := 0x1FB;
const SFX_SPELL_MASS_CURSE             := 0x1FC;
const SFX_SPELL_PARALYZE_FIELD         := 0x212;
const SFX_SPELL_REVEAL                 := 0x1FE;
const SFX_SPELL_CHAIN_LIGHTNING        := 0x30;
const SFX_SPELL_PARA_FIELD             := 0x20C;
const SFX_SPELL_FLAME_STRIKE           := 0x209;
const SFX_SPELL_GATE_TRAVEL            := 0x20F;
const SFX_SPELL_MANA_VAMPIRE           := 0x1FA;
const SFX_SPELL_MASS_DISPEL            := 0x20A;
const SFX_SPELL_METEOR_SWARM           := 0x161;
const SFX_SPELL_POLYMORPH              := 0x20A;
const SFX_SPELL_EARTHQUAKE             := 0x20E;
const SFX_SPELL_ENERGY_VORTEX          := 0x213;
const SFX_SPELL_RESSURECTION           := 0x215;
const SFX_SPELL_SUMMON_ELEMENTAL       := 0x218;
const SFX_SPELL_SUMMON_DAEMON          := 0x217;
const SFX_SPELL_ICE_BOLT               := 0x28F;
const SFX_SPELL_WRAITHFORM             := 0x210;
const SFX_SPELL_RAISEDEAD              := 0x22b;
const SFX_SPELL_FAIL                   := 0x05d;

const SFX_LIGHTNING_STRIKE             := 0x5C0;
const SFX_LIGHTNING_ARROW              := 0x51D;

const SFX_MEDITATION                   := 0x0fa;
const SFX_MEDIT_END                    := 0x0f6;

//================================================================================
//--------------------------------------------------------------------------------
// Skill Sounds
//--------------------------------------------------------------------------------
//================================================================================

const SFX_SKILL_SPIRITSPEAK            := 0x24B;
const SFX_SKILL_LUMBERJACK             := 0x13F;
const SFX_EXECUTION                    := 0x51D;

//================================================================================
//--------------------------------------------------------------------------------
// Item Sounds
//--------------------------------------------------------------------------------
//================================================================================

const SFX_TRAP_NEEDLE                  := 0x225;
const SFX_TRAP_POISON                  := 0x206;
const SFX_TRAP_EXPLOSION               := 0x208;
const SFX_ANVIL                        := 0x2B;
const SFX_PICK                         := 0x13B;
const SFX_HAMMER                       := 0x43;
const SFX_GLASSBREAK4                  := 0x42;
const SFX_HARP_SUCC                    := 0x46;
const SFX_LUTE_SUCC                    := 0x4D;
const SFX_DRUM_SUCC                    := 0x39;
const SFX_TAMB_SUCC                    := 0x53;

const SFX_HARP_FAIL                    := 0x47;
const SFX_LUTE_FAIL                    := 0x4E;
const SFX_DRUM_FAIL                    := 0x3A;
const SFX_TAMB_FAIL                    := 0x54;

const SFX_MILL                         := 0x248;

const SFX_CRYSTAL_BALL                 := 0x0F7;

//================================================================================
//--------------------------------------------------------------------------------
// Door Sounds
//--------------------------------------------------------------------------------
//================================================================================

const SFX_OPEN_WOODEN_DOOR             := 0xEB;
const SFX_CLOSE_WOODEN_DOOR            := 0xF2;
const SFX_OPEN_METAL_DOOR              := 0xED;
const SFX_CLOSE_METAL_DOOR             := 0xF4;
const SFX_OPEN_ICE_DOOR                := 0xEE;
const SFX_CLOSE_ICE_DOOR               := 0xF5;

const SFX_FEMALE_DMGSOUND              := 0x325;
const SFX_MALE_DMGSOUND                := 0x435;

//================================================================================
//--------------------------------------------------------------------------------
// New Magic Sounds
//--------------------------------------------------------------------------------
//================================================================================

const SFX_FEINT                        := 0x526;
const SFX_FOCUSATTACK                  := 0x510;
const SFX_FREN_WHIRLWIND               := 0x527;
const SFX_WHIRLWIND                    := 0x520;
const SFX_FIREBLAST                    := 0x56E; // hydra special
const SFX_IMMOLATION                   := 0x5CB;
const SFX_KIATTACK                     := 0x511;
const SFX_MIRROR_IMAGE                 := 0x512;
const SFX_SHADOW_JUMP                  := 0x513;
const SFX_SMOKE_BOMB                   := 0x533;
const SFX_THUNDER_STORM                := 0x5CF;
const SFX_GIFT_OF_RENEWAL              := 0x5CA;
const SFX_PUNCH                        := 0x534;

const SFX_FIRE_PREPARE                 := 0x1F4;
const SFX_FIRE_STRIKE                  := 0x44C; // sound used a lot
const SFX_FIRE1                        := 0x346;
const SFX_FIRE2                        := 0x1DE;
const SFX_WILD_FIRE                    := 0x5D0;

const SFX_ARCANECIRCLE                 := 0x5C1;
const SFX_ARCANE_POWER_UP              := 0x5C2;
const SFX_ARCANE_POWER_DOWN            := 0x5C3;

const SFX_ATTUNE                       := 0x5C4;
const SFX_BLADEWEAVE                   := 0x5BD;
const SFX_CONFIDENCE                   := 0x51B;

//================================================================================
//--------------------------------------------------------------------------------
// Npcs Sounds
//--------------------------------------------------------------------------------
//================================================================================

const SFX_WOLF_SCREAM                  := 0x0E7;

//================================================================================
//--------------------------------------------------------------------------------
// Nature Sounds
//--------------------------------------------------------------------------------
//================================================================================

const SFX_NATURE_WAVE1                 := 0x013;
const SFX_NATURE_WAVE2                 := 0x014;

//--------------------------------------------------------------------------------

const SFX_CUT_HARD                     := 0x57;

//--------------------------------------------------------------------------------

const SFX_DMG_ABSORBED                 := 0x0FD;

//--------------------------------------------------------------------------------
