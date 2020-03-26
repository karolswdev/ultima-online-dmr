//-----------------------------------------------------------------------------------//

use polsys;

//-----------------------------------------------------------------------------------//

include "include/checks/stdchecks";
include "include/finding";
include "include/dices";

//-----------------------------------------------------------------------------------//

const CLILOC_ITEMUPD         := 0x000fea1b;

const SPEECH_TYPE            := 0x0;
const EMOTE_TYPE             := 0x2;
const WHISP_TYPE             := 0x8;
const YELL_TYPE              := 0x9;
const EV_SPEECH_RANGE        := 12;
const MAX_WHISP_RANGE        := 6;
const MAX_SPEECH_RANGE       := 12;
const MAX_YELL_RANGE         := 18;

//-----------------------------------------------------------------------------------//

const PC_CONT_ITM_LEN        := 19;
const PC_CONT_ITM_LEN_KR     := 20;

//-----------------------------------------------------------------------------------//

function CrUpdStatsPacket(who)

  var Flag      := 0x04;
  var Expansion := who.acct.uo_expansion;
  var Offset    := 0;

  if(Expansion == "ML" or Expansion == "SA")
    Flag := 0x05;
  elseif(Expansion == "KR")
    Flag := 0x06;
  endif

  var ItemCfg := ReadConfigFile(ITEMS_CFG);
  var WpnDmg  := ItemCfg[who.weapon.objtype].Damage;

  var Packet := CreatePacket(0x11,MSGLEN_VARIABLE);
  
  Packet.SetInt32(3 ,who.serial);
  Packet.SetString(7,who.name,0);
  Packet.SetInt16(37,GetHp(who));
  Packet.SetInt16(39,GetMaxHp(who));
  Packet.SetInt8 (41,0x00);
  Packet.SetInt8 (42,Flag);
  Packet.SetInt8 (43,who.gender);
  Packet.SetInt16(44,GetStrength(who));
  Packet.SetInt16(46,GetDexterity(who));
  Packet.SetInt16(48,GetIntelligence(who));
  Packet.SetInt16(50,GetStamina(who));
  Packet.SetInt16(52,GetMaxStamina(who));
  Packet.SetInt16(54,GetMana(who));
  Packet.SetInt16(56,GetMaxMana(who));
  Packet.SetInt32(58,0x00000000);       //Gold handled in packethook
  Packet.SetInt16(62,who.ar);
  Packet.SetInt16(64,who.weight);

  if(Flag >= 0x05)
    Packet.SetInt16(66,GetMaxWeight(who));
    Packet.SetInt8 (68,0x00);
    Offset := 3;
  endif

  if(Flag >= 0x03)
    Packet.SetInt16(66+Offset,who.statcap);
    Packet.SetInt8 (68+Offset,0x00);
    Packet.SetInt8 (69+Offset,0x00);
  endif

  if(Flag >= 0x04)
    Packet.SetInt16(70+Offset,0x0000);
    Packet.SetInt16(72+Offset,0x0000);
    Packet.SetInt16(74+Offset,0x0000);
    Packet.SetInt16(76+Offset,0x0000);
    Packet.SetInt16(78+Offset,0x0000);
    Packet.SetInt16(80+Offset,GetDiceMinValue(WpnDmg));
    Packet.SetInt16(82+Offset,GetDiceMaxValue(WpnDmg));
    Packet.SetInt32(84+Offset,0x00000000);
  endif

  return Packet;

endfunction

//-----------------------------------------------------------------------------------//

