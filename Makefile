
CFLAGS = -Wall -fPIE $(shell pkg-config --cflags libnl-3.0 libnl-genl-3.0)
LDFLAGS = $(shell pkg-config --libs libnl-3.0 libnl-genl-3.0)
ifdef DEBUG
CFLAGS += -ggdb
endif

OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

all : test/list-interfaces test/listtest

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

src/list.o : include/list.h
src/wifi.o : include/wifi.h include/list.h include/interface.h include/nl80211.h
src/interface.o : include/interface.h include/list.h

test/list-interfaces : $(OBJS) include/wifi.h include/list.h include/interface.h include/nl80211.h
	gcc $(CFLAGS) $(OBJS) $@.c $(LDFLAGS) -o $@

test/listtest : include/linuxlist.h include/mem.h
	gcc $(CFLAGS) $@.c $(LDFLAGS) -o $@

.PHONY : clean
clean :
	rm src/*.o test/listtest test/list-interfaces || true
