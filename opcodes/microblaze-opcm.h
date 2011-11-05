#ifndef MICROBLAZE_OPCM
#define MICROBLAZE_OPCM

/*
 * Copyright (c) 2001 Xilinx, Inc.  All rights reserved. 
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Xilinx, Inc.  The name of the Company may not be used to endorse 
 * or promote products derived from this software without specific prior 
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Xilinx, Inc.
 * $Header:
 */

enum microblaze_instr {
   add, rsub, addc, rsubc, addk, rsubk, addkc, rsubkc, cmp, cmpu,
   addi, rsubi, addic, rsubic, addik, rsubik, addikc, rsubikc, mul, mulh, mulhu, mulhsu,
   idiv, idivu, bsll, bsra, bsrl, get, put, nget, nput, cget, cput,
   ncget, ncput, muli, bslli, bsrai, bsrli, mului, or, and, xor,
   andn, pcmpbf, pcmpbc, pcmpeq, pcmpne, sra, src, srl, sext8, sext16, wic, wdc, wdcclear, wdcflush, mts, mfs, br, brd,
   brld, bra, brad, brald, microblaze_brk, beq, beqd, bne, bned, blt,
   bltd, ble, bled, bgt, bgtd, bge, bged, ori, andi, xori, andni,
   imm, rtsd, rtid, rtbd, rted, bri, brid, brlid, brai, braid, bralid,
   brki, beqi, beqid, bnei, bneid, blti, bltid, blei, bleid, bgti,
   bgtid, bgei, bgeid, lbu, lhu, lw, lwx, sb, sh, sw, swx, lbui, lhui, lwi,
   sbi, shi, swi, msrset, msrclr, tuqula, fadd, frsub, fmul, fdiv, 
   fcmp_lt, fcmp_eq, fcmp_le, fcmp_gt, fcmp_ne, fcmp_ge, fcmp_un, flt, fint, fsqrt, 
   tget, tcget, tnget, tncget, tput, tcput, tnput, tncput,
   eget, ecget, neget, necget, eput, ecput, neput, necput,
   teget, tecget, tneget, tnecget, teput, tecput, tneput, tnecput,
   aget, caget, naget, ncaget, aput, caput, naput, ncaput,
   taget, tcaget, tnaget, tncaget, taput, tcaput, tnaput, tncaput,
   eaget, ecaget, neaget, necaget, eaput, ecaput, neaput, necaput,
   teaget, tecaget, tneaget, tnecaget, teaput, tecaput, tneaput, tnecaput,
   getd, tgetd, cgetd, tcgetd, ngetd, tngetd, ncgetd, tncgetd,
   putd, tputd, cputd, tcputd, nputd, tnputd, ncputd, tncputd,
   egetd, tegetd, ecgetd, tecgetd, negetd, tnegetd, necgetd, tnecgetd,
   eputd, teputd, ecputd, tecputd, neputd, tneputd, necputd, tnecputd,
   agetd, tagetd, cagetd, tcagetd, nagetd, tnagetd, ncagetd, tncagetd,
   aputd, taputd, caputd, tcaputd, naputd, tnaputd, ncaputd, tncaputd,
   eagetd, teagetd, ecagetd, tecagetd, neagetd, tneagetd, necagetd, tnecagetd,
   eaputd, teaputd, ecaputd, tecaputd, neaputd, tneaputd, necaputd, tnecaputd,
   invalid_inst } ;

enum microblaze_instr_type {
   arithmetic_inst, logical_inst, mult_inst, div_inst, branch_inst,
   return_inst, immediate_inst, special_inst, memory_load_inst,
   memory_store_inst, barrel_shift_inst, anyware_inst };

#define INST_WORD_SIZE 4

/* gen purpose regs go from 0 to 31 */
/* mask is reg num - max_reg_num, ie reg_num - 32 in this case */

#define REG_PC_MASK 0x8000
#define REG_MSR_MASK 0x8001
#define REG_EAR_MASK 0x8003
#define REG_ESR_MASK 0x8005
#define REG_FSR_MASK 0x8007
#define REG_BTR_MASK 0x800b
#define REG_EDR_MASK 0x800d
#define REG_PVR_MASK 0xa000

#define REG_PID_MASK   0x9000
#define REG_ZPR_MASK   0x9001
#define REG_TLBX_MASK  0x9002
#define REG_TLBLO_MASK 0x9003
#define REG_TLBHI_MASK 0x9004
#define REG_TLBSX_MASK 0x9005

#define MIN_REGNUM 0
#define MAX_REGNUM 31

#define MIN_PVR_REGNUM 0
#define MAX_PVR_REGNUM 15

#define REG_PC  32 /* PC */
#define REG_MSR 33 /* machine status reg */
#define REG_EAR 35 /* Exception reg */
#define REG_ESR 37 /* Exception reg */
#define REG_FSR 39 /* FPU Status reg */
#define REG_BTR 43 /* Branch Target reg */
#define REG_EDR 45 /* Exception reg */
#define REG_PVR 40960 /* Program Verification reg */

#define REG_PID   36864 /* MMU: Process ID reg       */
#define REG_ZPR   36865 /* MMU: Zone Protect reg     */
#define REG_TLBX  36866 /* MMU: TLB Index reg        */
#define REG_TLBLO 36867 /* MMU: TLB Low reg          */
#define REG_TLBHI 36868 /* MMU: TLB High reg         */
#define REG_TLBSX 36869 /* MMU: TLB Search Index reg */

/* alternate names for gen purpose regs */
#define REG_SP  1 /* stack pointer */
#define REG_ROSDP 2 /* read-only small data pointer */
#define REG_RWSDP 13 /* read-write small data pointer */

/* Assembler Register - Used in Delay Slot Optimization */
#define REG_AS    18
#define REG_ZERO  0
 
#define RD_LOW  21 /* low bit for RD */
#define RA_LOW  16 /* low bit for RA */
#define RB_LOW  11 /* low bit for RB */
#define IMM_LOW  0 /* low bit for immediate */

#define RD_MASK 0x03E00000
#define RA_MASK 0x001F0000
#define RB_MASK 0x0000F800
#define IMM_MASK 0x0000FFFF

// imm mask for barrel shifts
#define IMM5_MASK 0x0000001F


// FSL imm mask for get, put instructions
#define  RFSL_MASK 0x000000F

// imm mask for msrset, msrclr instructions
#define  IMM15_MASK 0x00007FFF

#endif /* MICROBLAZE-OPCM */
