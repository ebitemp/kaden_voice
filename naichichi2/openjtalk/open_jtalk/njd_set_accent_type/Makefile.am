
EXTRA_DIST = Makefile.mak

DISTCLEANFILES = *.log *.out *~

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I @top_srcdir@/njd @CHARSET@

noinst_LIBRARIES = libnjd_set_accent_type.a

libnjd_set_accent_type_a_SOURCES = njd_set_accent_type.h \
                                   njd_set_accent_type.c \
                                   njd_set_accent_type_rule_euc_jp.h \
                                   njd_set_accent_type_rule_shift_jis.h \
                                   njd_set_accent_type_rule_utf_8.h
