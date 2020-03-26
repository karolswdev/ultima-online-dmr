//-----------------------------------------------------------------------------

include "include/mobile/tamed";
include "include/misc";

//-----------------------------------------------------------------------------

include ":party:party";

//=============================================================================
//-----------------------------------------------------------------------------
// Friends
//-----------------------------------------------------------------------------
//=============================================================================

const MAX_FRIENDS    := 60;
const NO_PARTY_CHECK := 0x01;

//-----------------------------------------------------------------------------

function IsFriend(ofwho, who, Flag := 0)

  if(who.isa(POLCLASS_NPC))
    if(IsTamed(who))
      var Master := GetMaster(who);

      if(Master.serial == ofwho.serial)
        return 1;
      elseif(Master.serial in GetFriendList(ofwho))
        return (who.murderer == ofwho.murderer); // tylko osoby o tym samym statusie, blue dla reda nie moze byc friendem
      elseif(IsInSameParty(ofwho, Master) and !(Flag & NO_PARTY_CHECK))
        return 1;
      endif

    endif
  elseif(who.serial in GetFriendList(ofwho))
    return (who.murderer == ofwho.murderer); // tylko osoby o tym samym statusie, blue dla reda nie moze byc friendem
  elseif(IsInSameParty(ofwho, who) and !(Flag & NO_PARTY_CHECK))
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function GetFriendList(who)

  if(!GetObjProperty(who,"FriendList").size() or !GetObjProperty(who,"FriendList"))
    SetObjProperty(who,"FriendList",{});
  endif

  return GetObjProperty(who,"FriendList");

endfunction

//-----------------------------------------------------------------------------

function SetFriendList(who,Friends)

  if(Friends.size() != error)
    SetObjProperty(who,"FriendList",Friends);
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------

function AddCharFriend(ofwho,WhoSer)

  var Friends := GetFriendList(ofwho);
  if(WhoSer in Friends)
    return -1;
  endif

  if(len(Friends) >= MAX_FRIENDS)
    return -2;
  endif

  Friends.append(WhoSer);
  SetFriendList(ofwho,Friends);
  return len(Friends);

endfunction

//-----------------------------------------------------------------------------

function DeleteCharFriend(ofwho,FriendSer)

  var Friends := GetFriendList(ofwho);
  if(FriendSer in Friends)
    Friends.erase(retindex(Friends,FriendSer));
    SetFriendList(ofwho,Friends);
    return 1;
  endif
  return 0;

endfunction

//-----------------------------------------------------------------------------
