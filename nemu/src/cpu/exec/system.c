#include "cpu/exec.h"
//#include "device/port-io.h"

void difftest_skip_ref();
void difftest_skip_dut();
uint32_t pio_read_b(ioaddr_t addr);
uint32_t pio_read_w(ioaddr_t addr);
uint32_t pio_read_l(ioaddr_t addr);
void pio_write_b(ioaddr_t addr, uint32_t data);
void pio_write_w(ioaddr_t addr, uint32_t data);
void pio_write_l(ioaddr_t addr, uint32_t data);
void raise_intr(uint8_t NO, vaddr_t ret_addr);

make_EHelper(lidt) {
  //TODO();
  /*rtl_li(&cpu.ldtr.limit,(id_dest->val & 0xff));
  rtl_li(&t0,vaddr_read(id_dest->addr+4,2));
  rtl_shli(&t0,&t0,16);
  rtl_ori(&cpu.ldtr.base,&t0,id_dest->val >> 16);*/
  rtl_li(&cpu.ldtr.limit,vaddr_read(id_dest->addr,2));
  if(decoding.is_operand_size_16){
    rtl_li(&cpu.ldtr.base, vaddr_read(id_dest->addr + 2, 4) & 0x00ffffff);
  }
  else{
    rtl_li(&cpu.ldtr.base, vaddr_read(id_dest->addr + 2, 4));
  }
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(int) {
  //TODO();
  raise_intr(id_dest->val,*eip);
  //printf("in system.c int: leave!\n");
  print_asm("int %s", id_dest->str);

#if defined(DIFF_TEST) && defined(DIFF_TEST_QEMU)
  difftest_skip_dut();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&cpu.eip);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.val);
  decoding.seq_eip = 1;
  print_asm("iret");
}

make_EHelper(in) {
  //TODO();
  switch(id_dest->width){
    case 4: rtl_li(&t0,pio_read_l(id_src->val)); break;
    case 2: rtl_li(&t0,pio_read_w(id_src->val)); break;
    case 1: rtl_li(&t0,pio_read_b(id_src->val)); break;
  }
  operand_write(id_dest,&t0);
  print_asm_template2(in);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}

make_EHelper(out) {
  //TODO();
  rtl_mv(&t0,&id_src->val);
  switch(id_dest->width){
    case 4: pio_write_l(id_dest->val,t0); break;
    case 2: pio_write_w(id_dest->val,t0); break;
    case 1: pio_write_b(id_dest->val,t0); break;
  }
  print_asm_template2(out);

#if defined(DIFF_TEST)
  difftest_skip_ref();
#endif
}
