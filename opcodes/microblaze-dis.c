/* Disassemble Xilinx microblaze instructions.
   Copyright (C) 1993, 1999, 2000 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

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
 */


#include <stdio.h>
#include <sysdep.h>
#define STATIC_TABLE
#define DEFINE_TABLE

#include "microblaze-opc.h"
#include "dis-asm.h"
#include <string.h>

extern char * get_field (long, long, unsigned short);
extern char * get_field_imm (long);
extern char * get_field_imm5 (long);
extern char * get_field_imm12 (long);
extern char * get_field_imm14 (long);
extern char * get_field_unsigned_imm (long);
extern char * get_field_special (long, struct op_code_struct *);
extern unsigned long read_insn_microblaze (bfd_vma, struct disassemble_info *, struct op_code_struct **);
extern void no_abort_memory_error(int, bfd_vma, struct disassemble_info *);
extern int print_insn_microblaze (bfd_vma, struct disassemble_info *);
extern enum microblaze_instr get_insn_microblaze (long, bfd_boolean *, enum microblaze_instr_type *, short *);
extern short get_delay_slots_microblaze (long);
extern enum microblaze_instr microblaze_decode_insn (long, int *, int *, int *, int *);

#define get_field_rd(instr) get_field(instr, RD_MASK, RD_LOW)
#define get_field_r1(instr) get_field(instr, RA_MASK, RA_LOW)
#define get_field_r2(instr) get_field(instr, RB_MASK, RB_LOW)
#define get_int_field_imm(instr) ((instr & IMM_MASK) >> IMM_LOW)
#define get_int_field_r1(instr) ((instr & RA_MASK) >> RA_LOW)

char *
get_field (
     long instr, long mask,
     unsigned short low)
{
  char tmpstr[25];
  sprintf(tmpstr, "%s%d", register_prefix, (int)((instr & mask) >> low));
  return(strdup(tmpstr));
}

char *
get_field_imm (
     long instr)
{
  char tmpstr[25];
  sprintf(tmpstr, "%d", (short)((instr & IMM_MASK) >> IMM_LOW));
  return(strdup(tmpstr));
}

char *
get_field_imm5 (
     long instr)
{
  char tmpstr[25];
  sprintf(tmpstr, "%d", (short)((instr & IMM5_MASK) >> IMM_LOW));
  return(strdup(tmpstr));
}

char *
get_field_imm12 (
     long instr)
{
  char tmpstr[25];
  sprintf(tmpstr, "%s%d", fsl_register_prefix, (short)((instr & IMM12_MASK) >> IMM_LOW));
  return(strdup(tmpstr));
}

char *
get_field_imm14 (
     long instr)
{
  char tmpstr[25];
  sprintf(tmpstr, "%d", (short)((instr & IMM14_MASK) >> IMM_LOW));
  return(strdup(tmpstr));
}

char *
get_field_unsigned_imm (
     long instr)
{
  char tmpstr[25];
  sprintf(tmpstr, "%d", (int)((instr & IMM_MASK) >> IMM_LOW));
  return(strdup(tmpstr));
}

/*
  char *
  get_field_special (instr) 
  long instr;
  {
  char tmpstr[25];
  
  sprintf(tmpstr, "%s%s", register_prefix, (((instr & IMM_MASK) >> IMM_LOW) & REG_MSR_MASK) == 0 ? "pc" : "msr");
  
  return(strdup(tmpstr));
  }
*/

char *
get_field_special (
   long instr,
struct op_code_struct * op)
{
   char tmpstr[25];
   char spr[5];

   switch ( (((instr & IMM_MASK) >> IMM_LOW) ^ (op->immval_mask)) ) {
   case REG_MSR_MASK :
      strcpy(spr, "msr");
      break;
   case REG_PC_MASK :
      strcpy(spr, "pc");
      break;
   case REG_EAR_MASK :
      strcpy(spr, "ear");
      break;
   case REG_ESR_MASK :
      strcpy(spr, "esr");
      break;
   case REG_FSR_MASK :
      strcpy(spr, "fsr");
      break;      
   case REG_BTR_MASK :
      strcpy(spr, "btr");
      break;      
   case REG_PVR0_MASK :
   default :
      sprintf(spr, "pvr%d", (int)((instr & IMM_MASK) >> IMM_LOW) & 0xf);
      break;
      break;
   }
   
