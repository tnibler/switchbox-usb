switchbox: switchbox.c
	gcc switchbox.c -o switchbox -I/usr/include/hidapi -lhidapi-libusb
