//-----------------------------------------------------------------------------
// Core Objtypes
//-----------------------------------------------------------------------------

const UOBJ_TILLERMAN                   := 0xF010;
const UOBJ_PORT_PLANK                  := 0xF011;
const UOBJ_STARBOARD_PLANK             := 0xF012;
const UOBJ_HOLD                        := 0xF013;
const UOBJ_WRESTLE                     := 0xF020;
const UOBJ_MOUNT                       := 0xF021;
const UOBJ_TRADECONT                   := 0xFF01;

//-----------------------------------------------------------------------------
// Artifacts
//-----------------------------------------------------------------------------

const UOBJ_ART_START                   := 0xAA00;
const UOBJ_ART_END                     := 0xAA1E;

//-----------------------------------------------------------------------------
// Guild
//-----------------------------------------------------------------------------

const UOBJ_GUILD_STONE                 := 0xA390;
const UOBJ_GUILD_STONE_DEED            := 0xA391;

//-----------------------------------------------------------------------------
// Signs
//-----------------------------------------------------------------------------

const UOBJ_SHOP_SIGN_START             := 2980;
const UOBJ_SHOP_SIGN_END               := 3026;
const UOBJ_GUILD_SIGN_START            := 3028;
const UOBJ_GUILD_SIGN_END              := 3084;
const UOBJ_DIR_SIGN_START              := 4759;
const UOBJ_DIR_SIGN_END                := 4766;
const UOBJ_SIGN_HOLDER_START           := 2968;
const UOBJ_SIGN_HOLDER_END             := 2978;
const UOBJ_BANNER_START                := 5551;
const UOBJ_BANNER_END                  := 5621;
const UOBJ_ROADPOST                    := 4758;

//-----------------------------------------------------------------------------
// Containers
//-----------------------------------------------------------------------------

const UOBJ_TRASHBARREL                 := 0x7007;

const UOBJ_BOX                         := 0x09aa;
const UOBJ_BANKBOX                     := 0x966f;
const UOBJ_MAILBOX                     := 0x9660;
const UOBJ_STORE_MAILBOX               := 0x9661;
const UOBJ_STOREBOX                    := 0x966e;
const UOBJ_BANKBOX_REDIR               := 0x9669;
const UOBJ_STORE_TICKET                := 0xA29B;

const UOBJ_ESCROWBOX                   := 0x9668;
const UOBJ_ESCROWPACK                  := 0x9667;

const UOBJ_POTIONKEG                   := 0x1940;

const UOBJ_BACKPACK                    := 0x0e75;
const UOBJ_NPCPACK                     := 0x966b;
const UOBJ_STAFFPACK                   := 0x966c;
const UOBJ_BAG                         := 0x0E76;

const UOBJ_TREASURE_CHEST              := 0x6002;
const UOBJ_PS_CONT                     := 0xAABA;

//-----------------------------------------------------------------------------
// Gates
//-----------------------------------------------------------------------------

const UOBJ_BLUEGATE                    := 0x0F6C;
const UOBJ_REDGATE                     := 0x0dda;
const UOBJ_BLACKGATE                   := 0x1fd4;
const UOBJ_WHITEGATE                   := 0x1fe7;

const UOBJ_BLGATE_ANIM                 := 0x1af3;
const UOBJ_BLGATE_ASET                 := 0x1af4;
const UOBJ_BLGATE_AEND                 := 0x1af9;

const UOBJ_RDGATE_ASET                 := 0x1ae5;
const UOBJ_RDGATE_AEND                 := 0x1aed;

const UOBJ_BCGATE_ASET                 := 0x1fcb;
const UOBJ_BCGATE_AEND                 := 0x1fd3;

const UOBJ_WHGATE_ASET                 := 0x1fde;
const UOBJ_WHGATE_AEND                 := 0x1fe6;

const UOBJ_TELE                        := 0x6200;
const UOBJ_WAYGATE                     := 0x6202;
const UOBJ_WAYSTONE                    := 0xB030;
const UOBJ_TELE_HOLE                   := 0xa3c0;

//-----------------------------------------------------------------------------
//  FlourMill
//-----------------------------------------------------------------------------

const UOBJ_FLOURMILL_UP_E              := 0x1920;
const UOBJ_FLOURMILL_UP_N              := 0x192C;

const UOBJ_MILLMID_E                   := 0x1922;
const UOBJ_MILLMID_N                   := 0x192E;

const UOBJ_MILLCRANK_E                 := 0x1924;
const UOBJ_MILLCRANK_N                 := 0x1930;

