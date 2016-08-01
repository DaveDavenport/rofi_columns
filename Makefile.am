# Specify automake version.
AUTOMAKE_OPTIONS  = 1.11.3

noinst_LIBRARIES =

##
# Rofi the program
##
bin_PROGRAMS=rofi_columns

rofi_columns_SOURCES=\
	source/rofi_columns.c

rofi_columns_CFLAGS=\
	$(AM_CFLAGS)\
	$(glib_CFLAGS)\
	-DMANPAGE_PATH="\"$(mandir)/\""\
	-I$(top_builddir)/\
	-Werror=missing-prototypes

rofi_columns_LDADD=\
	$(glib_LIBS)\
	$(LIBS)

##
# Manpage
##
dist_man1_MANS=\
	doc/rofi_columns.1

.PHONY: indent
indent: $(rofi_columns_SOURCES) 
	uncrustify -c $(top_srcdir)/data/uncrustify.cfg --replace $^

.PHONY: cppcheck

cppcheck: $(rofi_columns_SOURCES)
	cppcheck --std=c99 --platform=unix64 --enable=all -Uerror_dialog --inconclusive  $^


update-manpage: $(top_srcdir)/doc/rofi_columns-manpage.markdown
	ronn --roff --pipe $^ > $(top_srcdir)/doc/rofi_columns.1
