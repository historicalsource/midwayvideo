	section	.text

	; void setup_tlb(void) - This function is used to setup the TLB
	;
	; The TLB is setup with 32 entries of 256k each.  Entries 33 - 48
	; are setup as invalid.  The entries are set up to map virtual
	; addresses 0 - 8Meg to physical addresses 0 - 8Meg with the first
	; 256k setup write protected (to cause dereferenced NULL pointers
	; from accidently writting into the ROM code area).  This is only
	; needed as an optimization for setting up buffers used be scatter
	; gather type DMA devices (GT64010 and IDE disk DMAS).
	;
	; NOTE - Each entry in the TLB maps two pages of 128k each
	;

	xdef	setup_tlb
setup_tlb
	; Clear wired so we can set up the TLB entries
	mtc0	r0,r6

	; First set up the page mask
	li	r8,$3f		; Each TLB entry is 512k
	sll	r8,13		; Shift to where it is suposed to be
	mtc0	r8,r5		; Move it to the page mask register

	; Now we go through and set up the first 16 TLB entries to
	; map virtual addresses 0 - 8Meg to physical addresses 0 - 8Meg
	; and only allow access to pages 1/2 - 15.
	move	r8,r0		; Starting TLB entry number (0)
	move	r10,r0		; Starting physical address (0)
	li	r12,$1000	; Address increment (256k/page)
@do_next
	mtc0	r8,r0		; Select the TLB entry to set
	sll	r9,r8,19	; Generate the data for virtual page number
	mtc0	r9,r10		; Set the EntryHi
	move	r13,r10		; Get physical address to set page to
	beq	r8,r0,@1	; Is this Entry 0 ? - br = YES
	nop
	ori	r13,$1f		; Set page to cacheable non-coherent writeback
@1
	mtc0	r13,r2		; Set the EntryLo0
	add	r10,r10,r12	; Generate the next address
	move	r13,r10		; Get physical address
	ori	r13,$1f		; Set page to cacheable non-coherent writeback
	mtc0	r13,r3		; Set the EntryLo1

	tlbwi			; Write the TLB Entry

	addi	r8,1		; Increment the index
	subi	r11,r8,16	; Are we at 16 ?
	bne	r11,r0,@do_next	; br = no
	addu	r10,r10,r12	; Generate the next address (BDSLOT)


	; Set TLB entries 16 - 48 to be invalid
@do_next_invalid
	mtc0	r8,r0		; Set the index register
	sll	r9,r8,19	; Generate the data for virtual page number
	mtc0	r9,r10		; Set EntryHi
	mtc0	r10,r2		; Set EntryLo0
	add	r10,r10,r12	; Generate the next address
	mtc0	r10,r3		; Set EntryLo1

	tlbwi			; Write the TLB Entry

	addi	r8,1		; Increment the index
	subi	r11,r8,48	; Are we at 48 ?
	bne	r11,r0,@do_next_invalid	; br = no
	addu	r10,r10,r12	; Generate the next address (BDSLOT)

	; Now protect the entries from being overwritten by setting the
	; wired register - this also sets the random register to the
	; upper bound of the number of TLB entries
	li	r8,32		; Number of entries to protect
	jr	r31		; Return to caller
	mtc0	r8,r6		; Write wired register (BDSLOT)


	; get_tlb(int num, int *)
	xdef	get_tlb
get_tlb:
	mtc0	r4,r0		; Set index register
	nop
	nop
	tlbr			; Read the tlb entry
	nop
	nop
	mfc0	r8,r10		; Get EntryHi
	mfc0	r9,r2		; Get EntryLo0
	mfc0	r10,r3		; Get EntryLo1
	sw	r8,0(r5)	; Write EntryHi
	sw	r9,4(r5)	; Write EntryLo0
	sw	r10,8(r5)	; Write EntryLo1
	jr	r31
	nop	
