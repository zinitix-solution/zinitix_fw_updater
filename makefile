zinitix_fw_updater : device.o firmware.o main.o util.o hid.o
	gcc -o zinitix_fw_updater device.o firmware.o main.o util.o hid.o -lm -ludev

device.o : device_hid.c
	gcc -c -o device.o device_hid.c

firmware.o : firmware.c
	gcc -c -o firmware.o firmware.c

main.o : main.c
	gcc -c -o main.o main.c

hid.o : hid.c
	gcc -c -o hid.o hid.c

util.o : util.c
	gcc -c -o util.o util.c

clean:
	rm *.o zinitix_fw_updater