#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
//control
make_EHelper(call);
make_EHelper(ret);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(jcc);
make_EHelper(jmp);
make_EHelper(call_rm);
make_EHelper(jmp_rm);
//arith
make_EHelper(sub);
make_EHelper(add);
make_EHelper(adc);
make_EHelper(cmp);
make_EHelper(dec);
make_EHelper(and);
make_EHelper(inc);
make_EHelper(imul2);
make_EHelper(idiv);
make_EHelper(neg);
make_EHelper(imul1);
make_EHelper(mul);
//logic
make_EHelper(xor);
make_EHelper(and);
make_EHelper(or);
make_EHelper(setcc);
make_EHelper(test);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(not);
make_EHelper(shr);
//data-move
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(leave);
make_EHelper(cltd);
make_EHelper(movsx);
//special
make_EHelper(nop);
