CFLAGS = -g -O2 -Wall -DGTK_DISABLE_DEPRECATED=1 -DGDK_DISABLE_DEPRECATED=1 -DG_DISABLE_DEPRECATED=1 `pkg-config --cflags gtk+-3.0`
LDLIBS = `pkg-config --libs gtk+-3.0`
LDFLAGS = -g

all : alarm en.mo cs.mo

alarm : alarm.o 
	$(CC) $(LDFLAGS) -o alarm alarm.o $(LDLIBS)

clean :
	rm -f alarm.o alarm en.mo cs.mo
	rm -rf en cs

# create po files (translations lost!)
init.po :
	msginit -l en -o en.po -i alarm.pot
	msginit -l cs -o cs.po -i alarm.pot

# update po files
cs.po : alarm.pot
	@if msgmerge --update --backup=simple cs.po editor.pot ; then \
		echo "msgmerge for cs.po - OK"; \
	else \
		echo "msgmerge for cs.po failed!"; \
	fi
en.po : alarm.pot
	msginit --no-translator -l en -o en.po -i alarm.pot

alarm.pot : alarm.c
	xgettext --output=alarm.pot --default-domain=editor --add-comments --keyword=_ alarm.c

en.mo:	en.po
	msgfmt -o en.mo en.po
	@mkdir -p en/LC_MESSAGES
	cp en.mo en/LC_MESSAGES/alarm.mo
cs.mo:	cs.po
	iconv -f iso-8859-2 -t utf-8 cs.po|msgfmt -o cs.mo -
	@mkdir -p cs/LC_MESSAGES
	cp cs.mo cs/LC_MESSAGES/alarm.mo
