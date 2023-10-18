
#define MANVERSION              3
#define MANREVISION             10

#ifdef __WIN32__
#define BUILD_NUMBER            0126
#endif


#define VERSIONSTR "3.10.00.0126\0"

#ifdef SST1
#   define HWSTR   "Voodoo Graphics(tm)\0"
#   ifdef NT_BUILD
#     define PRODNAME "Glide(tm) for Voodoo Graphics\251 and Windows\256 NT\0"
#   else
#     define PRODNAME "Glide(tm) for Voodoo Graphics\251 and Windows\256 95/98\0"
#   endif /* NT_BUILD */
#elif defined(SST96)
#   define HWSTR   " Voodoo Rush(tm)\0"
#   ifdef NT_BUILD
#     define PRODNAME "Glide(tm) for Voodoo Rush\251 and Windows\256 NT\0"
#   else
#     define PRODNAME "Glide(tm) for Voodoo Rush\251 and Windows\256 95/98\0"
#   endif /* NT_BUILD */
#elif defined(CVG) || defined(VOODOO2)
#   define HWSTR   " Voodoo^2(tm)\0"
#   ifdef NT_BUILD
#     define PRODNAME "Glide(tm) for Voodoo^2\251 and Windows\256 NT\0"
#   else
#     define PRODNAME "Glide(tm) for Voodoo^2\251 and Windows\256 95/98\0"
#   endif /* NT_BUILD */
#elif defined(H3)
#   define HWSTR   " Banshee(tm)\0"
#   define PRODNAME "Glide(tm) for Banshee\251, Windows\256 95/98, and Windows\256 NT\0"
#else
#   define HWSTR   "Some Hoopti Chip(tm)\0"
#endif