function PlayPcStEffect(x, y, z, Realm, EffId, Hue, Speed, Dur, RenderMode := 0x0)

  if(!x or !y or z == error or !Realm or !EffId or Dur <= 0 or Speed <= 0)
    return;
  endif

  if(z < 0)
    z := 255 + z;
  endif

  var Packet := CreatePacket(0xC0, 36);
  
  Packet.SetInt8 (1,0x2);
  Packet.SetInt32(2,0x0);    // Source serial
  Packet.SetInt32(6,0x0);    // Target serial
  Packet.SetInt16(10,EffId); // Item ObjType
  Packet.SetInt16(12,x);     // Source x,y,z
  Packet.SetInt16(14,y);
  Packet.SetInt8 (16,z);
  Packet.SetInt16(17,0x0);   // Target x,y,z
  Packet.SetInt16(19,0x0);
  Packet.SetInt8 (21,0x0);
  Packet.SetInt8 (22,Speed); // Speed
  Packet.SetInt8 (23,Dur);   // Duration
  Packet.SetInt16(24,0x0);   // Unknown
  Packet.SetInt8 (26,0x0);   // Fixed Dir
  Packet.SetInt8 (27,0x0);   // Explodes
  Packet.SetInt32(28,Hue);   // Hue
  Packet.SetInt32(32,RenderMode); // Render Mode

  Packet.SendAreaPacket(x, y, 16, Realm);

endfunction

//-----------------------------------------------------------------------------------//

function PlayPcEffect(Mob, EffId, Hue, Speed, Dur, RenderMode := 0x0)

  if(!Mob or !Mob.serial or !EffId or Dur <= 0 or Speed <= 0)
    return;
  endif

  var Packet := CreatePacket(0xC0, 36);
  
  Packet.SetInt8 (1,0x3);
  Packet.SetInt32(2,Mob.serial); // Source serial
  Packet.SetInt32(6,0x0);    // Target serial
  Packet.SetInt16(10,EffId); // Item ObjType
  Packet.SetInt16(12,0x0); // Source x,y,z
  Packet.SetInt16(14,0x0);
  Packet.SetInt8 (16,0x0);
  Packet.SetInt16(17,0x0);   // Target x,y,z
  Packet.SetInt16(19,0x0);
  Packet.SetInt8 (21,0x0);
  Packet.SetInt8 (22,Speed); // Speed
  Packet.SetInt8 (23,Dur);   // Duration
  Packet.SetInt16(24,0x0);   // Unknown
  Packet.SetInt8 (26,0x0);   // Fixed Dir
  Packet.SetInt8 (27,0x0);   // Explodes
  Packet.SetInt32(28,Hue);   // Hue
  Packet.SetInt32(32,RenderMode);   // Render Mode

  Packet.SendAreaPacket(Mob.x, Mob.y, 16, Mob.realm);

endfunction

//-----------------------------------------------------------------------------------//

function PlayPcMovEffect(Source, Targ, EffId, Hue, Speed, Loop := 0, Explode := 0, FixDir := 0, RenderMode := 0x0)

  if(!Source or !Targ or !EffId or Loop <= 0 or Speed <= 0)
    return;
  endif

  var sz := Source.z;
  var tz := Targ.z;

  if(sz == error or tz == error)
    return;
  endif

  if(sz < 0)
    sz := 255 + sz;
  endif

  if(tz < 0)
    tz := 255 + tz;
  endif

  var Packet := CreatePacket(0xC0, 36);
  
  Packet.SetInt8 (1,0x0);

  if(Source.serial and Targ.serial)
    Packet.SetInt32(2,Source.serial); // Source serial
    Packet.SetInt32(6,Targ.serial);    // Target serial
  else
    Packet.SetInt32(2,0x00); // Source serial
    Packet.SetInt32(6,0x00);    // Target serial
  endif

  Packet.SetInt16(10,EffId); // Item ObjType
  Packet.SetInt16(12,Source.x); // Source x,y,z
  Packet.SetInt16(14,Source.y);
  Packet.SetInt8 (16,sz);
  Packet.SetInt16(17,Targ.x);   // Target x,y,z
  Packet.SetInt16(19,Targ.y);
  Packet.SetInt8 (21,tz);
  Packet.SetInt8 (22,Speed); // Speed
  Packet.SetInt8 (23,Loop);  // Duration
  Packet.SetInt16(24,0x0);   // Unknown
  Packet.SetInt8 (26,FixDir);   // Fixed Dir
  Packet.SetInt8 (27,Explode);   // Explodes
  Packet.SetInt32(28,Hue);   // Hue
  Packet.SetInt32(32,RenderMode);   // Render Mode

  Packet.SendAreaPacket(Source.x, Source.y, 16, Source.realm);

