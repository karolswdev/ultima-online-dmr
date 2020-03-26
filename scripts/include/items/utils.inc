//-----------------------------------------------------------------------------

function GetItemsInBoundary(nwLoc, seLoc)

  if(coordist(nwLoc.x, nwLoc.y, seLoc.x, seLoc.y) > 100)
    return error{"errortext" := "Zasieg pomiedzy wskazanymi lokacjami przekracza 100! To stanowczo za duzo jak na jedna grupe."};
  elseif(nwLoc.realm != seLoc.realm)
    return error{"errortext" := "Wth?! Rozne realmy?"};
  endif
  
  var Items := array;

  foreach Item in ListObjectsInBox(nwLoc.x, nwLoc.y, -127, seLoc.x, seLoc.y, 127, nwLoc.realm)
    if(Item.isa(POLCLASS_ITEM))
      Items.append(Item);
    endif
  endforeach

  if(!Items.size())
    return error{"errortext" := "Nie znaleziono przedmiotow w wybranym zasiegu."};
  endif

  return Items;

endfunction
//-----------------------------------------------------------------------------

function GetStaticsInBoundary(nwLoc, seLoc)

  if(coordist(nwLoc.x, nwLoc.y, seLoc.x, seLoc.y) > 100)
    return error{"errortext" := "Zasieg pomiedzy wskazanymi lokacjami przekracza 100! To stanowczo za duzo jak na jedna grupe."};
  elseif(nwLoc.realm != seLoc.realm)
    return error{"errortext" := "Wth?! Rozne realmy?"};
  endif
  
  var Items := array;

  foreach Item in ListStaticsInBox(nwLoc.x, nwLoc.y, -127, seLoc.x, seLoc.y, 127, ITEMS_IGNORE_MULTIS, nwLoc.realm)
    Items.append(Item);
  endforeach

  if(!Items.size())
    return error{"errortext" := "Nie znaleziono statikow w wybranym zasiegu."};
  endif

  return Items;

endfunction

//-----------------------------------------------------------------------------
