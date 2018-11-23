AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = \
    moba-edit
moba_edit_SOURCES = \
    main.cpp \
    frmmain.cpp \
    layoutwidget.cpp \
    frmselect.cpp \
    toolboxwidget.cpp

moba_edit_LDADD = -lMoBaCommon $(GTKMM_LIBS) ../modules/lib-tracklayout/src/libtracklayout.a
LDFLAGS += -L/usr/local/lib

AM_CPPFLAGS = -Imodules/lib-tracklayout/src $(GTKMM_CFLAGS)

CLEANFILES = *~