const UOBJ_WHEAT                       := 0x1EBE;
const UOBJ_WHEAT2                      := 0x1EBF;
const UOBJ_FLOUR                       := 0x1039;
const UOBJ_CORN                        := 0x0C7F;
const UOBJ_CORNFLOUR                   := 0xC75B;

//-----------------------------------------------------------------------------
// SpawnNet
//-----------------------------------------------------------------------------

const UOBJ_SCHEST                      := 0xa400;
const UOBJ_SRUNE                       := 0xa300;

//-----------------------------------------------------------------------------
// Reagents
//-----------------------------------------------------------------------------

const UOBJ_GEM_START                   := 0x0f0f;
const UOBJ_GEM_END                     := 0x0f30;

const UOBJ_EMERALD                     := 0x0F10;

const UOBJ_BLACK_PEARL                 := 0x0f7a;
const UOBJ_BLOOD_MOSS                  := 0x0f7b;
const UOBJ_BRIMSTONE                   := 0x0f7f;
const UOBJ_FERTILE_DIRT                := 0x0f81;
const UOBJ_EXECUTIONERS_CAP            := 0x0f83;
const UOBJ_GARLIC                      := 0x0f84;
const UOBJ_GINSENG                     := 0x0f85;
const UOBJ_MANDRAKE_ROOT               := 0x0f86;
const UOBJ_NIGHTSHADE                  := 0x0f88;
const UOBJ_OBSIDIAN                    := 0x0f89;
const UOBJ_PIG_IRON                    := 0x0f8a;
const UOBJ_SULFUROUS_ASH               := 0x0f8c;
const UOBJ_SPIDER_SILK                 := 0x0f8d;
const UOBJ_VOLCANIC_ASH                := 0x0f8f;

const UOBJ_START_RAREREGS              := 0x0f78;
const UOBJ_END_RAREREGS                := 0x0f91;

const UOBJ_DEADWOOD                    := 0x0f90;

//-----------------------------------------------------------------------------
// Fields
//-----------------------------------------------------------------------------

const UOBJ_ENERGYFIELD_NS              := 0x7082;
const UOBJ_ENERGYFIELD_EW              := 0x7081;
const UOBJ_STONEFIELD_NS               := 0x7083;
const UOBJ_STONEFIELD_EW               := 0x7084;
const UOBJ_POISONFIELD_NS              := 0x7086;
const UOBJ_POISONFIELD_EW              := 0x7085;
const UOBJ_PARAFIELD_NS                := 0x7088;
const UOBJ_PARAFIELD_EW                := 0x7087;
const UOBJ_FROSTFIELD_NS               := 0x708a;
const UOBJ_FROSTFIELD_EW               := 0x7089;
const UOBJ_FIREFIELD_NS                := 0x708c;
const UOBJ_FIREFIELD_EW                := 0x708b;

//-----------------------------------------------------------------------------
// Boats
//-----------------------------------------------------------------------------

const GID_PLANK_EXTENDED_FACING_WEST   := 0x3ed5;
const GID_PLANK_RETRACTED_FACING_WEST  := 0x3eb1;
const GID_PLANK_EXTENDED_FACING_EAST   := 0x3ed4;
const GID_PLANK_RETRACTED_FACING_EAST  := 0x3eb2;
const GID_PLANK_EXTENDED_FACING_SOUTH  := 0x3e84;
const GID_PLANK_RETRACTED_FACING_SOUTH := 0x3e85;
const GID_PLANK_EXTENDED_FACING_NORTH  := 0x3e89;
const GID_PLANK_RETRACTED_FACING_NORTH := 0x3e8a;

//-----------------------------------------------------------------------------
// Housing
//-----------------------------------------------------------------------------

const UOBJ_DOORS_START                 := 0x0675;
const UOBJ_DOORS_END                   := 0x06f4;
const UOBJ_IDOORS_START                := 0x1fed;
const UOBJ_IDOORS_END                  := 0x1ffc;
const UOBJ_BRASS_SIGN                  := 0x0bd2;
const UOBJ_BRASS_SIGN2                 := 0x0bd1;
const UOBJ_WOOD_SIGN                   := 0x0bd0;
const UOBJ_WOOD_SIGN2                  := 0x0bcf;
const UOBJ_WOOD_SIGNTWO                := 0x0bd1;
const UOBJ_TENT_CHEST                  := 0x0E43;
const UOBJ_SECURED_CONT                := 0xa297;
const UOBJ_ARENAPACK                   := 0xA204;

