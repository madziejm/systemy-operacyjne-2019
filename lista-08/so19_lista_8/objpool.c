#include "csapp.h"
#include "bitstring.h"

// INCLUDE AT THE END OF THIS FILE

typedef struct {
  int data[8];          /* contents does not matter, sizeof(...) = 32 */
} object_t;

#define ARENA_EXTRA                                                            \
  struct {                                                                     \
    size_t nitems;      /* number of items */                                  \
    size_t nfree;       /* number of free items */                             \
    void *items;        /* pointer to first item */                            \
    bitstr_t bitmap[0]; /* bitmap of free items */                             \
  }

#include "arena.h"

static arenalist_t arenas = STAILQ_HEAD_INITIALIZER(arenas);

static arena_t *init_arena(arena_t *ar) {
  /* TODO: Calculate nitems given ARENA_SIZE, size of arena_t and object_t. */
  size_t nitems = (ARENA_SIZE - sizeof(arena_t)) / (sizeof(object_t));
  nitems -= (bitstr_size(nitems) / sizeof(object_t)) + 1;
  ar->nitems = nitems;
  ar->nfree = nitems;
  size_t block_count = ar->nitems;
  // ar->bitmap = bit_alloc(block_count);
  bit_nclear(ar->bitmap, 0, block_count);
  /* Determine items address that is aligned properly. */
  // bit_set(ar->bitmap, 1);
  ar->items = arena_end(ar) - nitems * sizeof(object_t);
  return ar;
}

static void* alloc_block(arena_t *ar) {
    assert(ar->nfree > 0);
  int index;
  /* TODO: Calculate index of free block and mark it used, update nfree. */
  index = -1;

  // for (int i = 0; i < ar->nitems; i++)
  // {
  //   if(!bit_test(ar->bitmap, i))
  //   {
  //     index = i; 
  //     ar->nfree--;
  //     // bit_set;
  //     bit_set(ar->bitmap, index);
  //     // printf("found free block no %d\n", index);
  //     break; 
  //   }
  // }

  // find first unset
  bit_ffc(ar->bitmap, ar->nitems, &index);
  // index++;
  assert(index < ar->nitems);
  bit_set(ar->bitmap, index);

  ar->nfree--;


  assert(index != -1);
  return ar->items + sizeof(object_t) * index;
}

static void free_block(arena_t *ar, void *ptr) {
  int index = (ptr - ar->items) / sizeof(object_t);
  /* TODO: Determine if ptr is correct and mark it free, update nfree. */
  assert(ptr < arena_end(ar));
  assert(ar->items <= ptr);
  ar->nfree++;
  index = (ptr - ar->items) / sizeof(object_t);
  bit_clear(ar->bitmap, index);
}

static void *objalloc(void) {
  /* Find arena with at least one free item. */
  arena_t *ar = NULL;
  STAILQ_FOREACH(ar, &arenas, arenalink) {
    if (ar->nfree > 0)
      return alloc_block(ar);
  }
  /* If none found then allocate an item from new arena. */
  return alloc_block(init_arena(alloc_after_arena(&arenas, NULL)));
}

static void objfree(void *ptr) {
  if (ptr == NULL)
    return;
  arena_t *ar = find_ptr_arena(&arenas, ptr);
  assert(ar != NULL);
  free_block(ar, ptr);
}

static void objmemcheck(void) {
  arena_t *ar;
  STAILQ_FOREACH(ar, &arenas, arenalink) {
    /* Check if nfree matches number of cleared bits in bitmap. */
    size_t nused = 0;
    for (int i = 0; i < ar->nitems; i++)
      nused += bit_test(ar->bitmap, i) ? 1 : 0;
    assert(nused == ar->nitems - ar->nfree);
  }
}

/* The test */

#define MAX_PTRS 10000
#define CYCLES 100

static void *alloc_fn(int *lenp) {
  *lenp = sizeof(object_t);
  return objalloc();
}

#define free_fn objfree
#define memchk_fn objmemcheck

#include "test.h"
