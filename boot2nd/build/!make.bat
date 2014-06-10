@echo off
set CFLAGS=-Iinclude -fbuiltin -Wpadded -fpack-struct
set LFLAGS=-T ./booti386.ld -N --strip-all -Map booti386.map
set OBJECTS=./main.o ./i386.o ./lib.o ./isr.o ./pic.o ./term.o ./mem.o ./keyb.o ./serial.o ./crc32.o ./config.o ./pci.o ./ide.o ./pit.o ./udivmoddi4.o ./udivdi3.o ./rtc.o

i586-elf-gcc %CFLAGS% -c ./main.c 		-o main.o 
i586-elf-gcc %CFLAGS% -c ./i386.c 		-o i386.o 
i586-elf-gcc %CFLAGS% -c ./lib.c 		-o lib.o 
i586-elf-gcc %CFLAGS% -c ./isr.S 		-o isr.o 
i586-elf-gcc %CFLAGS% -c ./pic.c 		-o pic.o 
i586-elf-gcc %CFLAGS% -c ./term.c 		-o term.o 
i586-elf-gcc %CFLAGS% -c ./mem.c 		-o mem.o 
i586-elf-gcc %CFLAGS% -c ./keyb.c 		-o keyb.o 
i586-elf-gcc %CFLAGS% -c ./serial.c 	-o serial.o 
i586-elf-gcc %CFLAGS% -c ./crc32.c 		-o crc32.o 
i586-elf-gcc %CFLAGS% -c ./config.c 	-o config.o 
i586-elf-gcc %CFLAGS% -c ./pci.c 		-o pci.o 
i586-elf-gcc %CFLAGS% -c ./ide.c 		-o ide.o
i586-elf-gcc %CFLAGS% -c ./pit.c 		-o pit.o
i586-elf-gcc %CFLAGS% -c ./rtc.c 		-o rtc.o
i586-elf-gcc %CFLAGS% -c ./udivmoddi4.c	-o udivmoddi4.o
i586-elf-gcc %CFLAGS% -c ./udivdi3.c 	-o udivdi3.o


i586-elf-ld %OBJECTS% -o ../bin/booti386.elf %LFLAGS% 

move *.o build
move *.map build


