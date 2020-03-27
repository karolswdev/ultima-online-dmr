//-----------------------------------------------------------------------------

use guilds;

//=============================================================================
//-----------------------------------------------------------------------------
// Guilds
//-----------------------------------------------------------------------------
//=============================================================================

function GuildAcp(Caster,CastOn)

  var CasterGd := CInt(GetObjProperty(Caster, PROP_GUILDID));
  var CastOnGd := CInt(GetObjProperty(CastOn, PROP_GUILDID));

  if(CasterGd > 0 and CastOnGd > 0)
    CasterGd := FindGuild(CasterGd);
    CastOnGd := FindGuild(CastOnGd);

    if(CasterGd and CastOnGd)
      if(CasterGd.guildid == CastOnGd.guildid)
        return 1;
      elseif(CasterGd.isallyguild(CastOnGd))
        return 1;
      elseif(CasterGd.isenemyguild(CastOnGd))
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsInAllyGuild(Caster,CastOn)

  var CasterGd := CInt(GetObjProperty(Caster, PROP_GUILDID));
  var CastOnGd := CInt(GetObjProperty(CastOn, PROP_GUILDID));

  if(CasterGd > 0 and CastOnGd > 0)
    CasterGd := FindGuild(CasterGd);
    CastOnGd := FindGuild(CastOnGd);

    if(CasterGd and CastOnGd)
      if(CasterGd.guildid == CastOnGd.guildid)
        return 1;
      elseif(CasterGd.isallyguild(CastOnGd))
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//--------------------------------------------------------------------------------

function IsInEnemyGuild(Caster,CastOn)

  var CasterGd := CInt(GetObjProperty(Caster, PROP_GUILDID));
  var CastOnGd := CInt(GetObjProperty(CastOn, PROP_GUILDID));

  if(CasterGd > 0 and CastOnGd > 0)
    CasterGd := FindGuild(CasterGd);
    CastOnGd := FindGuild(CastOnGd);

    if(CasterGd and CastOnGd)
      if(CasterGd.isenemyguild(CastOnGd))
        return 1;
      endif
    endif
  endif

  return 0;

endfunction

//-----------------------------------------------------------------------------
