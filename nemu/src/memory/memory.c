#include "nemu.h"
#include "device/mmio.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({                                       \
  Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
  guest_to_host(addr);                                                        \
})

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

bool is_diff_page(paddr_t addr, int len)
{
  paddr_t page_offset = addr & 0xfff;
  paddr_t max_offset = 0x1000 - len;
  if (page_offset > max_offset)
    return true;
  return false;
}

paddr_t page_translate(paddr_t addr)
{
  
  paddr_t direct_entry_addr = (cpu.cr3.val & 0xfffff000) | ((addr >> 22) << 2);
  paddr_t direct_entry = pmem_rw(direct_entry_addr, uint32_t);
  if (!(direct_entry & 0x1)){
    //Log("cr3 = %d",cpu.cr3.val);
    Log("va = %d,ptr = %d,ptr+offset = %d,ptr[offset] = %d",addr,cpu.cr3.val,direct_entry_addr,direct_entry);
    assert(0);
  }
    
  paddr_t page_addr = direct_entry & 0xfffff000;
  paddr_t page_offset = ((addr >> 12) & 0x3ff) << 2;
  paddr_t page_entry_addr = page_addr | page_offset;
  paddr_t page_entry = pmem_rw(page_entry_addr, uint32_t);
  if (!(page_entry & 0x1)){
    Log("page entry : addr = 0x%x",addr);
    assert(0);
  }
    
  paddr_t real_addr = (page_entry & 0xfffff000) | (addr & 0xfff);
  return real_addr;
}

uint32_t paddr_read(paddr_t addr, int len)
{
  int map_NO = is_mmio(addr);
  if (map_NO == -1)
  {
    //Log("in read");
    if (cpu.cr0.paging)
    {
      //Log("translate addr");
      if (is_diff_page(addr, len))
      {
        Log("addr = 0x%x,len = %d", addr, len);
        assert(0);
      }
      paddr_t phy_addr = page_translate(addr);
      return pmem_rw(phy_addr, uint32_t) & (~0u >> ((4 - len) << 3));
    }
    else
      return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  }
  else
  {
    return mmio_read(addr, len, map_NO);
  }
}

void paddr_write(paddr_t addr, uint32_t data, int len)
{
  int map_NO = is_mmio(addr);
  if (map_NO == -1)
  {
   //Log("in write");
    if (cpu.cr0.paging)
    {
       //Log("translate addr");
      if (is_diff_page(addr, len))
      {
        Log("addr = 0x%x,len = %d", addr, len);
        assert(0);
      }
      paddr_t phy_addr = page_translate(addr);
      memcpy(guest_to_host(phy_addr), &data, len);
    }
    else
    {
      memcpy(guest_to_host(addr), &data, len);
    }
  }
  else
  {
    mmio_write(addr, len, data, map_NO);
  }
}

uint32_t vaddr_read(vaddr_t addr, int len)
{
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, uint32_t data, int len)
{
  paddr_write(addr, data, len);
}
