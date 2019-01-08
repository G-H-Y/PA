#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&ret_addr);
  rtl_li(&t0,vaddr_read(cpu.ldtr.base + (NO << 3),4) & 0x0000ffff);
  rtl_li(&t1,vaddr_read(cpu.ldtr.base + (NO << 3) + 4,4) & 0xffff0000);
  rtl_or(&t2,&t0,&t1);
  rtl_li(&decoding.seq_eip,t2);
  //printf("in raise_intr: seq_eip = 0x%x, +NO = %d\n",decoding.seq_eip,cpu.ldtr.base + NO);
  //TODO();
}

void dev_raise_intr() {
}