endfunction

//-----------------------------------------------------------------------------------//

function PlayPcAnim(who, Id, FrameCount := -1, Rep := 0, Backward := 0, Delay := 0)

  if(!who.isa(POLCLASS_MOBILE) or !Id)
    return;
  endif

  var Packet := CreatePacket(0x6E, 0xE);

  if(FrameCount == -1)
    FrameCount := who.facing;
  endif

  Packet.SetInt32(1,  who.serial);
  Packet.SetInt16(5,  Id);
  Packet.SetIng8 (7,  0);
  Packet.SetInt8 (8,  FrameCount);
  Packet.SetInt16(9,  __IfElse(Rep, 0, 1));
  Packet.SetInt8 (11, Backward);
  Packet.SetInt8 (12, Rep);
  Packet.SetInt8 (13, Delay);

  Packet.SendAreaPacket(who.x, who.y, 16, who.realm);

endfunction

//-----------------------------------------------------------------------------------//

function CrClilocPacket(ObjSer, Type, byref UniText)

  var Packet := CreatePacket(0xD6,MSGLEN_VARIABLE);
  Packet.SetInt16(3,0x0001);
  Packet.SetInt32(5,ObjSer);
  Packet.SetInt16(9,0x0000);
  Packet.SetInt32(11,ObjSer);
  Packet.SetInt32(15,Type);
  Packet.SetUnicodeString(20,UniText,1);
  Packet.SetInt32(Packet.GetSize(),0x00000000);
  Packet.SetInt16(19,UniText.size() * 2);
  
  return Packet;
  
endfunction

//-----------------------------------------------------------------------------------//

function SendClilocUpdateLoc(who,Item,Range)

  var Packet := CrClilocPacket(Item.serial,CLILOC_ITEMUPD,CAscZ(Item.desc));

  Packet.SendAreaPacket(who.x, who.y, Range, who.realm);

endfunction

//-----------------------------------------------------------------------------------//

function SendClilocUpdate(who,Item)

  var Packet := CrClilocPacket(Item.serial,CLILOC_ITEMUPD,CAscZ(Item.desc));
  Packet.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------------//

function CrUCSpReqPacket(UniText,Type,Color,Font)

  var Packet := CreatePacket(0xAD,MSGLEN_VARIABLE);
  Packet.SetInt8(3,Type);
  Packet.SetInt16(4,Color);
  Packet.SetInt16(6,Font);
  Packet.SetString(8,"eng",1);
  Packet.SetUnicodeString(12,UniText,1);
  
  return Packet;

endfunction

//-----------------------------------------------------------------------------------//

function CrUCSpPacket(who,UniText,Type,Color,Font)

  var Packet := CreatePacket(0xAE,MSGLEN_VARIABLE);
  Packet.SetInt32(3,who.serial);
  Packet.SetInt16(7,who.objtype);
  Packet.SetInt8(9,Type);
  Packet.SetInt16(10,Color);
  Packet.SetInt16(12,Font);
  Packet.SetString(14,"eng",1);
  Packet.SetString(18,who.name,1);
  Packet.SetUnicodeString(48,UniText,1);
  
  return Packet;

endfunction

//-----------------------------------------------------------------------------------//

