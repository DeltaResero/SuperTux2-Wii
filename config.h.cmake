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

/* Whether GLEW is there to load OpenGL extensions with. Without it the
   build gets the plain headers and asks the driver for nothing. */
#cmakedefine HAVE_GLEW

/* Set when the hardware takes textures of any size, for builds that have no
   extension loader to ask. */
#cmakedefine HAVE_NPOT_TEXTURES

/* Texture sides are rounded up to a multiple of this. 1 leaves them alone. */
#define TEXTURE_ALIGNMENT ${TEXTURE_ALIGNMENT}

/* Which sound backends were built in. Either or both. When both are here,
   --audio-backend says which one runs. */
#cmakedefine ENABLE_OPENAL
#cmakedefine ENABLE_SDL_MIXER

#define BUILD_DATA_DIR "${BUILD_DATA_DIR}"

#define BUILD_CONFIG_DATA_DIR "${BUILD_CONFIG_DATA_DIR}"

#endif /*CONFIG_H*/
