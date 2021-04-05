TARGET = PSPFileBrowser
OBJS = main.o mp3player.o graphics.o framebuffer.o libccc.o intraFont.o libio.o converter.o \
getDir.o parseDate.o draw.o intraFontJP.o TextViewer.o SystemViewer.o CpuClockChanger.o OSK.o Property.o Error.o Readme.o FlieMenuAction.o

INCDIR =
CFLAGS = -Os -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = 
LIBS = -lpspgum -lpspgu -lpng -lz -lm -lmad -lpspaudiolib -lpspaudio -lpsprtc -lpsppower -lpspreg -lpspkubridge

EXTRA_TARGETS   = EBOOT.PBP
PSP_EBOOT_TITLE = PSP File Browser Ver 1.00
PSP_EBOOT_ICON = icon0.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak