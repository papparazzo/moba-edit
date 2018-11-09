AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = \
    moba-edit
moba_edit_SOURCES = \
    main.cpp \
    frmmain.cpp \
    layoutwidget.cpp \
    frmselect.cpp

moba_edit_LDADD = -lMoBaCommon $(GTKMM_LIBS)
LDFLAGS += -L/usr/local/lib

AM_CPPFLAGS = $(GTKMM_CFLAGS)

CLEANFILES = *~
