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
  //printf("subcode=%x\n",subcode);
  switch (subcode & 0xe) {
    case CC_O:
    case CC_B:
      printf("CC_0 CC_B\n");
      TODO();
    case CC_E:
      rtl_get_ZF(&t0);
      *dest = (t0 == 1);
      break;
    case CC_NE:
      rtl_get_ZF(&t0);
      *dest = (t0 == 0);
      break;
    case CC_BE:
    case CC_S:
    TODO();
    case CC_L:
      rtl_get_SF(&t0);
      rtl_get_OF(&t1);
      printf("in cc SF/t0 = %d,OF/t1 = %d\n",t0,t1);
      *dest = (t0 == t1);
      printf("dest = %d\n",t0==t1);
      break;
    case CC_LE:
      rtl_get_ZF(&t0);
      rtl_get_SF(&t1);
      rtl_get_OF(&t2);
      *dest = ((t0 == 1) || (t1 != t2));
      break;
    default: panic("should not reach here");
    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
