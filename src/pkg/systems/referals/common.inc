//-----------------------------------------------------------------------------

const INVITE_ONLINE_TIME     := 1800;
const INVITE_ONLINE_SEC_MAX_TIME := 86400;

const REF_SECSTEP_MIN_TIME   := 172800;
const REF_FIRSTSTEP_MIN_TIME := 86400;

const REF_NUM_FOR_COIN       := 5;

const REF_REWARDS_CFG        := ":referals:rewards";

//-----------------------------------------------------------------------------

function IsReferalInviteTime(who)

  var char, i;
  for(i:=1;i<=5;i:=i+1)
    char := who.acct.GetCharacter(i);
    
    if(GetOnlineTimer(Char) > INVITE_ONLINE_SEC_MAX_TIME)
      return 0;
    endif
  endfor

  if(GetOnlineTimer(who) < INVITE_ONLINE_TIME)
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function HaveBeenInvited(who)

  return who.acct.GetProp("ReferalInvited") != error;

endfunction

//-----------------------------------------------------------------------------

function StartChooseReferer(who)

  if(!HaveBeenInvited(who))
    start_script(":referals:choose_referer", who);
  endif

endfunction

//-----------------------------------------------------------------------------
