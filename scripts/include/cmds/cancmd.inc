//-----------------------------------------------------------------------------

include "include/checks/stdchecks";

//-----------------------------------------------------------------------------

const CANCMD_CFG    := MCFG_PKG + "cancmds";

//=============================================================================
//-----------------------------------------------------------------------------
// Can Use Command
//-----------------------------------------------------------------------------
//=============================================================================

const CMDA_READ        := 0x01;
const CMDA_MODIFY      := 0x02;
const CMDA_SECRET      := 0x04;
const CMDA_ADMIN       := 0x08;
const CMDA_REAL_ADMIN  := 0x10;

//-----------------------------------------------------------------------------

function CmdHaveAccess(who, whom, Flag)

  if(whom)
    if(whom.cmdlevel > who.cmdlevel)
      if((Flag & CMDA_READ) == CMDA_READ)
        return 1;
      endif
    endif
  endif

  if(Flag & CMDA_REAL_ADMIN)
    return IsRealAdmin(who);
  endif

  if(Flag & CMDA_ADMIN)
    return (who.cmdlevel >= 4);
  elseif((Flag & (CMDA_READ | CMDA_SECRET)) == (CMDA_READ | CMDA_SECRET))
    return (who.cmdlevel >= 3);
  elseif((Flag & (CMDA_MODIFY | CMDA_SECRET)) == (CMDA_MODIFY | CMDA_SECRET))
    return (who.cmdlevel >= 4) or (who == whom);
  elseif(Flag & CMDA_MODIFY)
    return (who.cmdlevel >= 3) or (who == whom);
  elseif(Flag & CMDA_READ)
    return (who.cmdlevel >= 2);
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function CanUseCmd(who, CmdName)

  if(IsAdmin(who))
    return 1;
  endif

  var CmdAccess := GetObjProperty(who, "CmdAccess");
  if(CmdAccess)
    if(CmdAccess[CmdName])
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function AccessCmd(who, CmdName)

  var CmdAccess := GetObjProperty(who, "CmdAccess");
  if(!CmdAccess or !CmdAccess.size())
    CmdAccess := dictionary;
  endif

  if(CmdAccess[CmdName])
    return CmdAccess[CmdName];
  else
    CmdAccess[CmdName] := PolCore().systime;
    SetObjProperty(who, "CmdAccess", CmdAccess);
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function EraseAccessCmd(who, CmdName)

  var CmdAccess := GetObjProperty(who, "CmdAccess");
  if(!CmdAccess or !CmdAccess.size())
    return 0;
  endif

  if(!CmdAccess[CmdName])
    return 0;
  else
    CmdAccess.erase(CmdName);
    SetObjProperty(who, "CmdAccess", CmdAccess);
    return 1;
  endif

endfunction

//-----------------------------------------------------------------------------

function GetAccessCmd(who)

  var CmdAccess := GetObjProperty(who, "CmdAccess");
  if(!CmdAccess or !CmdAccess.size())
    CmdAccess := dictionary;
    SetObjProperty(who, "CmdAccess", CmdAccess);
  endif

  return CmdAccess;

endfunction

//-----------------------------------------------------------------------------

function SetGroupAccessCmd(who, GroupName)

  var CmdCfg := ReadConfigFile(CANCMD_CFG);

  var Cmds := GetConfigStringArray(CmdCfg[GroupName], "Command");

  if(Cmds and Cmds.size())
    
    foreach Cmd in Cmds
      AccessCmd(who, Cmd);
    endforeach

    return 1;

  else
    return 0;
  endif

endfunction

//-----------------------------------------------------------------------------

function EraseAllCmds(who)

  SetObjProperty(who, "CmdAccess", dictionary);

endfunction

//-----------------------------------------------------------------------------
