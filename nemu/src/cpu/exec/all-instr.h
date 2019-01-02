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
//arith
make_EHelper(sub);
make_EHelper(add);
//logic
make_EHelper(xor);
make_EHelper(and);
//data-move
make_EHelper(lea);