function SendContainerContents(who, Cont)

  var Items := ListRootItemsInContainer(Cont);

  if(!Items.size())
    return 0;
  endif

  var Cnt    := 0;
  var Pos    := 5;
  var Packet := CreatePacket(0x3C, MSGLEN_VARIABLE);

  foreach Item in Items
    if(IsKRPacketType(who))
      Packet.SetInt32(Pos   , Item.serial);
      Packet.SetInt16(Pos+4 , Item.graphic);
      Packet.SetInt8 (Pos+6 , 0);
      Packet.SetInt16(Pos+7 , Item.amount);
      Packet.SetInt16(Pos+9 , Item.x);
      Packet.SetInt16(Pos+11, Item.y);
      Packet.SetInt8 (Pos+13, 0);
      Packet.SetInt32(Pos+14, Cont.serial);
      Packet.SetInt16(Pos+18, Item.color);

      Pos := Pos + PC_CONT_ITM_LEN_KR;
    else
      Packet.SetInt32(Pos   , Item.serial);
      Packet.SetInt16(Pos+4 , Item.graphic);
      Packet.SetInt8 (Pos+6 , 0);
      Packet.SetInt16(Pos+7 , Item.amount);
      Packet.SetInt16(Pos+9 , Item.x);
      Packet.SetInt16(Pos+11, Item.y);
      Packet.SetInt32(Pos+13, Cont.serial);
      Packet.SetInt16(Pos+17, Item.color);

      Pos := Pos + PC_CONT_ITM_LEN;
    endif

    Cnt := Cnt + 1;
  endforeach

  Packet.SetInt16(3,Cnt);

  return Packet.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------------//

const SPTT_OBJECT            := 0;
const SPTT_GROUND            := 1;
const SPTGT_DEF_WAITTIME     := 600;

//-----------------------------------------------------------------------------------//

function SendPcTarget(who, Type, Noto)

  if(!IsLoggedIn(who))
    return 0;
  endif

  if(IsSetTempFlag(who, PCF_TARGETING))
    return 0;
  endif

  var Packet := CreatePacket(0x6C, 0x0013);

  Packet.SetInt8(1, Type);
  Packet.SetInt32(2, who.serial);

  if(Noto & TGTOPT_HARMFUL)
    Packet.SetInt8(6, 0x01);
  elseif(Noto & TGTOPT_HELPFUL)
    Packet.SetInt8(6, 0x02);
  endif

  if(Packet.SendPacket(who))
    SetObjProperty(who, "#TargPid", GetPid());
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------------//

function ClosePcTarget(who)

  var Packet := CreatePacket(0x6C, 0x0013);

  Packet.SetInt8(1, 0x00);
  Packet.SetInt32(2, who.serial);
  Packet.SetInt8(6, 0x03);

  Packet.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------------//

function RecvPcTarget(who, Time, byref Param)

  if(!IsLoggedIn(who))
    return 0;
  endif

  var eTime := ReadGameClock() + Time;
  var lev;
  var ret := 0;

  SetObjProperty(who, "#TargPid", GetPid());

  while(ReadGameClock() < eTime)
    lev := wait_for_event(1);

    if(lev)
      if(lev.Packet)

        ret := struct;
        ret.+serial  := lev.Packet.GetInt32(7);
        ret.+Loc     := struct;
        ret.Loc.+x   := lev.Packet.GetInt16(11);
        ret.Loc.+y   := lev.Packet.GetInt16(13);
        ret.Loc.+z   := lev.Packet.GetInt8(16);
        ret.+objtype := lev.Packet.GetInt16(17);
        EraseObjProperty(who, "#TargPid");

        if(ret.Loc.z > 127)
          ret.Loc.z := ret.Loc.z - 256;
        endif

        if(ret.Loc.x == 0xFFFF and ret.Loc.y == 0xFFFF)
          EraseObjProperty(who, "#TargFlags");
          return 0;
        else
          var Targ := SystemFindObjectBySerial(ret.serial);

          case(CInt(GetObjProperty(who, "#TargFlags")))
            0x01:       if(CanGetCriminal(who, Targ, TGTOPT_HARMFUL))
                          SetCriminal(who, 1, "RecvPcTarget", "Wskazal ofensywnym czarem postac "+GetMobCriminalDesc(Targ));
                        endif

            0x02:       if(CanGetCriminal(who, Targ, TGTOPT_HELPFUL))
                          SetCriminal(who, 1, "RecvPcTarget", "Wskazal defensywnym czarem postac "+GetMobCriminalDesc(Targ));
                        endif
          endcase

          EraseObjProperty(who, "#TargFlags");
          return ret;
        endif
      endif
    endif

    if(!PcTargetWaitFunc(Param))
      ret := error;
      break;
    endif
  endwhile

  if(GetObjProperty(who, "#TargPid") == GetPid())
    ClosePcTarget(who);
    lev := wait_for_event(5);
    EraseObjProperty(who, "#TargPid");
    EraseObjProperty(who, "#TargFlags");
  endif

  return ret;

