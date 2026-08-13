#ifndef CONFIG_H
#define CONFIG_H

#define PACKAGE_NAME "supertux2"

#cmakedefine ENABLE_SQDBG

#cmakedefine ENABLE_BINRELOC
#define INSTALL_SUBDIR_BIN "${INSTALL_SUBDIR_BIN}"
#define INSTALL_SUBDIR_SHARE "${INSTALL_SUBDIR_SHARE}"

#define SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P}
#if SIZEOF_VOID_P == 8
#define _SQ64
#endif

#cmakedefine HAVE_ICONV_CONST
#ifdef HAVE_ICONV_CONST
#define ICONV_CONST const
#else
#define ICONV_CONST
#endif

#cmakedefine HAVE_OPENGL

/* Which sound backends were built in. Either or both. When both are here,
   --audio-backend says which one runs. */
#cmakedefine ENABLE_OPENAL
#cmakedefine ENABLE_SDL_MIXER

#define BUILD_DATA_DIR "${BUILD_DATA_DIR}"

#define BUILD_CONFIG_DATA_DIR "${BUILD_CONFIG_DATA_DIR}"

#endif /*CONFIG_H*/
