#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

bool is_diff_page(paddr_t addr){
  paddr_t offset = addr & 0xfff;
  if(offset > 0xffb) return true;
  return false;
}

paddr_t page_translate(paddr_t addr){
  paddr_t page_entry = cpu.cr3 + (addr >> 22);
  assert(page_entry & 0x1);
  paddr_t page = pmem_rw(page_entry,uint32_t);
  assert(page & 0x1);
  paddr_t real_addr = (page << 20) | (addr & 0xfff);
  return real_addr;
}

uint32_t paddr_read(paddr_t addr, int len) {
  int map_NO = is_mmio(addr);
  if(map_NO == -1){
    assert(is_diff_page(addr));
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
    assert(is_diff_page(addr));
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