endfunction

//-----------------------------------------------------------------------------------//

function PcTarget(who, Noto, byref Param)

  if(SendPcTarget(who, SPTT_OBJECT, Noto))
    var Res := RecvPcTarget(who, SPTGT_DEF_WAITTIME, Param);

    if(Res)
      var TObject := SystemFindObjectBySerial(Res.serial);
      if(TObject)
        if((Noto & TGTOPT_CHECK_LOS) and !CheckLineOfSight(who, TObject))
          var Err := error;
          Err.+errortext := "Nie widzisz tego.";
          return Err;
        endif
        return TObject;
      else
        return 0;
      endif
    elseif(Res == error)
      return Res;
    endif

  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------------//

function PcTargetLoc(who, byref Param)

  if(SendPcTarget(who, SPTT_GROUND, 0))
    var pRes := RecvPcTarget(who, SPTGT_DEF_WAITTIME, Param);

    if(pRes)
      if(pRes.serial >= 0x40000000)
        pRes.+item := FindItemBySerial(pRes.serial);
      else
        pRes.+mobile := FindPlayerBySerial(pRes.serial);
      endif

      pRes.+x := pRes.Loc.x;
      pRes.+y := pRes.Loc.y;
      pRes.+z := pRes.Loc.z;
      pRes.+realm := who.realm;
      pRes.Loc.+realm := who.realm;

      return pRes;
    elseif(pRes == error)
      return pRes;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------------//

function SendMountSpeedPacket(who, bState)

  var Packet := CreatePacket(0xBF, MSGLEN_VARIABLE);

  Packet.SetInt16(3, 0x26);
  Packet.SetInt8(5, bState);

  Packet.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------------//
/*
function SendWornItemPacket(who, item)

  var Packet := CreatePacket(0x2E, 15);

  Packet.SetInt32(1, item.serial);
  Packet.SetInt16(5, item.graphic);
  Packet.SetInt8(7, 0);
  Packet.SetInt8(8, item.layer);
  Packet.SetInt32(9, who.serial);
  Packet.SetInt16(13, item.color);

  Packet.SendAreaPacket(who.x, who.y, 18, who.realm);

endfunction
*/
//-----------------------------------------------------------------------------------//

function SendDeleteObjectPacket(who, Serial)

  var Packet := CreatePacket(0x1D, 5);
  Packet.SetInt32(1, Serial);

  Packet.SendPacket(who);

endfunction

//-----------------------------------------------------------------------------------//

function CheckCurseDelay(who)

  var CurseDelay := GetObjProperty(who, "#CurseDelay");
  if(CurseDelay and CurseDelay > ReadGameClock())
    return 0;
  endif
  
  return 1;

endfunction 

//-----------------------------------------------------------------------------------//

function DamagePacket(who,amount)

  var Packet := CreatePacket(0xBF,0xB);
  Packet.SetInt16(1,0xB);
  Packet.SetInt16(3,0x22);
  Packet.SetInt8(5,0x1);
  Packet.SetInt32(6, who.serial);
  Packet.SetInt8(10,amount);
  Packet.SendPacket(who); 

  return 1;

endfunction 

//-----------------------------------------------------------------------------------//

function CloseContainer(who, Cont)

  if(Cont and Cont.isa(POLCLASS_CONTAINER))

    var Packet := CreatePacket(0xBF, MSGLEN_VARIABLE);
    Packet.SetInt16(3,0x16);
    Packet.SetInt32(5,0x0C);
    Packet.SetInt32(9,Cont.serial);
    Packet.SendPacket(who); 

  endif

endfunction

//-----------------------------------------------------------------------------------//
