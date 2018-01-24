
CFLAGS = -Wall -fPIE $(shell pkg-config --cflags libnl-3.0 libnl-genl-3.0) -lpcap -lpthread
LDFLAGS = $(shell pkg-config --libs libnl-3.0 libnl-genl-3.0) -lpcap -lpthread
ifdef DEBUG
CFLAGS += -ggdb
endif

OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

all : test/list-interfaces test/listtest test/info-interface test/testscan test/testchangefreq test/testmeshid

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

src/wifi.o : include/wifi.h include/interface.h include/nl80211.h include/linuxlist.h include/mem.h include/util.h
src/interface.o : include/interface.h include/linuxlist.h include/mem.h include/util.h
src/scan2.o : include/network.h include/mem.h include/linuxlist.h include/scan2.h include/util.h
src/network.c : include/linuxlist.h include/network.h
src/util.c : include/mem.h include/linuxlist.h
src/ip.c : 

test/list-interfaces : test/list-interfaces.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h include/linuxlist.h include/mem.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@

test/listtest : test/listtest.c include/linuxlist.h include/mem.h
	gcc $(CFLAGS) $< $(LDFLAGS) -o $@
	
test/info-interface : test/info-interface.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@

test/testscan : test/testscan.c $(OBJS) include/network.h include/mem.h include/linuxlist.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@
	
test/testchangefreq : test/testchangefreq.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@
	
test/testmeshid: test/testmeshid.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@
	

.PHONY : clean
clean :
	rm src/*.o test/listtest test/list-interfaces || true
