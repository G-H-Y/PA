#include "cpu/exec.h"
#include "cpu/cc.h"

make_EHelper(test) {
  rtl_and(&t0,&id_dest->val,&id_src->val);
  //printf("in test:t0 = %d,dest = %d,src = %d\n",t0,id_dest->val,id_src->val);
  rtl_xor(&t1,&t0,&t0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  rtl_and(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);

  rtl_xor(&t1,&t0,&t0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  //calculate and save
  rtl_xor(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  //set OFCF
  rtl_xor(&t1,&t0,&t0); //set t1=0
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  //set ZFSF
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_xor(&t1,&t0,&t0);
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  //printf("in sar: dest = %x,src = %x\n",id_dest->val,id_src->val);
  rtl_shr(&t0,&id_dest->val,&id_src->val);//logic
  //printf("in sar: res = %x\n",t0);
  rtl_msb(&t1,&id_dest->val,id_dest->width);
  if(t1){
   // printf("in sar: sign\n");
    rtl_li(&t2,0xffffffff);
    rtl_shli(&t2,&t2,(id_dest->width << 3)-id_src->val);
   // printf("in sar:t2 = 0x%x\n",t2);
    rtl_or(&t0,&t0,&t2);
  }
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decoding.opcode & 0xf;

  rtl_setcc(&t2, cc);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  rtl_not(&t0,&id_dest->val);
  operand_write(id_dest,&t0);
  print_asm_template1(not);
}
