/* jconfig.vc --- jconfig.h for Microsoft Visual C++ on Windows 95 or NT. */
/* see jconfig.doc for explanations */

#define HAVE_PROTOTYPES
#define HAVE_UNSIGNED_CHAR
#define HAVE_UNSIGNED_SHORT
/* #define void char */
/* #define const */
#undef CHAR_IS_UNSIGNED
#define HAVE_STDDEF_H
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H
#endif
#undef NEED_BSD_STRINGS
#undef NEED_SYS_TYPES_H
#undef NEED_FAR_POINTERS	/* we presume a 32-bit flat memory model */
#undef NEED_SHORT_EXTERNAL_NAMES
#undef INCOMPLETE_TYPES_BROKEN

/* Define "boolean" as unsigned char, not int, per Windows custom */
#ifndef __RPCNDR_H__		/* don't conflict if rpcndr.h already read */
typedef unsigned char boolean;
#endif
#define HAVE_BOOLEAN		/* prevent jmorecfg.h from redefining it */

#if defined(__SYMBIAN32__) && defined(__GCCE__) && !defined(_WCHAR_T_DECLARED_)
#ifndef __cplusplus
#define _WCHAR_T_DECLARED_
typedef unsigned short wchar_t; 
#endif /* __cplusplus */
#endif

#ifdef JPEG_INTERNALS

#undef RIGHT_SHIFT_IS_UNSIGNED

#endif /* JPEG_INTERNALS */

#ifdef JPEG_CJPEG_DJPEG

#undef BMP_SUPPORTED		/* BMP image file format */
#undef GIF_SUPPORTED		/* GIF image file format */
#undef PPM_SUPPORTED		/* PBMPLUS PPM/PGM image file format */
#undef RLE_SUPPORTED		/* Utah RLE image file format */
#undef TARGA_SUPPORTED		/* Targa image file format */

#define TWO_FILE_COMMANDLINE	/* optional */
#define USE_SETMODE		/* Microsoft has setmode() */
#undef NEED_SIGNAL_CATCHER
#undef DONT_USE_B_MODE
#undef PROGRESS_REPORT		/* optional */

#if defined(_WIN32_WCE) || defined(SN_TARGET_PS3) || defined(GLITCH_USE_JIL_DEVICE) || defined(NN_PLATFORM_CTR) /*3DS*/ || defined(SN_TARGET_PSP2) || defined(__ANDROID__)
	#undef USE_SETMODE
#endif

#endif /* JPEG_CJPEG_DJPEG */

#if defined(SN_TARGET_PSP2)
#define NO_GETENV
#endif
