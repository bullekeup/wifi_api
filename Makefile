all : test/list-interface

test/list-interface : test/list.o test/wifi.o test/interface.o test/list-interfaces.o
	gcc -o test/list-interface test/list.o test/wifi.o test/interface.o test/list-interfaces.o $$(pkg-config --cflags --libs libnl-3.0 libnl-genl-3.0)

test/list.o : src/list.c include/list.h
	gcc -o test/list.o src/list.c -c

test/wifi.o : src/wifi.c include/wifi.h include/list.h include/interface.h include/nl80211.h
	gcc -o test/wifi.o src/wifi.c -c

test/interface.o : src/interface.c include/interface.h include/list.h
	gcc -o test/interface.o src/interface.c -c

test/list-interfaces.o : test/list-interfaces.c include/wifi.h include/list.h include/interface.h include/nl80211.h
	gcc -o test/list-interfaces.o test/list-interfaces.c -c
