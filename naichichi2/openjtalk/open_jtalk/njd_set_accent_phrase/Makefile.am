
EXTRA_DIST = Makefile.mak

DISTCLEANFILES = *.log *.out *~

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I @top_srcdir@/njd @CHARSET@

noinst_LIBRARIES = libnjd_set_accent_phrase.a

libnjd_set_accent_phrase_a_SOURCES = njd_set_accent_phrase.h \
                                     njd_set_accent_phrase.c \
                                     njd_set_accent_phrase_rule_euc_jp.h \
                                     njd_set_accent_phrase_rule_shift_jis.h \
                                     njd_set_accent_phrase_rule_utf_8.h
