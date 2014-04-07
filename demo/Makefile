CC=m68k-elf-gcc

CFLAGS=-m68000 -O1 -I../libmega/
LDFLAGS=-T ldscript -nostdlib -L../libmega -lmega

OBJS = sega.o main.o spiral.o forcer_1.o vector_balls.o sprites.o vert.o stdlib.o d_sega.o forcer_2.o polygon.o vax.o forcer_phone.o plazma.o test.o rotate.o vertical.o test2.o waitroom.o indoctor.o prescrip.o nurse.o secret.o pillz.o

rom.bin: rom.
	m68k-elf-objcopy -O binary rom. rom.bin
	checksum rom.bin
#	m68k-elf-strip -R .comment rom.
#	objdump -h rom.

rom.: ${OBJS}
	m68k-elf-gcc ${CFLAGS} ${OBJS} -o rom. ${LDFLAGS}

%.o: %.c
	m68k-elf-gcc -c ${CFLAGS} -o $@ $<

#vertical.o: vertical.c
#	m68k-elf-gcc -c vertical.c -m68000 -O3 -I../libmega/ -o vertical.o


sega.o: sega.s
	m68k-elf-gcc -c ${CFLAGS} -o sega.o sega.s

clean:
	rm -rf *.o rom. rom.bin
