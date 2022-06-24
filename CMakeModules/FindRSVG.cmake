#use pkg-config to find various modues
SET(FPHSA_NAME_MISMATCHED true)
INCLUDE(FindPkgConfig OPTIONAL)
UNSET(FPHSA_NAME_MISMATCHED)
IF(PKG_CONFIG_FOUND)

    INCLUDE(FindPkgConfig)

    #Version 2.35 introduces the rsvg_cleanup function which is used
    PKG_CHECK_MODULES(RSVG librsvg-2.0>=2.35)

    PKG_CHECK_MODULES(CAIRO cairo)

    IF (RSVG_FOUND AND NOT CAIRO_FOUND)
       SET(RSVG_FOUND FALSE)
    ENDIF()


ENDIF()
