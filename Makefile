
CFLAGS = -Wall -fPIE $(shell pkg-config --cflags libnl-3.0 libnl-genl-3.0)
LDFLAGS = $(shell pkg-config --libs libnl-3.0 libnl-genl-3.0)
ifdef DEBUG
CFLAGS += -ggdb
endif

OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

all : test/list-interfaces test/listtest

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

src/wifi.o : include/wifi.h include/interface.h include/nl80211.h include/linuxlist.h include/mem.h
src/interface.o : include/interface.h include/linuxlist.h include/mem.h

test/list-interfaces : test/list-interfaces.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h include/linuxlist.h include/mem.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@

test/listtest : test/listtest.c include/linuxlist.h include/mem.h
	gcc $(CFLAGS) $< $(LDFLAGS) -o $@

.PHONY : clean
clean :
	rm src/*.o test/listtest test/list-interfaces || true
