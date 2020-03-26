//-----------------------------------------------------------------------------
// Global
//-----------------------------------------------------------------------------

const NGD_DATA               := ":npcguilds:npcguilds";
const NGD_GLOBAL_PROPS       := "Global";
const NGUILD_TITLE           := "Gd_";
const NGD_QTYPE              := "QuestType";

const NGDTYPE_MAXID          := 10;
const NGDSTATUS_MAXID        := 3;
const NGD_MAXTYPES           := 15;
const NGD_MAXSKIDS           := 30;
const NGD_MAXCITIES          := 8;

const NGD_PLAYER_DELAY       := 14400; // 4h
const NGD_TYPE_MURD_COUNT    := 30;
const NGD_ORDER_MURD_MAX     := 10;
const NGD_SK_REQ_VAL         := 30;
const NGD_MAX_GUILD_MSHIP    := 2;
const NGD_EXP_MODIFIER       := 100;
const NGD_CITIES_CHECK_TIME  := 600;
const NGD_CHSK_DELAY         := 1209600; // 2 tyg
const NGD_LOSE_REP_TIME      := 1209600; // 2 tyg
const NGD_LOSE_REP_LLTIME    := 172800;
const NGD_LOSE_REP_DELAY     := 7200; // 2h
const NGD_LOSE_REP_VALUE     := NGD_EXP_MODIFIER;
const NGD_MQUEST_REPGAIN     := NGD_EXP_MODIFIER;
const NGD_RESIGN_EXP_LOSS    := 0.05; // procent utraty reputacji przy wyrzuceniu z gildii w setnych (aktualnie 5%)

const NGD_CITY_RESPECT_LVL   := 3;
const NGD_CITY_LEXP_VALUE    := NGD_EXP_MODIFIER;

//-----------------------------------------------------------------------------
// Config
//-----------------------------------------------------------------------------

const NGD_CFG                := ":npcguilds:guilds";

const NGDCFG_TYPE_TITLE      := "Type_";
const NGDCFG_LVL_TITLE       := "Level_";

const NGDCFG_TNAMES          := "Types";
const NGDCFG_SNAMES          := "Status";
const NGDCFG_TITLES          := "PlayerTitles";
const NGDCFG_AGEREQ          := "AgeReq";

//-----------------------------------------------------------------------------
// Npc Guild Flags
//-----------------------------------------------------------------------------

const NGF_QGD_ONLY           := 0x00000001;
const NGF_EXPIRED            := 0x00000002;

//-----------------------------------------------------------------------------
// Npc Guild Player Flags
//-----------------------------------------------------------------------------

const NGPF_MEMBER            := 0x00000001;
const NGPF_TEST_QUEST        := 0x00000002;
const NGPF_MAXSK_CHOOSEN     := 0x00000004;
const NGPF_NOT_ACTIVE        := 0x00000008;

//-----------------------------------------------------------------------------
// Guild Data Properties
//-----------------------------------------------------------------------------

const GDPROP_NAME            := "Name";
const GDPROP_TYPE            := "Type";
const GDPROP_STATUS          := "Status";
const GDPROP_QTYPES          := "QuestTypes";
const GDPROP_BUILDINGS       := "Buldings";
const GDPROP_MEMBERS         := "Members";
const GDPROP_SKIDS           := "SkillIDs";
const GDPROP_MAINSKIDS       := "MainSkIDs";
const GDPROP_MASTER          := "Master";
const GDPROP_CITIES          := "Cities";
const GDPROP_BOARD           := "Board";

const GDAPROP_CRINFO         := "CrInfo";
const GDAPROP_UPINFO         := "UpInfo";
const GDAPROP_FLAGS          := "Flags";

const GDGPROP_GDCRINFO       := "GuildCrInfo";
const GDGPROP_GDUPINFO       := "GuildUpInfo";
const GDGPROP_NEXTID         := "GuildNextID";

const PROP_NGDDATA           := "NpcGuildData";
const PROP_NGDCITIES         := "NpcGuildCities";

const GDBPROP_GDID           := "GuildId";

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

const NGDTYPE_WARRIORS       := 1;
const NGDTYPE_ARCHERS        := 2;
const NGDTYPE_MAGES          := 3;
const NGDTYPE_MURDERERS      := 4;
const NGDTYPE_THIEFS         := 5;
const NGDTYPE_WEARWOLFS      := 6;
const NGDTYPE_VAMPIRES       := 7;
const NGDTYPE_NECROS         := 8;
const NGDTYPE_DRUIDS         := 9;
const NGDTYPE_CRAFTSMAN      := 10;

//-----------------------------------------------------------------------------
// Status
//-----------------------------------------------------------------------------

const NGDSTATUS_ORDER        := 1;
const NGDSTATUS_CHAOS        := 2;
const NGDSTATUS_NEUTRAL      := 3;

//-----------------------------------------------------------------------------
