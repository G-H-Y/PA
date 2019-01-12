#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

bool is_diff_page(paddr_t addr,int len){
  paddr_t page_offset = ((addr >> 12) & 0x3ff) << 2;
  paddr_t max_offset = 0xfff - len;
  if(page_offset > max_offset) return true;
  return false;
}

paddr_t page_translate(paddr_t addr){
  paddr_t page_addr = cpu.cr3 + ((addr >> 22) << 2);
  assert(page_addr & 0x1);
  paddr_t page_offset = ((addr >> 12) & 0x3ff) << 2;
  paddr_t page_entry = pmem_rw(page_addr + page_offset, uint32_t);
  assert(page_entry & 0x1);
  paddr_t real_addr = (page_entry & 0xfffff000) | (addr & 0xfff);
  return real_addr;
}

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1){
    assert(is_diff_page(addr,len));
    paddr_t phy_addr = page_translate(addr);
    return pmem_rw(phy_addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
  else
  {
    return mmio_read(addr,len,map_NO);
  }
    
}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1){
    assert(is_diff_page(addr,len));
    paddr_t phy_addr = page_translate(addr);
    memcpy(guest_to_host(phy_addr), &data, len);
  }
  else{
    mmio_write(addr,len,data,map_NO);
  }   
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  paddr_write(addr, data, len);
}
