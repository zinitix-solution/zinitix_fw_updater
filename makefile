Zinitix_FWupdate : device.o firmware.o main.o util.o
	gcc -o Zinitix_FWupdate device.o firmware.o main.o util.o -lm -ludev

device.o : device_hid.c
	gcc -c -o device.o device_hid.c

firmware.o : firmware.c
	gcc -c -o firmware.o firmware.c

main.o : main.c
	gcc -c -o main.o main.c

util.o : util.c
	gcc -c -o util.o util.c

clean:
	rm *.o Zinitix_FWupdate
