#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&ret_addr);
  rtl_li(&decoding.seq_eip,vaddr_read(cpu.ldtr.base + NO,4));
  printf("in raise_intr: seq_eip = 0x%x\n",decoding.seq_eip);
  //TODO();
}

void dev_raise_intr() {
}
