.globl _cnt

.data
.globl _intrcnt
_intrcnt:
    /* spurious  1  2  3  4  5  6  7*/
	.long 0, 0, 0, 0, 0, 0, 0, 0

.text

#define INTERRUPT_HANDLE(interrupt_num) \
	clrw	sp@-		;	/* ???? stack alignment?*/\
	moveml	#0xC0C0,sp@-	;	/* save a0 a1, d0, d1 */\
	addql #1,_intrcnt+interrupt_num	;/*increment interrupt counter */\
	movw	sr,sp@-		;	/* push current SR value */\
	clrw	sp@-		;	/*    padded to longword */\
	jbsr	_intrhand	;	/* handle interrupt 	 */\
	addql	#4,sp		;	/* pop SR		 */\
	moveml	sp@+,#0x0303	;	/* restore a0, a1, d0, d1*/\
	addql	#2,sp		;	/* undo stack alignment? hanck*/\
	addql #1, _cnt+V_INTR	;	/* more statistics gathering */\
	jra rei

.globl _level0intr, _level1intr, _level2intr, _level3intr, _level4intr
.globl _level5intr, _level6intr, _level7intr

.align 4
/* spurious interrupt */
_level0intr:	
	INTERRUPT_HANDLE(0)
/* system enable register 1 */
.align 4
_level1intr:	
	INTERRUPT_HANDLE(1)
/* system enable register 2, SCSI */
.align 4
_level2intr: 
	INTERRUPT_HANDLE(2)

/* system enable register 3, Ethernet */
.align 4
_level3intr:
	INTERRUPT_HANDLE(3)

/* video */
.align 4
_level4intr:
	INTERRUPT_HANDLE(4)

/* clock */
.align 4
_level5intr:
	INTERRUPT_HANDLE(5)

/* SCCs */
.align 4
_level6intr:
	INTERRUPT_HANDLE(6)

/* Memory Error/NMI */
.align 4
_level7intr:
	INTERRUPT_HANDLE(7)

_spurintr:
	
