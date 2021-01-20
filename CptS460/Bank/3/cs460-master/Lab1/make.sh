#/bin/sh

as86 -o bs.o bs.s
bcc -c -ansi t.c
ld86 -d bs.o t.o /usr/lib/bcc/libc.a
dd if=a.out of=mtximage bs=1024 count=1 conv=notrunc

actualsize=$(wc -c "a.out" | cut -f 1 -d ' ')
if [ $actualsize -gt 1024 ]; then
    echo "a.out is too big"
else 
    qemu-system-i386 -fda mtximage -no-fd-bootchk -localtime -serial mon:stdio
fi
