//-----------------------------------------------------------------------------

include "include/objtypes/skills";

//-----------------------------------------------------------------------------

function GrabLoot(byref ev)

  if(IsSetFlag(me, NPCF_NO_LOOTING))
    return;
  elseif(IsSummoned(me) or GetPrevTamed(me))
    return;
  endif

  if(!me.backpack)
    return;
  endif

  foreach item in ListItemsNearLocationOfType(me.x, me.y, me.z,15,UOBJ_CORPSE,me.realm)

    if(CheckLineOfSight(me, item))
    
      if(GetStandingHeight(item.x, item.y, item.z,item.realm).multi)
        continue;
      endif
      
      if(Item.isa(POLCLASS_CORPSE))
        var items := array;

        foreach thingie in EnumerateItemsInContainer(item)
          if(CanGrabIt(thingie))
            items.append(thingie);
          endif
        endforeach

        if(Items.size())
          if(!RunToIt(Item, ev))
            return 0;
          endif
 
          if(Distance(me, item) < 2)
            foreach gItem in Items
              if(GrabItem(gItem, item))
                return 1;
              endif
            endforeach
          endif
        endif

      elseif(CanGrabIt(Item))

        if(!RunToIt(Item, ev))
          return 0;
        endif

        if(Distance(me, item) < 2)
          return GrabItem(Item, 0);
        endif
      endif

    endif

  endforeach
  
endfunction

//-----------------------------------------------------------------------------

function CanGrabIt(Item)

  if(GetObjProperty(Item,"#ignoreit") or !Item.movable)
    return 0;
  endif

  if(Item.isa(POLCLASS_WEAPON) or Item.isa(POLCLASS_ARMOR))
    return 1;
  endif

  if(Item.weight > 30)
    return 0;
  elseif(IsHairs(Item))
    return 0;
  elseif((Item.objtype >= UOBJ_ORES_START and Item.objtype <= UOBJ_ORES_END) or (Item.objtype >= UOBJ_INGOTS_START and Item.objtype <= UOBJ_ALL_INGOTS_END))
    return 0;
  endif

  return 1;

endfunction

//-----------------------------------------------------------------------------

function GrabItem(Item, Cont)

  if(Cont)
    if(!InInventory(Cont, Item))
      return 0;
    endif
  endif

  if(MoveItemToContainer(Item, me.backpack))
    Say("*yoink*");
    sleep(1);
    return 1;
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------

function RunToIt(Item, byref ev)

  var Count := Distance(me, item) * 2;
  var wtime := 0;

  while(Count > 0 and Distance(me, item) > 1)
    ev := wait_for_event(wtime);

    if(CanHandleEvent(ev))
      return 0;
    endif

    wtime := !WalkToward(Item);
    Count := Count - 1;
  endwhile

  ev := 0;
  return 1;

endfunction

//-----------------------------------------------------------------------------
