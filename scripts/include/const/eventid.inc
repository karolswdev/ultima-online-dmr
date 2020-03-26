//================================================================================
//--------------------------------------------------------------------------------
// Event IDs
//--------------------------------------------------------------------------------
//================================================================================

const EVID_SPEECH                    := 0x00000001;
const EVID_ENGAGED                   := 0x00000002;
const EVID_DISENGAGED                := 0x00000004;
const EVID_DAMAGED                   := 0x00000008;
const EVID_ENTEREDAREA               := 0x00000010;
const EVID_LEFTAREA                  := 0x00000020;
const EVID_OPPONENT_MOVED            := 0x00000040;
const EVID_HOSTILE_MOVED             := 0x00000080; // not implemented

const EVID_MERCHANT_BOUGHT           := 0x00000100;
const EVID_MERCHANT_SOLD             := 0x00000200;
const EVID_ITEM_GIVEN                := 0x00000400;
const EVID_DOUBLECLICKED             := 0x00000800;
const EVID_GHOST_SPEECH              := 0x00001000;
const EVID_GONE_CRIMINAL             := 0x00002000;

//--------------------------------------------------------------------------------
// Custom events
//--------------------------------------------------------------------------------

const EVID_RUN                       := 0x00100001;
const EVID_WALK                      := 0x00100002;
const EVID_PUSH                      := 0x00100003;
const EVID_FLEE                      := 0x00100004;
const EVID_MOUNT                     := 0x00100005;
const EVID_WAKEUP                    := 0x00100006;
const EVID_MOUNT_LOGON               := 0x00100007;
const EVID_MOUNT_LOGOFF              := 0x00100008;
const EVID_WALK_FOLLOW               := 0x00100009;
const EVID_RUN_FOLLOW                := 0x00100010;
const EVID_FOLLOW_STOP               := 0x00100011;
const EVID_PEACEMADE                 := 0x00100012;
const EVID_RUN_AWAY                  := 0x00100013;
const EVID_KEEP_DISTANCE             := 0x00100014;

const EVID_CMERC_BOUGHT              := 0x00100101;
const EVID_CMERC_SOLD                := 0x00100102;
const EVID_MASTER_DEAD               := 0x00100103;
const EVID_CORPSE_LOOT               := 0x00100104;
const EVID_FRIEND_HEAL               := 0x00100105;
const EVID_QUEST_MSG                 := 0x00100106;
const EVID_UNEQUIP                   := 0x00100107;
const EVID_QUESTION                  := 0x00100108;
const EVID_EQUIP                     := 0x00100109;

const EVID_ALL_ATTACK_CMD            := 0x00100110;
const EVID_ALL_FOLLOW_CMD            := 0x00100111;
const EVID_ALL_GUARD_CMD	           := 0x00100112;
const EVID_ALL_TRANSFER_CMD	         := 0x00100113;

const EVID_GOLDCHECK                 := 0x00100200;
const EVID_REPORTMAKRO               := 0x00100201;

const EVID_PLAYER_LOGIN              := 0x00100203;
const EVID_PLAYER_LOGOFF             := 0x00100204;
const EVID_PLAYER_DELAY              := 0x00100205;
const EVID_PLAYER_RECONNECT          := 0x00100206;
const EVID_SEND_SEER                 := 0x00100207;
const EVID_SEND_GM                   := 0x00100208;
const EVID_SEND_ADMIN                := 0x00100209;

const EVID_MOD                       := 0x00100301;
const EVID_SKMOD                     := 0x00100302;
const EVID_MAGICMOD                  := 0x00100303;
const EVID_MAGICSKMOD                := 0x00100304;
const EVID_UNDOMOD                   := 0x00100305;
const EVID_UNDOSKMOD                 := 0x00100306;
const EVID_MURDPENALTY               := 0x00100307;

const EVID_SK_AWARD                  := 0x00100310;
const EVID_ST_AWARD                  := 0x00100311;
const EVID_SK_SET                    := 0x00100312;
const EVID_ST_SET                    := 0x00100313;
const EVID_AT_REC                    := 0x00100314;
const EVID_SETSPECABS                := 0x00100315;
const EVID_REBORN                    := 0x00100316;

//--------------------------------------------------------------------------------
