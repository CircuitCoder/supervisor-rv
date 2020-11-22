#include <stdint.h>
#include <stddef.h>
#include <assert.h>

typedef uint32_t pte_t;
#define PTE_PER_PAGE (4096 / 4)
__attribute__((always_inline)) inline
pte_t construct_pte(
  size_t ppn,
  uint8_t flags
) {
  return (ppn << 10) | flags; // We don't use rsw
}

const uint8_t PTE_FLAG_D = 1 << 7;
const uint8_t PTE_FLAG_A = 1 << 6;
const uint8_t PTE_FLAG_G = 1 << 5;
const uint8_t PTE_FLAG_U = 1 << 4;
const uint8_t PTE_FLAG_X = 1 << 3;
const uint8_t PTE_FLAG_W = 1 << 2;
const uint8_t PTE_FLAG_R = 1 << 1;
const uint8_t PTE_FLAG_V = 1 << 0;
const uint8_t PTE_FLAG_PTR = PTE_FLAG_G | PTE_FLAG_V;

// Currently we only supports RV32
// TODO(meow): supports SV39 / SV48

// Page tables
pte_t root_page_table[PTE_PER_PAGE]
  __attribute__((section(".data.pt"), aligned(4096)));
pte_t kernel_page_table[PTE_PER_PAGE]
  __attribute__((section(".data.pt"), aligned(4096)));
pte_t user_page_table[PTE_PER_PAGE]
  __attribute__((section(".data.pt"), aligned(4096)));

// Helpers
__attribute__((always_inline)) inline
void panic(char *msg) {
  // TODO: write to term
  (void) msg;

  while(1) {}
}

__attribute__((always_inline)) inline
size_t inpage_offset(void *ptr) {
  return ((size_t) ptr) & ((1<<12)-1);
}

__attribute__((always_inline)) inline
size_t page_number(void *ptr) {
  return ((size_t) ptr) >> 12;
}

// Read from here
char * const MEM_BASE = (char *) 0x80000000;
const size_t MEM_SIZE = 0x800000; // 8M RAM

void* init_page_table() {
  // Asserts that root_page_table is correctly aligned
  if(inpage_offset(root_page_table) != 0)
    panic("Root page table not correctly aligned");
  if(inpage_offset(kernel_page_table) != 0)
    panic("Kernel page table not correctly aligned");
  if(inpage_offset(user_page_table) != 0)
    panic("User page table not correctly aligned");

  // root[0x0] -> user
  root_page_table[0] = construct_pte(page_number(user_page_table), PTE_FLAG_PTR);

  // root[0x3ff] -> kernel
  root_page_table[0x3ff] = construct_pte(page_number(kernel_page_table), PTE_FLAG_PTR);

  // user[0x1] -> the last page in addressable memory, Null page is left out
  // Hence the first addressable address in U-mode is 0x1000
  root_page_table[1] = construct_pte(page_number(MEM_BASE+MEM_SIZE-4096), PTE_FLAG_PTR);

  // TODO: map kernel stack and kernel instructions

  return root_page_table;
}
