
#define MANVERSION              2
#define MANREVISION             5

#ifndef GLIDE3
#define VERSIONSTR "2.5\0"
#else
#define VERSIONSTR "3.0\0"
#endif

#if defined(CVG) || defined(VOODOO2)
#   define HWSTR   " Voodoo^2(tm)\0"
#   ifdef NT_BUILD
#     define PRODNAME "Glide(tm) for Voodoo^2\251 and Windows\256 NT\0"
#   else
#     define PRODNAME "Glide(tm) for Voodoo^2\251 and Windows\256 95/98\0"
#   endif /* NT_BUILD */
#elif defined(H3)
#   define HWSTR   " Banshee(tm)\0"
#   ifdef NT_BUILD
#     define PRODNAME "Glide(tm) for Banshee\251 and Windows\256 NT\0"
#   else
#     define PRODNAME "Glide(tm) for Banshee\251 and Windows\256 95/98\0"
#   endif /* NT_BUILD */
#else
#   define HWSTR   "Some Hoopti Chip(tm)\0"
#endif
