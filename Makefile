
CFLAGS = -Wall -fPIE $(shell pkg-config --cflags libnl-3.0 libnl-genl-3.0) -lpcap -lpthread
LDFLAGS = $(shell pkg-config --libs libnl-3.0 libnl-genl-3.0) -lpcap -lpthread
LIBS = lib/libnetlink.a
ifdef DEBUG
CFLAGS += -ggdb
endif

OBJS = $(patsubst %.c,%.o,$(wildcard src/*.c))

all : test/list-interfaces test/listtest test/info-interface test/testscan test/testchangefreq test/testmeshid test/getip test/getmpath test/demo

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

src/wifi.o : include/wifi.h include/interface.h include/nl80211.h include/linuxlist.h include/mem.h include/util.h
src/interface.o : include/interface.h include/linuxlist.h include/mem.h include/util.h
src/scan2.o : include/network.h include/mem.h include/linuxlist.h include/scan2.h include/util.h
src/network.c : include/linuxlist.h include/network.h
src/util.c : include/mem.h include/linuxlist.h
src/ip.c : include/ip.h

test/list-interfaces : test/list-interfaces.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h include/linuxlist.h include/mem.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)

test/listtest : test/listtest.c include/linuxlist.h include/mem.h
	gcc $(CFLAGS) $< $(LDFLAGS) -o $@
	
test/info-interface : test/info-interface.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)

test/testscan : test/testscan.c $(OBJS) include/network.h include/mem.h include/linuxlist.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)
	
test/testchangefreq : test/testchangefreq.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)
	
test/testmeshid: test/testmeshid.c $(OBJS) include/wifi.h include/interface.h include/nl80211.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)
	
test/getip: test/getip.c $(OBJS) include/ip.h include/util.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)
	
test/getmpath: test/getmpath.c $(OBJS) include/route.h include/util.h include/mpath.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)

test/demo: test/demo.c $(OBJS) include/wifi.h include/interface.h include/scan2.h include/network.h include/ip.h include/mpath.h include/route.h include/util.h
	gcc $(CFLAGS) $(OBJS) $< $(LDFLAGS) -o $@ $(LIBS)
	

.PHONY : clean
clean :
	rm src/*.o test/listtest test/list-interfaces || true
