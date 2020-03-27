//-----------------------------------------------------------------------------

function drop_dungeon_anchor(NoMove := 0)

//IsDungeon(me.x, me.y, me.realm) and 

  if(!GetPrevTamed(me))
    if(!GetObjProperty(me, "Anchor"))
      drop_anchor(NoMove);
    endif
  elseif(GetObjProperty(me, "Anchor"))
    EraseObjProperty(me, "Anchor");
  endif

endfunction

//-----------------------------------------------------------------------------

function erase_anchor()

  EraseObjProperty(me, "Anchor");
  SetAnchor(me.x, me.y, 0, 0);

endfunction

//-----------------------------------------------------------------------------

function drop_anchor(NoMove := 0)

  if(!NoMove)
    if(!move_to_anchor())
      return;
    endif
  endif
  
  var cfgElem := GetNpcConfig(me);

  var dstart := CInt(cfgElem.AnchorRange);
  var psub   := CInt(cfgElem.psub);

  if(!psub)
    psub := 10;
  endif

  if(!dstart)
    dstart := 10;
  endif

  if(WalkProps)
    psub   := 0;
    dstart := NPCWALK_MAX_RANGE;
  endif

  if(!IsSetFlag(me, NPCF_NO_ANCHOR))
    SetAnchor(me.x, me.y, dstart, psub);
  endif

  SetObjProperty(me, "Anchor", { me.x, me.y, me.z, dstart, me.realm });
  return 1;

endfunction

//-----------------------------------------------------------------------------

function get_anchor()

  return GetObjProperty(me,"Anchor");

endfunction

//-----------------------------------------------------------------------------

function delay_anchor(Time)

  SetObjProperty(me, "AnchorDelayed", ReadGameClock() + Time);

endfunction

//-----------------------------------------------------------------------------

function is_anchor_delayed()

  var Delayed := Getobjproperty(me,"AnchorDelayed");

  if(Delayed and Delayed > ReadGameClock())
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function check_anchor(Range := 0)

  if(POLCore().uptime < 600)
	return 1;
  endif

  if(IsSetFlag(me, NPCF_NO_ANCHOR) or GetObjProperty(me, "#AnchorStop"))
    return 0;
  endif

  var anchor := Getobjproperty(me,"Anchor");

  if(is_anchor_delayed())
    return 0;
  endif

  if(anchor)
    if(!Range)
      Range := anchor[4];
    endif
    if(coordist(anchor[1],anchor[2],me.x,me.y) > Range)
      if(coordist(anchor[1],anchor[2],me.x,me.y) < 100 and anchor[5] == me.realm)
      
        ev := struct;
        ev.+type := EVID_RUN;
        ev.+source :=  LocStruct();
        ev.source.x := anchor[1];
        ev.source.y := anchor[2];
        ev.source.z := anchor[3];
        
        if(NpcMoveTo(ev, NMT_SMART) == 0)
          var Res := MoveObject(me, anchor[1], anchor[2], anchor[3], anchor[5]);
          if(!Res)
            LogError("[NpcAnchor]: CheckAnchor: "+me.serial+" ["+me.x+","+me.y+","+me.z+","+me.realm+" => "+Anchor[1]+","+Anchor[2]+","+Anchor[3]+" "+Anchor[5]+"] Err: "+Res.errortext, LOGF_WORLD);
          endif
        endif
        return 1;
      else
        var Res := MoveObject(me, anchor[1], anchor[2], anchor[3], anchor[5]);
        if(!Res)
          LogError("[NpcAnchor]: CheckAnchor: "+me.serial+" ["+me.x+","+me.y+","+me.z+","+me.realm+" => "+Anchor[1]+","+Anchor[2]+","+Anchor[3]+" "+Anchor[5]+"] Err: "+Res.errortext, LOGF_WORLD);
        endif
      endif
      return 1;
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function move_to_anchor()

  if(POLCore().uptime < 600)
	return 1;
  endif

  if(IsSetFlag(me, NPCF_NO_ANCHOR))
    return 1;
  endif

  var anchor := GetObjProperty(me, "Anchor");
  if(anchor)
    if(me.x == anchor[1] and me.y == anchor[2] and me.z == anchor[3] and me.realm == anchor[5])
      return -1;
    endif

    var Res := MoveObject(me, anchor[1], anchor[2], anchor[3], anchor[5]);
    if(Res)
      if(WalkProps)
        WalkProps[5] := 1;
      endif
      return 1;
    else
      LogError("[NpcAnchor]: MoveToAnchor: "+me.serial+" ["+me.x+","+me.y+","+me.z+","+me.realm+" => "+Anchor[1]+","+Anchor[2]+","+Anchor[3]+" "+Anchor[5]+"] Err: "+Res.errortext, LOGF_WORLD);
      return 0;
    endif
  endif

  return -2;

endfunction

//-----------------------------------------------------------------------------
