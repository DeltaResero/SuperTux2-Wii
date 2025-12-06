#ifndef CONFIG_H
#define CONFIG_H

#define PACKAGE_NAME "supertux2"

#cmakedefine ENABLE_SQDBG

#cmakedefine ENABLE_BINRELOC
#define INSTALL_SUBDIR_BIN "${INSTALL_SUBDIR_BIN}"
#define INSTALL_SUBDIR_SHARE "${INSTALL_SUBDIR_SHARE}"

/* Wii platform detection */
#cmakedefine _WII_
#cmakedefine GEKKO

#ifdef _WII_
#define SIZEOF_VOID_P 4
#else
#define SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P}
#endif
#if SIZEOF_VOID_P == 8
#define _SQ64
#endif

#cmakedefine HAVE_ICONV_CONST
#ifdef HAVE_ICONV_CONST
#define ICONV_CONST const
#else
#define ICONV_CONST
#endif

#cmakedefine WORDS_BIGENDIAN

#cmakedefine HAVE_OPENGL

/* Define if GLEW is available */
#cmakedefine HAVE_GLEW

/* NPOT (Non-Power-Of-Two) texture support */
#cmakedefine HAVE_NPOT_TEXTURES

/* 16-bit Color Mode (Display & Textures) */
#cmakedefine ENABLE_16BIT_COLOR

/* Lightmap rendering support */
#cmakedefine HAVE_LIGHTMAP
#cmakedefine USE_ROUNDTRIP_LIGHTMAP

#cmakedefine HAVE_OPENAL
#cmakedefine USE_SDL_MIXER

#define BUILD_DATA_DIR "${BUILD_DATA_DIR}"

#define BUILD_CONFIG_DATA_DIR "${BUILD_CONFIG_DATA_DIR}"

#endif /*CONFIG_H*/
