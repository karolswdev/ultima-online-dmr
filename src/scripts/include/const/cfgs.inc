//=====================================================================
//---------------------------------------------------------------------
// Configs & Logs
//---------------------------------------------------------------------
//=====================================================================

const ITEMS_CFG                        := ":*:itemdesc";
const UOSKILLS_CFG                     := ":hooks:uoskills";
const UOCLIENT_CFG                     := ":hooks:uoclient";
const ATTRIBUTES_CFG                   := ":hooks:attributes";
const ATTRGROUPS_CFG                   := ":hooks:attr_groups";
const SPELLS_CFG                       := ":*:spells";
const CIRCLES_CFG                      := "::circles";
const LANDTILES_CFG                    := "::landtiles";
const TILES_CFG                        := "::tiles";
const NPCSKILLS_CFG                    := "::npcskills";

const SPEECH_LOG                       := ":splog:sp";
const ERROR_LOG                        := ":errlog:errors.log";
const WM_ERROR_LOG                     := ":errlog:wm_errors.log";
const SM_ERROR_LOG                     := ":errlog:sm_errors.log";
const NOTICE_LOG                       := ":errlog:notices.log";
const WM_NOTICE_LOG                    := ":errlog:wm_notices.log";
const SM_NOTICE_LOG                    := ":errlog:sm_notices.log";

const LOGS_PKG                         := ":logs:";
const DATA_PKG                         := ":datacfg:";
const SHARECFG_PKG                     := ":sharecfg:";
const MCFG_PKG                         := ":mcfg:";
const LASTLOG_PKG                      := ":lastlog:";

const SERVER_STATUS_LOG                := LOGS_PKG+"status";
const ONLINE_LOG                       := LOGS_PKG+"online";
const ACCS_LOG                         := LOGS_PKG+"accs";
const FSPEECH_LOG                      := LOGS_PKG+"fspeech.log";
const SHARDINFO_LOG                    := LOGS_PKG+"shard_info";

const GOLOC_CFG                        := DATA_PKG+"golocs";
const MDEEDS_CFG                       := DATA_PKG+"mdeeds";
const CENWORDS_CFG                     := DATA_PKG+"cenwords";
const CWORDS_CFG                       := DATA_PKG+"cwords";
const BWORDS_CFG                       := DATA_PKG+"bwords";
const EMOTES_CFG                       := DATA_PKG+"emotes";
const EMOTESND_CFG                     := DATA_PKG+"emotesnd";

const MOONGATE_CFG                     := ":teleporters:moongate";
const SAVER_CFG                        := ":saver:saver";
const RACES_CFG                        := ":character:races";
const CLASSES_CFG                      := ":character:classes";

const CHAMP_CFG                        := ":champions:champions";

const ATTR_CFG                         := MCFG_PKG+"attr";
const ISETS_CFG                        := MCFG_PKG+"itemsets";
const CITIES_CFG                       := MCFG_PKG+"cities";
const DUNGS_CFG                        := MCFG_PKG+"dungeons";
const BADNAMES_CFG                     := MCFG_PKG+"badnames";
const CLIENTS_CFG                      := MCFG_PKG+"clients";
const NPCSPEECH_CFG                    := MCFG_PKG+"npcspeech";
const NOTO_CFG                         := MCFG_PKG+"noto";
const NPCGRAP_CFG                      := MCFG_PKG+"npcsgrap";
const UNICS_CFG                        := MCFG_PKG+"unics";
const EQUIP_CFG                        := MCFG_PKG+"equips";
const FOOD_CFG                         := MCFG_PKG+"food";
const INN_CFG                          := MCFG_PKG+"inn";
const USESTUFF_CFG                     := MCFG_PKG+"use_stuff";
const MRCSPAWN_CFG                     := MCFG_PKG+"mrcspawn";
const NAMES_CFG                        := MCFG_PKG+"names";
const STEQP_CFG                        := MCFG_PKG+"starteqp";
const TRAVEL_CFG                       := MCFG_PKG+"traveling";
const CLOTHS_CFG                       := MCFG_PKG+"cloths";
const COLORS_CFG                       := MCFG_PKG+"colors";
const LOOTS_CFG                        := MCFG_PKG+"nlootgroup";
const ITEMGROUPS_CFG                   := MCFG_PKG+"itemgroups";
const NTZONE_CFG                       := MCFG_PKG+"ntzone";
const PEACEFUL_CFG                     := MCFG_PKG+"peaceful";
const STORE_CFG                        := MCFG_PKG+"store";
const LOGOPT_CFG                       := MCFG_PKG+"log";
const LOGONLOCS_CFG                    := MCFG_PKG+"logonlocs";

const PLAYERHOOKS_CFG                  := MCFG_PKG+"phooks";
const VAMP_AREA_CFG                    := MCFG_PKG+"vampire_area";

//---------------------------------------------------------------------
