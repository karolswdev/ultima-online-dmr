//-----------------------------------------------------------------------------
// Basic
//-----------------------------------------------------------------------------

const AI_SPEECH_SAY_CFG                := ":*:npc_speech";
const AI_SPEECH_FULL_SENT_SIZE         := 15;

const AI_SPEECH_DEF_RANGE              := 2;
const AI_SPEECH_QUESTION_WAIT_TIME     := 15;

//-----------------------------------------------------------------------------
// Trigger flags
//-----------------------------------------------------------------------------

const AI_SPT_CASE_SENSITIVE            := 0x0001;
const AI_SPT_FULL_SENTENCE             := 0x0002;
const AI_SPT_SPOKE_NPC_NAME            := 0x0004;
const AI_SPT_NO_CAN_SPEAK_TRIG         := 0x0008;

//-----------------------------------------------------------------------------
// Say IDs
//-----------------------------------------------------------------------------

const AI_SPF_SAY_PRIVATE               := 0x01;

//-----------------------------------------------------------------------------

const AI_SPID_BUSY_SOURCE              := "BusySource";
const AI_SPID_BUSY_OTHER               := "BusyOther";
const AI_SPID_DONT_UNDERSTAND          := "DontUnderstand";
const AI_SPID_EMPTY_SELL               := "EmptySellList";
const AI_SPID_EMPTY_BUY                := "EmptyBuyList";
const AI_SPID_ANOTHER_TIME             := "AnotherTime";

const AI_SPID_LOW_PRICE_SELL           := "LowPriceSell";
const AI_SPID_MID_PRICE_SELL           := "MidPriceSell";
const AI_SPID_HIGH_PRICE_SELL          := "HighPriceSell";

const AI_SPID_LOW_PRICE_BUY            := "LowPriceBuy";
const AI_SPID_MID_PRICE_BUY            := "MidPriceBuy";
const AI_SPID_HIGH_PRICE_BUY           := "HighPriceBuy";

const AI_SPID_GOLD_THANKS              := "GoldThanks";
const AI_SPID_NONEED_GOLD              := "NoNeedGold";

const AI_SPID_TAKE_ITEM                := "TakeItem";
const AI_SPID_NO_NEED_ITEM             := "NoNeedItem";
const AI_SPID_NO_ITEM_FOR_ME           := "NotItemForMe";

const AI_SPID_QUESTION_WRONG_DUDE      := "QuestionWrongDude";
const AI_SPID_NO_LONGER_WAIT           := "NoLongerWait";
const AI_SPID_QUESTION_WRONG_ANSWER    := "WrongAnswer";

const AI_SPID_GET_OUT                  := "GetOut";
const AI_SPID_GET_OUT_NAKED            := "GetOutNaked";

const AI_SPID_DONT_BARGE               := "DontBarge";
const AI_SPID_PUSHED                   := "Pushed";
const AI_SPID_GOODBYE                  := "Goodbye";

//-----------------------------------------------------------------------------