   sprintf(tmpstr, "%s%s", register_prefix, spr);
   return(strdup(tmpstr));
}

unsigned long
read_insn_microblaze(
   bfd_vma memaddr,
struct disassemble_info *info,
struct op_code_struct **opr)
{
  unsigned char       ibytes[4];
  int                 status;
  struct op_code_struct * op;
  unsigned long inst;

  status = info->read_memory_func (memaddr, ibytes, 4, info);

  if (status != 0) 
    {
      info->memory_error_func (status, memaddr, info);
      return 0;
    }

  if (info->endian == BFD_ENDIAN_BIG)
    inst = (ibytes[0] << 24) | (ibytes[1] << 16) | (ibytes[2] << 8) | ibytes[3];
  else if (info->endian == BFD_ENDIAN_LITTLE)
    inst = (ibytes[3] << 24) | (ibytes[2] << 16) | (ibytes[1] << 8) | ibytes[0];
  else
    abort ();

  /* Just a linear search of the table.  */
  for (op = opcodes; op->name != 0; op ++)
    if (op->bit_sequence == (inst & op->opcode_mask))
      break;

  *opr = op;
  return inst;
}

void no_abort_memory_error(int status ATTRIBUTE_UNUSED, 
			   bfd_vma memaddr ATTRIBUTE_UNUSED,
			   struct disassemble_info *info ATTRIBUTE_UNUSED) {}

int 
print_insn_microblaze (
     bfd_vma memaddr,
     struct disassemble_info * info)
{
  fprintf_ftype       fprintf = info->fprintf_func;
  void *              stream = info->stream;
  unsigned long       inst, prev_inst;
  struct op_code_struct * op, *pop;
  int                 immval = 0;
  bfd_boolean             immfound = FALSE;
  static bfd_vma prev_insn_addr = -1; /*init the prev insn addr */
  void *tmp;
 
  info->bytes_per_chunk = 4;

  tmp = info->memory_error_func;
  info->memory_error_func = no_abort_memory_error;
  inst = read_insn_microblaze (memaddr, info, &op);
  info->memory_error_func = tmp;
  if (inst == 0) {
    fprintf(stream, "Unable to read memory location 0x%x", (unsigned)memaddr);
    return 4;
  }

  if (memaddr-(info->bytes_per_chunk) == prev_insn_addr) {
    tmp = info->memory_error_func;
    info->memory_error_func = no_abort_memory_error;
    prev_inst = read_insn_microblaze (prev_insn_addr, info, &pop);
    info->memory_error_func = tmp;
    if (prev_inst == 0) {
      fprintf(stream, "Unable to read memory location 0x%x", (unsigned)memaddr);
      return 4;
    }
    if (pop->instr == imm) {
      immval = (get_int_field_imm(prev_inst) << 16) & 0xffff0000;
      immfound = TRUE;
    }
    else {
      immval = 0;
      immfound = FALSE;
    }
  }
  /* make curr insn as prev insn */
  prev_insn_addr = memaddr;

