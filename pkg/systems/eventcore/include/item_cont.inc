//-----------------------------------------------------------------------------

function CheckAutoEventItemInsert(who, Cont, item)

  var TopCont := GetTopCont(Cont);
  var Source := __IfElse(TopCont, TopCont.container, Cont.container);

  if(!Source)
    Source := who;
  endif

  if(IsPlayer(Source))
    if(GetObjProperty(Source, "AutoEvent"))
      if(IsAutoEventItem(Item) or IsAutoEventItem(Cont))
        var Data := struct;
        Data.+item := item;
        Data.+container := Cont;
        Data.+source := Source;

        SendEventMessageForObject(Source, MSGID_ITEM_INSERTED, Data);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------

function CheckAutoEventItemRemove(who, Cont, item)

  var TopCont := GetTopCont(Cont);
  var Source := __IfElse(TopCont, TopCont.container, Cont.container);

  if(!Source)
    Source := who;
  endif

  if(IsPlayer(Source))
    if(GetObjProperty(Source, "AutoEvent"))
      if(IsAutoEventItem(Item) or IsAutoEventItem(Cont))
        var Data := struct;
        Data.+item := item;
        Data.+container := Cont;
        Data.+source := Source;

        SendEventMessageForObject(Source, MSGID_ITEM_REMOVED, Data);
      endif
    endif
  endif

endfunction

//-----------------------------------------------------------------------------
