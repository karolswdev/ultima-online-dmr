//=====================================================================
//---------------------------------------------------------------------
// Alignments
//---------------------------------------------------------------------
//=====================================================================

const CITYAL_GOOD                      := "good";
const CITYAL_EVIL                      := "evil";
const CITYAL_NEUTRAL                   := "neutral";

const NPCAL_GOOD                       := 2;
const NPCAL_EVIL                       := 1;
const NPCAL_NEUTRAL                    := 0;

//=====================================================================
//---------------------------------------------------------------------
// Realms
//---------------------------------------------------------------------
//=====================================================================

const BRITB_REALM                      := "britannia";
const BRITA_REALM                      := "britannia_alt";
const ILSHE_REALM                      := "ilshenar";
const MALAS_REALM                      := "malas";
const TOKUNO_REALM                     := "tokuno";

//=====================================================================
//---------------------------------------------------------------------
// Items Constants
//---------------------------------------------------------------------
//=====================================================================

const SKILL_TO_MAKE_EXP                := 80;
const EXP_QUALITY_MULTIPLIER           := 1.5;
const SKILL_TO_NAMED_EXP               := 95;
const EXP_ITEM_NAME                    := "Wyjatkowej jakosci";
const MAX_ITEMTRAIN_SKVAL              := 40;
const EXP_CITY_GAIN                    := 25;
const ITEM_START_SERIAL                := 0x40000000;
const DEF_CONTAINER_MAX_ITEMS          := 150;

const MAX_VISIBLE_RANGE                := 18;

//=====================================================================
//---------------------------------------------------------------------
// Times
//---------------------------------------------------------------------
//=====================================================================

const MURD_YELL_TIME                   := 180;
const ACC_MINCHAR_TIME                 := 172800;
const ACC_DELETE_TIME                  := 5184000; //60 days
const ACC_NODEL_TIME                   := 31536000; // 365 dni = rok
const CHAR_DELETE_TIME                 := 604800;
const ACC_ACTIVE_TIME                  := 864000; //10 days

const ACC_YOUNG_TIME                   := 172800; //2 days
const CHAR_DELYOUNG_TIME               := 43200; // 12h
const MAX_CHAR_ON_ACC                  := 4;

const ACC_TBANEXPIRE                   := 604800;
const OSZUST_TIME                      := 604800;

const LONG_COUNT_DECAY_TIME            := 86400; //1 day
const LONG_COUNT_DECAY_TIME_MAX        := 432000; //5 days
const SHORT_COUNT_DECAY_TIME           := 21600; //6h
const MURD_TIME_MULTIPLIER             := 10800; // 3h za jedno zabojstwo
const MAX_REPORT_TIME                  := 7200;
const HOW_LONG_WITHOUT_KILL            := 604800; //7 days
const DEF_HUNGER_TIME                  := 600;

const ITEM_DECAY_TIME                  := 1200; // 20 min

//=====================================================================
//---------------------------------------------------------------------
// Misc
//---------------------------------------------------------------------
//=====================================================================

const SPEECH_RANGE                     := 2;
const MAX_LOOK_LOOP                    := 10;
const FAMILY_MAXSTR                    := 500;
const YOUNG_UNTOUCH_MAXSTR             := 250;

const MAX_MURD_TIMES                   := 5;

const DEF_ITEMHP                       := 20;

const CR_TEMP_ITEM_X                   := 6104;
const CR_TEMP_ITEM_Y                   := 1182;
const CR_TEMP_ITEM_Z                   := 0;
const CR_TEMP_ITEM_OBJTYPE             := 0xef01;
const CR_TEMP_ITEM_REALM               := "britannia";

const CR_TEMP_NPC_X                    := 6106;
const CR_TEMP_NPC_Y                    := 1182;
const CR_TEMP_NPC_Z                    := 0;
const CR_TEMP_NPC_REALM                := "britannia";

const DRUNK_LOW_LVL                    := 1;
const DRUNK_MID_LVL                    := 2;
const DRUNK_HIGH_LVL                   := 3;
const DRUNK_SUPER_LVL                  := 4;

const DRUNK_MAX_LVL                    := 5;

//=====================================================================
//---------------------------------------------------------------------
// Invisible
//---------------------------------------------------------------------
//=====================================================================

const INVIS_ITEM_GRAP                  := 8611;
const INVIS_MOB_GRAP                   := 731;

const MINVIS_MOB_GRAP                  := 886;

//---------------------------------------------------------------------

const AGE_COUNT                        := 129600; // 36h - 1.5 days
const START_AGE                        := 20;     // 20 years old
const NPC_AGE_COUNT                    := 604800; // 7 days
const NPC_START_AGE                    := 1;
const NPC_MAX_AGE                      := 100;
const ADULT_AGE                        := 23;

const SKCAP_TIME_BARIER                := 0; // zaczynajac od 21 lat (od 3d online)
const SKCAP_TIME_DIVIDER               := AGE_COUNT / 10; // co 1 rok +10 do skillcapa, dzielone na 10 - w wieku 30 lat dostanie 800.
const SKCAP_ADD_BASE_POINTS            := 10;

const STCAP_TIME_BARIER                := AGE_COUNT * 5; // zaczynajac od 25 lat
const STCAP_TIME_DIVIDER               := AGE_COUNT * 2; // co rok +1 do statcapa
const STCAP_ADD_BASE_POINTS            := 20;

const DEFAULT_SKILL_CAP                := 8000;
const DEFAULT_STAT_CAP                 := 2000;
const MAX_SKILL_CAP                    := 9000;
const MAX_STAT_CAP                     := 2250;

//---------------------------------------------------------------------

