#include "cpu/exec.h"

make_EHelper(mov) {
  //printf("in mov: id_src = %x\n",id_src->val);
  if((id_dest->addr == 37736576)&&(id_dest->type = OP_TYPE_MEM)){
    Log("id_src->type = %d, id_src->val = %d",id_src->type,id_src->val);
    Log("id_src->reg = %d",id_src->reg);
    Log("eip = 0x%x",cpu.eip);
    Log("edx = %d",cpu.edx);
  }
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();

  rtl_push(&(id_dest->val));
  // Log("in push: id_dest->val = 0x%x\n",id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&(id_dest->val));
  operand_write(id_dest,&id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  rtl_mv(&t0,&cpu.esp);
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
 // Log("before poal : esp = 0x%x",cpu.esp);
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
 // Log("after popal : esp = 0x%x",cpu.esp);
  print_asm("popa");
}

make_EHelper(leave) {
  //set esp to ebp (esp := ebp)
  rtl_lr(&t0,R_EBP,id_dest->width);
  rtl_sr(R_ESP,&t0,id_dest->width);
  //pop ebp
  rtl_pop(&t1);
  rtl_sr(R_EBP,&t1,id_dest->width);
  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_lr(&t0,R_AX,2);
    rtl_msb(&t1,&t0,2);
    if(t1) 
      rtl_li(&t2,0xffffffff);
    else 
      rtl_li(&t2,0x0);
    rtl_sr(R_DX,&t2,2);
  }
  else {
    rtl_lr(&t0,R_EAX,4);
    rtl_msb(&t1,&t0,4);
    if(t1) 
      rtl_li(&t2,0xffffffff);
    else 
      rtl_li(&t2,0x0);
    rtl_sr(R_EDX,&t2,4);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_lr(&t0,R_AL,1);
    rtl_msb(&t1,&t0,1);
    if(t1)
      rtl_li(&t2,0xffffffff);
    else
      rtl_li(&t2,0x0);
    rtl_sr(R_AH,&t2,1);
  }
  else {
    rtl_lr(&t0,R_AX,2);
    rtl_sext(&t1,&t0,2);
    rtl_sr(R_EAX,&t1,4);
    
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  //printf("in movsx: src = %x,src_addr = %x\n",id_src->val,id_src->addr);
  rtl_sext(&t0, &id_src->val, id_src->width);
  //printf("in movsx: res/t0 = %x\n",t0);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
