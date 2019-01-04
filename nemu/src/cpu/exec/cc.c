#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  printf("subcode=%x\n",subcode);
  switch (subcode & 0xe) {
    case CC_O:
      printf("CC_0\n");
      TODO();
    case CC_B:
      rtl_get_CF(&t0);
      *dest = (t0 == 1);
      break;
    case CC_E:
      rtl_get_ZF(&t1);
      *dest = (t1 == 1);
      break;
    case CC_NE:
      rtl_get_ZF(&t1);
      *dest = (t1 == 0);
      break;
    case CC_BE:
      rtl_get_CF(&t0);
      rtl_get_ZF(&t1);
      *dest = ((t0 == 1)||(t1 == 1));
      break;
    case CC_S:
    printf("CC_S\n");
    TODO();
    case CC_L:
      rtl_get_SF(&t1);
      rtl_get_OF(&t2);
      printf("in cc SF/t1 = %d,OF/t2 = %d\n",t1,t2);
      *dest = (t1 != t2);
      printf("dest = %d\n",*dest);
      break;
    case CC_LE:
      rtl_get_ZF(&t1);
      rtl_get_SF(&t2);
      rtl_get_OF(&t3);
      *dest = ((t1 == 1) || (t2 != t3));
      break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
