//-----------------------------------------------------------------------------

include "include/const/uof";

//-----------------------------------------------------------------------------

const UOF_PCID     := 0xEE;

//-----------------------------------------------------------------------------

function VerToStr(intVer)

  return (intVer&0xFFFF)+"."+((intVer>>16)&0xFFFF);

endfunction

//-----------------------------------------------------------------------------

function PcExtractProcInfo(byref Packet, Num, Pos := 24)

  var Procs := array;
  var i,tLen,fSize,ProcId;

  for(i:=0;i<Num;i:=i+1)
    if(Pos >= Packet.GetSize())
      break;
    endif

    ProcId := Packet.GetInt32(Pos);
    fSize  := Packet.GetInt32(Pos+4);
    tLen   := Packet.GetInt8(Pos+8);
    Pos    := Pos + 9;
    Procs.append(array(ProcId, fSize, Packet.GetString(Pos,tLen)));
    Pos    := Pos + tLen;
  endfor

  return Procs;

endfunction

//-----------------------------------------------------------------------------

function PcBuildProcInfo(byref Procs)

  var i,ProcId,fSize,fName,Found,lfName;
  var BadSizes := array(ADREN3_SIZE, ADREN3_SIZE1, ADREN3_SIZE2, VELOCITY_SIZE, NINJA_SIZE, GESTURE_SIZE, TEXTER_SIZE, COLORCONF_SIZE);
  var BadNames := array(ADREN3_NAME, ADREN3_NAME, ADREN3_NAME, VELOCITY_NAME, NINJA_NAME, GESTURE_NAME, TEXTER_NAME, COLORCONF_NAME);

  for(i:=1;i<=Procs.size();i:=i+1)
    ProcId := Procs[i][1];
    fSize  := Procs[i][2];
    fName  := Procs[i][3];
        
    GCheckNextPage(18);
    GTextLine(20, (67+(GNPCount()*20)),550,hex(CInt(ProcId)));
    
    Found := RetIndex(BadSizes, fSize);

    if(Found)
      GTextLine(100, (67+(GNPCount()*20)),1210,fSize);
      GTextLine(170, (67+(GNPCount()*20)),1210,BadNames[Found][1,5]);
    else
      GTextLine(100, (67+(GNPCount()*20)),550,fSize);
    endif

    Found  := 0;
    lfName := lower(fName);

    foreach Name in BadNames
      if(lfName[Name])
        Found := 1;
        break;
      endif
    endforeach

    if(Found)
      GTextLine(220,(67+(GNPCount()*20)),1210,fName);
    else
      GTextLine(220,(67+(GNPCount()*20)),570,fName);
    endif
  endfor

endfunction

//-----------------------------------------------------------------------------

function SendMwgCmd(who, Cmd)

  var Packet := CreatePacket(UOF_PCID, MSGLEN_VARIABLE);
  Packet.SetInt8(3, Cmd);
  Packet.SetInt32(4, GetPid());
  Packet.SendPacket(who);
  
  var ev;
  WaitForEv(ev, 5);

  return ev.packet;

endfunction

//-----------------------------------------------------------------------------

function SendMwgIntCmd(who, Cmd, Value)

  var Packet := CreatePacket(UOF_PCID, MSGLEN_VARIABLE);
  Packet.SetInt8(3, Cmd);
  Packet.SetInt32(4, GetPid());
  Packet.SetInt32(8, CInt(Value));
  Packet.SendPacket(who);
  
  var ev;
  WaitForEv(ev, 5);

  return ev.packet;

endfunction

//-----------------------------------------------------------------------------

function SendMwgStringCmd(who, Cmd, Str)

  var Packet := CreatePacket(UOF_PCID, MSGLEN_VARIABLE);
  Packet.SetInt8(3, Cmd);
  Packet.SetInt32(4, GetPid());
  Packet.SetInt8(8, len(Str));
  Packet.SetString(9, Str, 0);
  Packet.SendPacket(who);
  
  var ev;
  WaitForEv(ev, 5);

  return ev.packet;

endfunction

//-----------------------------------------------------------------------------

function GetMwGid(who)

  var Packet := SendMwgCmd(who, CUODMR_CMD_GID);

  if(Packet)
    return array(Packet.GetInt32(20), Packet.GetString(25, Packet.GetInt8(24)));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function GenerateNewMwGid(who)

  var Packet := SendMwgCmd(who, CUODMR_CMD_GENID);

  if(Packet)
    return array(Packet.GetInt32(20), Packet.GetString(25, Packet.GetInt8(24)));
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CheckUoClient(who)

  var Packet := SendMwgCmd(who, CUODMR_CMD_VERCLIENT);

  if(Packet)
    var Res   := Packet.GetInt8(20);
    if(Res == 0)
      var edLen := Packet.GetInt8(21);
      var fdErr := Packet.GetString(22,edLen);
      var esLen := Packet.GetInt8(22+edLen);
      var fsErr := Packet.GetString(23+edLen,esLen);
      var Err   := error;
      Err.errortext := "["+fdErr+"] R:["+fsErr+"]";
      return Err;
    endif
    return Res;
  endif

  return -1;

endfunction

//-----------------------------------------------------------------------------

function GetFileSpecs(who, fName)

  var Packet := SendMwgStringCmd(who, CUODMR_CMD_FILE, fName);

  if(Packet)
    var rfName := Packet.GetString(21, Packet.GetInt8(20));
    var rfSize := Packet.GetInt32(21+len(rfName));
    return array(rfName, rfSize);
  endif

  return -1;

endfunction

//-----------------------------------------------------------------------------

function WaitForEv(byref ev, Time)

  ev := 0;

  var rTime := ReadGameClock();

  while(!ev)
    ev := wait_for_event(1);
    if(rTime + Time < ReadGameClock())
      break;
    endif
  endwhile

endfunction

//-----------------------------------------------------------------------------

function SendShpsGump(who, Nr)

  var Procs := UnPack(Elems[Nr][6]);

  GInit(20,20);

  GPage(0);
  GResPic(0,0,BKGND_BLACKSLAB,600,450);
  GTextLine(20,20,500,CUnixTime(Elems[Nr][1]) + " "+Elems[Nr][2]+"/"+Elems[Nr][3]+" D: "+Elems[Nr][4]+","+Elems[Nr][5]);
  GAddPageData(565,25,2650,2651,565,413,2648,2647);
  PcBuildProcInfo(Procs);
  GSend(who);

endfunction

//-----------------------------------------------------------------------------