//-----------------------------------------------------------------------------
// Keys
//-----------------------------------------------------------------------------

const UOBJ_KEYSTART                    := 0x100e;
const UOBJ_KEYEND                      := 0x1013;
const UOBJ_KEYRING                     := 0x1011;
const UOBJ_COPPER_KEY                  := 0x100e;
const UOBJ_GOLD_KEY                    := 0x100f;
const UOBJ_IRON_KEY                    := 0x1010;
const UOBJ_KEY_RING                    := 0x1011;
const UOBJ_MAGIC_KEY                   := 0x1012;
const UOBJ_RUSTY_IRON_KEY              := 0x1013;

//-----------------------------------------------------------------------------
// Hairs
//-----------------------------------------------------------------------------

const UOBJ_STARTHAIR                   := 0x2030;
const UOBJ_ENDHAIR                     := 0x2060;

const UOBJ_SHORT_HAIR                  := 0x203B;
const UOBJ_LONG_HAIR                   := 0x203C;

const UOBJ_ELF_SHORT_HAIR              := 0x2FC1;
const UOBJ_ELF_LONG_HAIR               := 0x2FCD;

const UOBJ_MUSTACH                     := 0x2041;
const UOBJ_SHORT_BEARD_MUS             := 0x204B;
const UOBJ_LONG_BEARD_MUS              := 0x204C;
const UOBJ_LONG_BEARD                  := 0x203E;

//-----------------------------------------------------------------------------
// Monetary
//-----------------------------------------------------------------------------

const UOBJ_GOLDCOIN                    := 0x0EED;
const UOBJ_CHEQUE                      := 0xa298;

//-----------------------------------------------------------------------------
// Misc
//-----------------------------------------------------------------------------

const UOBJ_EMPTY_VIAL                  := 0x0E24;
const UOBJ_RAWRIBS                     := 0x09f1;
const UOBJ_SPAM                        := 0x1044;

const UOBJ_BOOK_START                  := 0x0FEF;
const UOBJ_BOOK_END                    := 0x0FF4;

const UOBJ_TROUGH                      := 0x7008;

const UOBJ_THIEF_GLOVES                := 0x0E02;
const UOBJ_WIG                         := 0x0E03;

const UOBJ_EGG                         := 0x09b5;

const UOBJ_BLANKMAP                    := 0x14eb;

const UOBJ_SPIDERWEB                   := 0x0ee4;
const UOBJ_TICKET                      := 0x14f0;

const UOBJ_KINDLING                    := 0x0de1;
const UOBJ_CAMPFIRE                    := 0x0de3;
const UOBJ_EMBERS                      := 0x0de9;
const UOBJ_ASHES                       := 0x0dea;

const UOBJ_DYETUB                      := 0x7011;
const UOBJ_DYES                        := 0x7010;

const UOBJ_BANDAGE                     := 0x0e21;
const UOBJ_BLOODYBANDAGES              := 0x0e20;
const UOBJ_BLANK_SCROLL                := 0x0e34;

const UOBJ_DUMMYSWING_NS               := 0x1071;
const UOBJ_DUMMYFREEZE_NS              := 0x1070;
const UOBJ_DUMMYSWING_EW               := 0x1075;
const UOBJ_DUMMYFREEZE_EW              := 0x1074;

const UOBJ_PDUMMYSWING_EW              := 0x1ec1;
const UOBJ_PDUMMYFREEZE_EW             := 0x1ec0;
const UOBJ_PDUMMYSWING_NS              := 0x1ec4;
const UOBJ_PDUMMYFREEZE_NS             := 0x1ec3;

const UOBJ_BLOOD_BIG1                  := 0x122a;
const UOBJ_BLOOD_BIG2                  := 0x122d;
const UOBJ_BLOOD_SMALL1                := 0x122b;
const UOBJ_BLOOD_SMALL2                := 0x122c;
const UOBJ_BLOOD_SMALL3                := 0x122e;
const UOBJ_BLOOD_SMEAR                 := 0x122f;
const UOBJ_ACID_POOL                   := 0x966D;
const UOBJ_SPIDERWEB_POOL              := 0x10D5;
const UOBJ_BOW                         := 0x13b2;
const UOBJ_HEAVY_XBOW                  := 0x13fd;
const UOBJ_XBOW                        := 0x0f4f;

const UOBJ_MAP1                        := 0x14eb;
const UOBJ_MAP2                        := 0x6008;
const UOBJ_ROLLED_MAP1                 := 0x14ed;
const UOBJ_ROLLED_MAP2                 := 0x14ee;

