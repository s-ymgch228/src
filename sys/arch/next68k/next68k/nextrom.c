/*	$NetBSD: nextrom.c,v 1.4 1998/07/11 07:06:17 dbj Exp $	*/
/*
 * Copyright (c) 1998 Darrin B. Jewell
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Darrin B. Jewell
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/types.h>
#include <machine/cpu.h>

#include <next68k/next68k/seglist.h>

#include <next68k/next68k/nextrom.h>


void    next68k_bootargs __P((unsigned char *args[]));

int mon_getc(void);
int mon_putc(int c);

struct mon_global *mg;


#define	MON(type, off) (*(type *)((u_int) (mg) + off))

#define RELOC(v, t)	(*((t *)((u_int)&(v) + NEXT_RAMBASE)))

#define	MONRELOC(type, off) \
     (*(type *)((u_int) RELOC(mg,struct mon_global *) + off))


typedef int (*getcptr)(void);
typedef int (*putcptr)(int);

/*
 * Print a string on the rom console before the MMU is turned on
 */

#define ROM_PUTS(xs) \
  do { char *_s = xs + NEXT_RAMBASE; \
		while(_s && *_s) (*MONRELOC(putcptr,MG_putc))(*_s++); \
	} while(0)

/* Print a hex byte on the rom console */

char romprint_hextable[] = "0123456789abcdef";

#define ROM_PUTX(v) \
  do { \
    (*MONRELOC(putcptr,MG_putc)) \
			 ((romprint_hextable+NEXT_RAMBASE)[((v)>>4)&0xf]); \
    (*MONRELOC(putcptr,MG_putc)) \
			 ((romprint_hextable+NEXT_RAMBASE)[(v)&0xf]); \
	} while(0);

u_char rom_enetaddr[6];

void
next68k_bootargs(args)
     unsigned char *args[];
{
  RELOC(mg,char *) = args[1];

	ROM_PUTS("Welcome to NetBSD/next68k\r\n");

	ROM_PUTS("Constructing the segment list...\r\n");

	ROM_PUTS("machine type = 0x");
	ROM_PUTX(MONRELOC(char,MG_machine_type));
	ROM_PUTS("\r\nboard rev = 0x");
	ROM_PUTX(MONRELOC(char,MG_board_rev));
	ROM_PUTS("\r\n");


  /* Construct the segment list */
  {        
		u_int msize16;
		u_int msize4;
		u_int msize1;
    int i;
    int j = 0;

		if (MONRELOC(char,MG_machine_type) == NeXT_X15) {
			msize16 = 0x1000000;
			msize4  = 0x400000;
			msize1  = 0x100000;
			ROM_PUTS("Looks like a NeXT_X15\r\n");
		} else if (MONRELOC(char,MG_machine_type) == NeXT_WARP9C) {
			msize16 = 0x800000;
			msize4  = 0x200000;
			msize1  = 0x80000;			/* ? */
			ROM_PUTS("Looks like a NeXT_WARP9C\r\n");
		} else {
			msize16 = 0x100000;
			msize4  = 0x100000;
			msize1  = 0x100000;
			ROM_PUTS("Unrecognized machine_type\r\n");
		}

    for (i=0;i<N_SIMM;i++) {
			
			ROM_PUTS("Memory bank 0x");
			ROM_PUTX(i);
			ROM_PUTS(" has value 0x");
			ROM_PUTX(MONRELOC(char,MG_simm+i))
			ROM_PUTS("\r\n");
			
      if ((MONRELOC(char,MG_simm+i) & SIMM_SIZE) != SIMM_SIZE_EMPTY) {
        RELOC(phys_seg_list[j].ps_start, vm_offset_t) 
          = NEXT_RAMBASE+(i*msize16);
      }
      if ((MONRELOC(char,MG_simm+i) & SIMM_SIZE) == SIMM_SIZE_16MB) {
        RELOC(phys_seg_list[j].ps_end, vm_offset_t) = 
          RELOC(phys_seg_list[j].ps_start, vm_offset_t) +
						msize16;
        j++;
      } 
      if ((MONRELOC(char,MG_simm+i) & SIMM_SIZE) == SIMM_SIZE_4MB) {
        RELOC(phys_seg_list[j].ps_end, vm_offset_t) = 
          RELOC(phys_seg_list[j].ps_start, vm_offset_t) +
						msize4;
        j++;
      }
      if ((MONRELOC(char,MG_simm+i) & SIMM_SIZE) == SIMM_SIZE_1MB) {
        RELOC(phys_seg_list[j].ps_end, vm_offset_t) = 
          RELOC(phys_seg_list[j].ps_start, vm_offset_t) +
						msize1;
        j++;
      }
    }

		/* The NeXT ROM or something appears to reserve the very
		 * top of memory
		 */
		RELOC(phys_seg_list[j-1].ps_end, vm_offset_t) -= 0x2000;
		
    /* pmap is unhappy if it is not null terminated */
    for(;j<MAX_PHYS_SEGS;j++) {
      RELOC(phys_seg_list[j].ps_start, vm_offset_t) = 0;
      RELOC(phys_seg_list[j].ps_end, vm_offset_t) = 0;
    }
  }


	{
		int i;
		ROM_PUTS("Memory segments found:\r\n");
		for (i=0;RELOC(phys_seg_list[i].ps_start, vm_offset_t);i++) {
			ROM_PUTS("\t0x");
			ROM_PUTX((RELOC(phys_seg_list[i].ps_start, vm_offset_t)>>24)&0xff);
			ROM_PUTX((RELOC(phys_seg_list[i].ps_start, vm_offset_t)>>16)&0xff);
			ROM_PUTX((RELOC(phys_seg_list[i].ps_start, vm_offset_t)>>8)&0xff);
			ROM_PUTX((RELOC(phys_seg_list[i].ps_start, vm_offset_t)>>0)&0xff);
			ROM_PUTS(" - 0x");
			ROM_PUTX((RELOC(phys_seg_list[i].ps_end, vm_offset_t)>>24)&0xff);
			ROM_PUTX((RELOC(phys_seg_list[i].ps_end, vm_offset_t)>>16)&0xff);
			ROM_PUTX((RELOC(phys_seg_list[i].ps_end, vm_offset_t)>>8)&0xff);
			ROM_PUTX((RELOC(phys_seg_list[i].ps_end, vm_offset_t)>>0)&0xff);
			ROM_PUTS("\r\n");
		}
	}

  /* Read the ethernet address from rom, this should be done later
   * in device driver somehow.
   */
  {
    int i;
		ROM_PUTS("Ethernet address: ");
    for(i=0;i<6;i++) {
      RELOC(rom_enetaddr[i], u_char) = MONRELOC(u_char *, MG_clientetheraddr)[i];
			ROM_PUTX(RELOC(rom_enetaddr[i],u_char));
			if (i < 5) ROM_PUTS(":");
    }
		ROM_PUTS("\r\n");
  }

	ROM_PUTS("Check serial port A for console.\r\n");

}
