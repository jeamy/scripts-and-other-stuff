PROJ = golomb2
PROJFILE = golomb2.mak
DEBUG = 1

PWBRMAKE  = pwbrmake
NMAKEBSC1  = set
NMAKEBSC2  = nmake
LINKER  = link
ILINK  = ilink
LRF  = echo > NUL
BIND  = bind
RC  = rc
IMPLIB  = implib
LFLAGS_G  =  /NOE
LFLAGS_D  =  /CO /NOF /NOP
LFLAGS_R  =  /NOF /NOP
MAPFILE_D  = NUL
MAPFILE_R  = NUL
CC  = cl
CFLAGS_G  = /W3 /G2 /J
CFLAGS_D  = /Od /FPa /Zi
CFLAGS_R  = /Ot /FPa /Gs
ASM  = masm
AFLAGS_G  = /Mx /T
AFLAGS_D  = /Zi
CVFLAGS  =  /25 /E /F
RUNFLAGS  = bootsel

OBJS  = GOL2.obj
SBRS  = GOL2.sbr

all: $(PROJ).exe

.SUFFIXES: .c .sbr .obj

GOL2.obj : GOL2.C

GOL2.sbr : GOL2.C


$(PROJ).bsc : $(SBRS)
        $(PWBRMAKE) @<<
$(BRFLAGS) $(SBRS)
<<

$(PROJ).exe : $(OBJS)
!IF $(DEBUG)
        $(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_D)
$(LLIBS_G: = +^
) +
$(LLIBS_D: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_D);
<<
!ELSE
        $(LRF) @<<$(PROJ).lrf
$(RT_OBJS: = +^
) $(OBJS: = +^
)
$@
$(MAPFILE_R)
$(LLIBS_G: = +^
) +
$(LLIBS_R: = +^
) +
$(LIBS: = +^
)
$(DEF_FILE) $(LFLAGS_G) $(LFLAGS_R);
<<
!ENDIF
!IF $(DEBUG)
        $(LINKER) @$(PROJ).lrf
!ELSE
        $(LINKER) @$(PROJ).lrf
!ENDIF


.c.sbr :
!IF $(DEBUG)
        $(CC) /Zs $(CFLAGS_G) $(CFLAGS_D) /FR$@ $<
!ELSE
        $(CC) /Zs $(CFLAGS_G) $(CFLAGS_R) /FR$@ $<
!ENDIF

.c.obj :
!IF $(DEBUG)
        $(CC) /c $(CFLAGS_G) $(CFLAGS_D) /Fo$@ $<
!ELSE
        $(CC) /c $(CFLAGS_G) $(CFLAGS_R) /Fo$@ $<
!ENDIF


run: $(PROJ).exe
        $(PROJ).exe $(RUNFLAGS)

debug: $(PROJ).exe
        CV $(CVFLAGS) $(PROJ).exe $(RUNFLAGS)