  if (op->name == 0)
    fprintf (stream, ".short 0x%04x", (unsigned)inst);
  else
    {
      fprintf (stream, "%s", op->name);
      
      switch (op->inst_type)
	{
  case INST_TYPE_RD_R1_R2:
     fprintf(stream, "\t%s, %s, %s", get_field_rd(inst), get_field_r1(inst), get_field_r2(inst));
     break;
        case INST_TYPE_RD_R1_IMM:
	  fprintf(stream, "\t%s, %s, %s", get_field_rd(inst), get_field_r1(inst), get_field_imm(inst));
	  if (info->print_address_func && get_int_field_r1(inst) == 0 && info->symbol_at_address_func) {
	    if (immfound)
	      immval |= (get_int_field_imm(inst) & 0x0000ffff);
	    else {
	      immval = get_int_field_imm(inst);
	      if (immval & 0x8000)
		immval |= 0xFFFF0000;
	    }
	    if (immval > 0 && info->symbol_at_address_func(immval, info)) {
	      fprintf (stream, "\t// ");
	      info->print_address_func (immval, info);
	    }
	  }
	  break;
	case INST_TYPE_RD_R1_IMM5:
	  fprintf(stream, "\t%s, %s, %s", get_field_rd(inst), get_field_r1(inst), get_field_imm5(inst));
	  break;
	case INST_TYPE_RD_IMM12:
	  fprintf(stream, "\t%s, %s", get_field_rd(inst), get_field_imm12(inst));
	  break;
	case INST_TYPE_R1_IMM12:
	  fprintf(stream, "\t%s, %s", get_field_r1(inst), get_field_imm12(inst));
	  break;
	case INST_TYPE_RD_SPECIAL:
	  fprintf(stream, "\t%s, %s", get_field_rd(inst), get_field_special(inst, op));
	  break;
	case INST_TYPE_SPECIAL_R1:
	  fprintf(stream, "\t%s, %s", get_field_special(inst, op), get_field_r1(inst));
	  break;
	case INST_TYPE_RD_R1:
	  fprintf(stream, "\t%s, %s", get_field_rd(inst), get_field_r1(inst));
	  break;
	case INST_TYPE_R1_R2:
	  fprintf(stream, "\t%s, %s", get_field_r1(inst), get_field_r2(inst));
	  break;
	case INST_TYPE_R1_IMM:
	  fprintf(stream, "\t%s, %s", get_field_r1(inst), get_field_imm(inst));
	  /* The non-pc relative instructions are returns, which shouldn't 
	     have a label printed */
	  if (info->print_address_func && op->inst_offset_type == INST_PC_OFFSET && info->symbol_at_address_func) {
	    if (immfound)
	      immval |= (get_int_field_imm(inst) & 0x0000ffff);
	    else {
	      immval = get_int_field_imm(inst);
	      if (immval & 0x8000)
		immval |= 0xFFFF0000;
	    }
	    immval += memaddr;
	    if (immval > 0 && info->symbol_at_address_func(immval, info)) {
	      fprintf (stream, "\t// ");
	      info->print_address_func (immval, info);
	    } else {
	      fprintf (stream, "\t\t// ");
	      fprintf (stream, "%x", immval);
	    }
	  }
	  break;
        case INST_TYPE_RD_IMM:
	  fprintf(stream, "\t%s, %s", get_field_rd(inst), get_field_imm(inst));
	  if (info->print_address_func && info->symbol_at_address_func) {
	    if (immfound)
	      immval |= (get_int_field_imm(inst) & 0x0000ffff);
	    else {
	      immval = get_int_field_imm(inst);
	      if (immval & 0x8000)
		immval |= 0xFFFF0000;
	    }
	    if (op->inst_offset_type == INST_PC_OFFSET)
	      immval += (int) memaddr;
	    if (info->symbol_at_address_func(immval, info)) {
	      fprintf (stream, "\t// ");
	      info->print_address_func (immval, info);
	    } 
	  }
	  break;
        case INST_TYPE_IMM:
	  fprintf(stream, "\t%s", get_field_imm(inst));
	  if (info->print_address_func && info->symbol_at_address_func && op->instr != imm) {
	    if (immfound)
	      immval |= (get_int_field_imm(inst) & 0x0000ffff);
	    else {
	      immval = get_int_field_imm(inst);
	      if (immval & 0x8000)
		immval |= 0xFFFF0000;
	    }
	    if (op->inst_offset_type == INST_PC_OFFSET)
	      immval += (int) memaddr;
	    if (immval > 0 && info->symbol_at_address_func(immval, info)) {
	      fprintf (stream, "\t// ");
	      info->print_address_func (immval, info);
	    } else if (op->inst_offset_type == INST_PC_OFFSET) {
	      fprintf (stream, "\t\t// ");
	      fprintf (stream, "%x", immval);
	    }
	  }
	  break;
        case INST_TYPE_RD_R2:
	  fprintf(stream, "\t%s, %s", get_field_rd(inst), get_field_r2(inst));
	  break;
  case INST_TYPE_R2:
     fprintf(stream, "\t%s", get_field_r2(inst));
     break;
  case INST_TYPE_R1:
     fprintf(stream, "\t%s", get_field_r1(inst));
     break;
  case INST_TYPE_RD_R1_SPECIAL:
     fprintf(stream, "\t%s, %s", get_field_rd(inst), get_field_r2(inst));
     break;
  case INST_TYPE_RD_IMM14:
     fprintf(stream, "\t%s, %s", get_field_rd(inst), get_field_imm14(inst));
     break;
     /* For tuqula instruction */
  case INST_TYPE_RD:
     fprintf(stream, "\t%s", get_field_rd(inst));
     break;
     
  default:
	  /* if the disassembler lags the instruction set */
	  fprintf (stream, "\tundecoded operands, inst is 0x%04x", (unsigned) inst);
	  break;
	}
    }
  
