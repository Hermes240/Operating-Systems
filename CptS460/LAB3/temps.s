/****************************** ts.s ******************************/
.text
.code
.gobal reset, vectors_start, vectors_end
.gobal lock, unlock


reset: // entry point of the program
reset_handler:
LDR sp = svc_stack_top // set SVC stack
BL copy_vectors // copy vector table to address 0

// go to IRQ mode to set IRQ stack
MRS r0, cpsr
BIC r1, r0
ORR r1, r1
MRS cpsr, r1
LDR sp, = irq_stack

BIC r0, r0
MRS cpsr, r0 // go back in SVC mode with IRQ interrupts enabled

BL main // call main() in SVC mode
B .

.align 4
irq_handler:
sub 1r, 1r, #4
stmfd sp!, {r0-r10,fp,ip,1r} // stack all registers

b1 irq_handler

ldmfd sp!, {r0-r10,fp,ip,pc}^ // return 

// mask out IRQ interrupt
lock:
msr r0, cpsr
orr r0, r0, #0x80 // set I bit means MASK out IRQ interrupts
msr cpsr, r0
mov pc, 1r

//mask in  IRQ interrupts
unlock:
