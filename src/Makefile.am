AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = \
    moba-edit

moba_edit_SOURCES = \
    main.cpp \
    frmmain.cpp \
    layoutwidget.cpp \
    frmselect.cpp \
    frmnew.cpp \
    toolboxwidget.cpp

moba_edit_LDADD = \
    -lmobacommon $(GTKMM_LIBS) \
    $(srcdir)/../modules/lib-tracklayout/src/moba/libtracklayout.a \
    $(srcdir)/../modules/lib-msghandling/src/moba/libmsghandling.a

LDFLAGS += -L/usr/local/lib

AM_CPPFLAGS = \
    $(GTKMM_CFLAGS) \
    -I$(srcdir)/../modules/lib-tracklayout/src \
    -I$(srcdir)/../modules/lib-msghandling/src \
    -g3 -ggdb -O3 -Wall

CLEANFILES = *~