  /* Say how many bytes we consumed? */
  return 4;
}

enum microblaze_instr
get_insn_microblaze( 
  long inst,
  bfd_boolean *isunsignedimm,
  enum microblaze_instr_type *insn_type,
  short *delay_slots)
{
  struct op_code_struct * op;
  *isunsignedimm = FALSE;

  /* Just a linear search of the table.  */
  for (op = opcodes; op->name != 0; op ++)
    if (op->bit_sequence == (inst & op->opcode_mask))
      break;

  if (op->name == 0)
    return invalid_inst;
  else {
    *isunsignedimm = (op->inst_type == INST_TYPE_RD_R1_UNSIGNED_IMM);
    *insn_type = op->instr_type;
    *delay_slots = op->delay_slots;
    return op->instr;
  }
}

short
get_delay_slots_microblaze ( 
  long inst)
{
  bfd_boolean isunsignedimm;
  enum microblaze_instr_type insn_type;
  enum microblaze_instr op;
  short delay_slots;

  op = get_insn_microblaze( inst, &isunsignedimm, &insn_type, &delay_slots);
  if (op == invalid_inst)
    return 0;
  else 
    return delay_slots;
}

enum microblaze_instr
microblaze_decode_insn (
  long insn,
int *rd, int *ra, int *rb, int *imm)
{
  enum microblaze_instr op;
  bfd_boolean t1;
  enum microblaze_instr_type t2;
  short t3;

  op = get_insn_microblaze(insn, &t1, &t2, &t3);
  *rd = (insn & RD_MASK) >> RD_LOW;
  *ra = (insn & RA_MASK) >> RA_LOW;
  *rb = (insn & RB_MASK) >> RB_LOW;
  t3 = (insn & IMM_MASK) >> IMM_LOW;
  *imm = (int) t3;
  return (op);
}

unsigned long
microblaze_get_target_address (
  long inst,
  bfd_boolean immfound,
  int immval,
  long pcval,
  long r1val,
  long r2val,
  bfd_boolean *targetvalid,
  bfd_boolean *unconditionalbranch)
{
  struct op_code_struct * op;
  long targetaddr = 0;

  *unconditionalbranch = FALSE;
  /* Just a linear search of the table.  */
  for (op = opcodes; op->name != 0; op ++)
    if (op->bit_sequence == (inst & op->opcode_mask))
      break;

  if (op->name == 0) {
    *targetvalid = FALSE;
  } else if (op->instr_type == branch_inst) {
    switch (op->inst_type) {
    case INST_TYPE_R2:
      *unconditionalbranch = TRUE;
      /* fallthru */
    case INST_TYPE_RD_R2:
    case INST_TYPE_R1_R2:
      targetaddr = r2val;
      *targetvalid = TRUE;
      if (op->inst_offset_type == INST_PC_OFFSET)
	targetaddr += pcval;
      break;
    case INST_TYPE_IMM:
      *unconditionalbranch = TRUE;
      /* fallthru */
    case INST_TYPE_RD_IMM:
    case INST_TYPE_R1_IMM:
      if (immfound) {
	targetaddr = (immval << 16) & 0xffff0000;
	targetaddr |= (get_int_field_imm(inst) & 0x0000ffff);
      } else {
	targetaddr = get_int_field_imm(inst);
	if (targetaddr & 0x8000)
	  targetaddr |= 0xFFFF0000;
      }
      if (op->inst_offset_type == INST_PC_OFFSET)
	targetaddr += pcval;
      *targetvalid = TRUE;
      break;
    default:
      *targetvalid = FALSE;
      break;
    }
  } else if (op->instr_type == return_inst) {
      if (immfound) {
	targetaddr = (immval << 16) & 0xffff0000;
	targetaddr |= (get_int_field_imm(inst) & 0x0000ffff);
      } else {
	targetaddr = get_int_field_imm(inst);
	if (targetaddr & 0x8000)
	  targetaddr |= 0xFFFF0000;
      }
      targetaddr += r1val;
      *targetvalid = TRUE;
  } else {
    *targetvalid = FALSE;
  }
  return targetaddr;
}