const NPCMODCORE_SCNAME                := ":magiceffects:npcmodcore";
const MAKROCHECK_SCNAME                := ":makrocheck:makrocheck";
const MAILCONTROL_SCNAME               := ":mail:mail_ctrl";
const CHCORE_SCNAME                    := ":champions:champcore";
const RESLOGON_SCNAME                  := ":reslogon:checkrl";
const SHARDINFO_SCNAME                 := "::shardinfo";

//=====================================================================
//---------------------------------------------------------------------
// AI Npcs
//---------------------------------------------------------------------
//=====================================================================

const WANDER_TIME                      := 30;
const NPCWALK_MAX_RANGE                := 75;
const GUARD_WANDER_TIME                := 15;
const PEACEMAKING_DELAY                := 3;
const DEF_WAITTIME                     := 120;
const DEF_WILD_WTIME                   := 20;
const DEF_WALK_PAUSE_TIME              := 60;
const SHORT_WALK_PAUSE                 := 30;
const GUARD_WAITTIME                   := 15;
const GUARD_REMEMBER_TIME              := 10800; // 3h

const DEF_SPELLTIME                    := 4;
const DEF_FLEEPOINT                    := 10;

//=====================================================================
//---------------------------------------------------------------------
// Guard Remind & Chase
//---------------------------------------------------------------------
//=====================================================================

const PROP_GUARD_CHASE                 := "GuardChase";
const GDCF_ALLCITIES                   := 0x00000001;

const PROP_GUARD_REMTIME               := "GuardRemindTime";
const PROP_GUARD_REMIND                := "GuardRemind";
const PROP_GUARD_CRIMINALS             := "Criminals";

const GUARD_REMIND_DELAY               := 60;
const GUARD_REMIND_TIMEOFF             := 84600;
const MAX_GUARD_REMIND                 := 10;

//---------------------------------------------------------------------
// Guard Memory Reasons
//---------------------------------------------------------------------

const GMR_CHASING                      := 1;
const GMR_ANIMAL_INB                   := 2;
const GMR_NOT_MURD                     := 3;
const GMR_TO_WEAK_MURD                 := 4;
const GMR_MURD                         := 5;
const GMR_CRIMINAL                     := 6;
const GMR_STEALING                     := 7;
const GMR_ATTACKED                     := 8;
const GMR_INMEMORY                     := 9;
const GMR_CITYCHASING                  := 10;
const GMR_NPCATTACKED                  := 11;
const GMR_TAMEDFIGHT                   := 12;
const GMR_PLAYER_ATTACK                := 13;
const GMR_MAGIC_CAST                   := 14;

//---------------------------------------------------------------------

const PROP_NPC_MEMBER                  := "Member";
const NPC_MEMBER_TIME                  := 43200;

//---------------------------------------------------------------------
// Crier News Flags
//---------------------------------------------------------------------

const CNF_CHAOS                        := 0x01;
const CNF_ORDER                        := 0x02;
const CNF_BRIT_ALT                     := 0x04;

//=====================================================================
//---------------------------------------------------------------------
// Player Hooks
//---------------------------------------------------------------------
//=====================================================================

const PROP_MOVEHOOK_PID                := "#MoveHookPid";
const PROP_PLAYERHOOK_PID              := "#PlayerHookPid";
const PROP_PLAYERSPEECH_PID            := "#PlayerSpeechPid";
const PROP_ATTRCORE_PID                := "#AttrCorePid";
const PROP_MODCORE_PID                 := "#ModCorePid";
const PROP_REGENHOOK_PID               := "#RegenHookPid"; 

const SCNAME_MOVEHOOK                  := ":hooks:movehook";
const SCNAME_PLAYERHOOK                := ":hooks:playerhook";
const SCNAME_PLAYERSPEECH              := ":hooks:playerspeech";
const SCNAME_ATTRCORE                  := ":hooks:attrcore";
const SCNAME_MODCORE                   := ":magiceffects:modcore";

//=====================================================================
//---------------------------------------------------------------------
// Restricted Logon
//---------------------------------------------------------------------
//=====================================================================

const PROP_ARLOGON                     := "AlwaysRestrictedLogon";
const PROP_RESLOGON                    := "#RestrictedLogon";

//---------------------------------------------------------------------

const GITEMS_STORAGE                   := "GivenItems";

const MERC_FSBOX                       := " Box";
const MERC_PBBOX                       := "OldPB Box";
const MERC_STORAGE                     := "Merchant Box";
const VENDOR_STORAGE                   := "Merchant Storage";

const VENDOR_FS_CONTNAME               := " FS";
const VENDOR_PB_CONTNAME               := " PB";
const VENDOR_1C_CONTNAME               := " 1C";

const BANK_STORAGE                     := "World Bank";
const BANKBOX_PREFIX                   := "Bankbox of ";
const STOREBOX_PREFIX                  := "Storebox of ";

const ESCROW_STORAGE                   := "World Escrow";
const ESCROWBOX_PREFIX                 := "Escrow of ";

const MAIL_STORAGE                     := "World Mail";
const MAILBOX_PREFIX                   := "Mailbox of ";

//---------------------------------------------------------------------

const LIFETIME_UNTAMED                 := 259200;
const LIFETIME_SUMMONED                := 86400;

//---------------------------------------------------------------------

const CRIMSLOG_MAXSIZE                 := 20;

//---------------------------------------------------------------------

const RUNSPEED_NORMAL                  := 0;
const RUNSPEED_FAST                    := 1;
const RUNSPEED_SLOW                    := 2;
const RUNSPEED_HYBRID                  := 3;

//---------------------------------------------------------------------