const UOBJ_HOUSE_DEED1                 := 0x14ef;
const UOBJ_SHIP_DEED1                  := 0x14f1;
const UOBJ_SHIP_DEED2                  := 0x14f2;

const UOBJ_FEATHER                     := 0x1bd1;
const UOBJ_SHAFTS                      := 0x1bd4;
const UOBJ_ARROW                       := 0x0f3f;
const UOBJ_BOLTS                       := 0x1bfb;

const UOBJ_ARM                         := 0x1cec;
const UOBJ_LEG                         := 0x1ce2;
const UOBJ_TRUNK                       := 0x1cee;
const UOBJ_HEART                       := 0x1cef;
const UOBJ_HEAD                        := 0x1da0;
const UOBJ_ELF_EARS                    := 0x312D;
const UOBJ_HUMAN_EARS                  := 0x312F;

const UOBJ_CORPSE                      := 0x2006;

const UOBJ_SCONCE_ST                   := 0x0a0b;
const UOBJ_SCONCE_SC                   := 0x0a01;
const UOBJ_SCONCE_ET                   := 0x0a06;
const UOBJ_SCONCE_EC                   := 0x09fc;

const UOBJ_BONES_START                 := 0x0ECA;
const UOBJ_BONES_END                   := 0x0ED2;

const UOBJ_INVISITEM                   := 0xb200;
const UOBJ_JUKA                        := 0xAAAA;

const UOBJ_WATERPITCHER                := 0x1f9D;
const UOBJ_MILKPITCHER                 := 0x09F0;

const UOBJ_MSGBOARD                    := 0x1E5E;
const UOBJ_MSGBOARD2                   := 0x1E5F;
const UOBJ_BOARDMSG                    := 0x0EB0;

const UOBJ_QUESTTICKET                 := 0xB300;
const UOBJ_QUESTITEM                   := 0xB301;
const UOBJ_QUESTCONT                   := 0xB302;
const UOBJ_NGD_BOARD                   := 0xB310;
const UOBJ_MQUESTITEM                  := 0xB320;

const UOBJ_GAMEPIECE                   := 0x70B3;

const UOBJ_TR_BAG                      := 0xA240;
const UOBJ_TR_ROBE                     := 0xA241;
const UOBJ_TR_CLOAK                    := 0xA242;

const UOBJ_ROBE                        := 0x9945;
const UOBJ_CLOAK                       := 0x1515;
const UOBJ_WATCH                       := 0x5015;

const UOBJ_HATCHET                     := 0x0F43;

const UOBJ_MOONCLAW                    := 0x27FB;

//-----------------------------------------------------------------------------

const UOBJ_HORSE_ARMOR_1               := 0x1376;
const UOBJ_HORSE_ARMOR_2               := 0x1377;

const UOBJ_SMITH_HAMMER                := 0x13E3;

//-----------------------------------------------------------------------------

const UOBJ_HRSIGN_N                    := 0xA902;
const UOBJ_HRSIGN_W                    := 0xA903;
const UOBJ_HRDSIGN_N                   := 0xA904;
const UOBJ_HRDSIGN_W                   := 0xA905;

const UOBJ_HRSIGN_START                := 0xA902;
const UOBJ_HRSIGN_END                  := 0xA905;

//-----------------------------------------------------------------------------

const UOBJ_HSIGN_START                 := 0x600A;
const UOBJ_HSIGN_END                   := 0x600D;

//-----------------------------------------------------------------------------

const UOBJ_BARB_ELIX_SET               := 0xA8D0;
const UOBJ_BARB_ELIX_END               := 0xA8DA;

//-----------------------------------------------------------------------------

const UOBJ_KENSAI_ELIX_SET             := 0xA8A0;
const UOBJ_KENSAI_ELIX_END             := 0xA8AB;

//-----------------------------------------------------------------------------

const UOBJ_LVL1_MAP                    := 0x6003;
const UOBJ_LVL2_MAP                    := 0x6004;
const UOBJ_LVL3_MAP                    := 0x6005;
const UOBJ_LVL4_MAP                    := 0x6006;
const UOBJ_LVL5_MAP                    := 0x6007;

//-----------------------------------------------------------------------------
// MarketPlace 
//-----------------------------------------------------------------------------
const UOBJ_MP_SING                     := 0xEA01;
const UOBJ_MP_BOX                      := 0xEA02;
//-----------------------------------------------------------------------------

const UOBJ_HOUSE_START                 := 0x6210;
const UOBJ_HOUSE_END                   := 0x62AF;

//-----------------------------------------------------------------------------
