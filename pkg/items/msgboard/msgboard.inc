//-----------------------------------------------------------------------------

const PROP_POSTS             := "Posts";
const PROP_POST_NEXTID       := "PostNextId";
const PROP_BOARD_CTRLPID     := "BoardCtrl";
const PROP_VIEW_PIDS         := "#ViewPids";
const PROP_BOARD_VIEWING     := "#VwBoard_";
const PROP_CHECK_TIME        := "CheckTime";

//-----------------------------------------------------------------------------

const GMBEVID_POST           := 0x01;
const GMBEVID_REPLY          := 0x02;
const GMBEVID_DELETE         := 0x03;

const GMBEVID_RECREATE       := 0x10;

//-----------------------------------------------------------------------------

const GMB_CHECK_DELAY        := 1800;
const GMB_TOPIC_EXP_TIME     := 604800;

//-----------------------------------------------------------------------------

function SendBoardEvent(Board, byref ev)

  var bProc := GetProc(GetObjProperty(Board, PROP_BOARD_CTRLPID));

  if(!bProc)
    LogError("SendBoardMessage: Brak CtrlProc w tablicy ["+Board.serial+"]");
    return 0;
  endif

  return bProc.sendevent(ev);

endfunction

//-----------------------------------------------------------------------------

function SendViewersEvent(Board, byref ev)

  var ViewPids := GetObjProperty(Board, PROP_VIEW_PIDS);
  if(!ViewPids or !ViewPids.size())
    return;
  endif

  foreach Proc in ViewPids
    Proc := GetProc(Proc);
    if(Proc)
      Proc.sendevent(ev);
    endif
  endforeach

endfunction

//-----------------------------------------------------------------------------

function AddBoardViewer(Board)

  var ViewPids := GetObjProperty(Board, PROP_VIEW_PIDS);
  if(!ViewPids)
    ViewPids := array;
  endif

  ViewPids.append(GetPid());
  SetObjProperty(Board, PROP_VIEW_PIDS, ViewPids);

endfunction

//-----------------------------------------------------------------------------

function DelBoardViewer(Board)

  var ViewPids := GetObjProperty(Board, PROP_VIEW_PIDS);
  var Idx      := RetIndex(ViewPids, GetPid());

  if(Idx)
    ViewPids.erase(Idx);
    SetObjProperty(Board, PROP_VIEW_PIDS, ViewPids);
  endif

endfunction

//-----------------------------------------------------------------------------

function GetBoardMessageCount(Board)

  var Posts := GetObjProperty(Board, PROP_POSTS);
  
  if(Posts)
    return Posts.size();
  endif
  
  return 0;

endfunction

//-----------------------------------------------------------------------------
