    BOOTSEG = 0x9000            ! Boot block is loaded again to here
    SSP = 8192                  ! Stack pointer at SS+8KB
    
    .globl _main, _prints               ! IMPORT symbols from C code
    .globl _getc, _putc, _error, _readfd, _getblk, _setes, _inces         ! EXPORT symbols to C code

! Only one SECTOR loaded at (0000, 7C00). Load entire block to 0x90000

    mov ax,#BOOTSEG              ! set ES to 0x9000
    mov es,ax
    xor bx,bx                   ! clear BX = 0

! Call BIOS INT-13 to read BOOT BLOCK to (segment,offset)=(0x9000,0)

    xor dx,dx                   ! DH=head=0, DL=drive=0
    xor cx,cx                   ! CH=cylinder, CL=sector
    incb cl                     ! BIOS counts sector from 1
    mov ax,#0x0202              ! AH=READ AL=2 sectors
    int 0x13                    ! cal BIOS INT-13

    jmpi start,BOOTSEG           ! CS=0x9000, IP=start

start:
    mov ax,cs                   ! Set segment registers for CPU
    mov ds,ax                   ! we know ES,CS=0x9000. Let DS=CS
    mov ss,ax                   ! SS = CS ===> all point at 0x9000
    mov es,ax 
    mov sp,#SSP                ! SP = 8192 above SS=0x9000

!------------------ Optional
    mov ax,#0x0012              ! Call BIOS for 640x480 color mode
    int 0x10
!------------------
    call _main                  ! call main() in C

    jmpi 0,0x1000

! I/O functions

! char getc()      function: returns a char
_getc:
    xorb ah,ah                  ! clear ah
    int 0x16                    ! call BIOS to get a char in AX
    ret

! void putc(charc) function: print a char
_putc:
    push bp
    mov bp,sp

    movb al,4[bp]               ! get the char into aL
    movb ah,#14                 ! aH = 14
    movb bl,#0x0D               ! bL = color
    int 0x10                    ! call BIOS to display the char

    pop bp
    ret

! readfd(cyl, head, sector, buf)
!         4     6     8      10
_readfd:
    push bp
    mov bp,sp                   ! bp = stack frame pointer
    
    movb dl, #0x00              ! drive 0 = FD0
    movb dh, 6[bp]              ! head
    movb cl, 8[bp]              ! sector
    incb cl
    movb ch, 4[bp]              ! cyl
    mov bx, 10[bp]              ! BX=buf ==> memory addr=(ES,BX)
    mov ax, #0x0202             ! READ 2 sectors to (EX,BX)

    int 0x13                    ! call BIOS to read the block
    jb _error                   ! to error if CarryBit is on [read failed]

    pop bp
    ret

! void set_es(unigned short segnment) set ES register to segment
_setes:
    push bp
    mov bp,sp
    mov ax,4[bp]
    mov es,ax
    pop bp
    ret

! void inces() inc ES by 0x40, or 1kb
_inces:
    mov ax,es
    add ax,#0x40
    mov es,ax
    ret

! error & reboot
_error:
    mov bx, #bad
    push bx
    call _prints
    int 0x19                    !reboot

bad: .asciz "Error!"

