//-----------------------------------------------------------------------------
// Modified: 2005-05-03
//-----------------------------------------------------------------------------


include "include/cmds/cmds";


//-----------------------------------------------------------------------------

function MatchIP(IpMask, DestIp)

  IpMask := SplitIP(IpMask);
  DestIP := SplitIP(DestIP);

  if(IpMask and DestIP)
    var i;
    for(i:=1;i<=IpMask.size();i:=i+1)
      if(IpMask[i] != "*")
        if(IpMask[i] != DestIP[i])
          return 0;
        endif
      endif
    endfor
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function SplitIP(Ip)

  if(Ip[" "])
    return 0;
  endif

  var Dots := 0;

  while(Ip["."])
    Ip["."] := " ";
    Dots := Dots + 1;
  endwhile

  if(Dots != 3)
    return 0;
  endif

  Ip := SplitWords(Ip);

  if(Ip.size() != 4)
    return 0;
  endif

  var Correct := 0;

  foreach IpPart in Ip
    if(IpPart != "*")
      if(IpPart == "0")
        IpPart := CInt(IpPart);
      else
        IpPart := CInt(IpPart);
        if(!IpPart)
          return 0;
        endif
      endif
      Correct := 1;
    endif
  endforeach

  if(Correct)
    return Ip;
  else
    return 0;
  endif

endfunction

//=============================================================================
//-----------------------------------------------------------------------------
// Goto Player
//-----------------------------------------------------------------------------
//=============================================================================

function GoToPlayerBySerial(who,serial)

  if(!CInt(serial))
    SendSysMessage(who,"Biedny serial. S: "+serial+".",FONT_NORMAL,COLOR_RED);
    return;
  endif

  serial := CInt(serial);

  var Searching := FindPlayerBySerial(serial);
  if(!Searching)
    SendSysMessage(who,"Nie mozna bylo znalezc celu. S: "+serial+", "+Searching+".",FONT_NORMAL,COLOR_RED);
    return;
  endif

  GoToPlayer(who, Searching);

endfunction

//-----------------------------------------------------------------------------

function GoToPlayer(who, Player)

  var ToLoc := GetCmdLoc(Player, who);

  SendSysMessage(who,"Przenoszenie do "+Player.name+" x: "+ToLoc.x+" y: "+ToLoc.y+" z: "+ToLoc.z+" R: "+ToLoc.realm+".",FONT_NORMAL,COLOR_GREEN);
  LogMsg(GOTO_LOG,who,{Player,ToLoc.x,ToLoc.y,ToLoc.z,ToLoc.realm});
//  LogNotice("["+GetRealName(who)+"/"+who.acctname+"] przeniosl sie do postaci "+GetRealName(Player)+"/"+Player.acctname+"] do lokacji "+ToLoc.x+","+ToLoc.y+","+ToLoc.z+","+ToLoc.realm, LOGF_FORCE_ALERT | LOGF_ALERT_ADMINS | LOGF_SCRIPTS);

  return MoveObject(who,ToLoc.x,ToLoc.y,ToLoc.z,ToLoc.realm,MOVECHAR_FORCELOCATION);

endfunction

//-----------------------------------------------------------------------------
