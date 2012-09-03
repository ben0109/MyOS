#include "kernel/klog.h"
#include "kernel/interrupts.h"
#include "kernel/kmalloc.h"

/*
 * Basic allocation memory scheme
 *
 * Memory is first allocated in 1MB blocks. with each of these blocks comes
 * a bitmap structure that show how the memory is used.
 *
 * the 1MB is divided into 2 512KB blocks, 4 256KB blocks ... 
 * 2^15 32 bytes blocks and 2^16 16 bytes blocks.

 * For each of these blocks, a bit says whether this block was allocated or not
 * It does not say that it is free, but that a block of this size was allocated
 * here: a bigger block, enclosing that block, could have been allocated 
 * without changing this bit.
 *
 * Lookup tables give the sizes of the biggest free block in blocks of 
 * intermediate sizes (256, 4k, 64k and 1M). They are carefully kept in sync.
 */

typedef struct {
	// one bit for each of the 2^16 base chunks and the 2^16-1 compound chunks
	// => 2^17-1 bits => 2^14 bytes
	uint8_t binary_map[0x4000];

	/*
		LUTs to find the max free blocks size in intermediate sizes
		1 for the 1MB block, 16 for the 64KB block, 256 for the 4KB blocks, 
		and 4096 for the 256 bytes blocks
	*/
	uint8_t quick_max_size[0x1111];
} mem_map_t;

/*
	return the bit index for a block of the given size
	size=16: 1MB   => 1
	size=15: 512KB => 2,3
	size=14: 256KB => 4,5,6,7
	...
	size=1:  32B   => 2^15 ... 2^16-1
	size=0:  16B   => 2^16    ...    2^17-1
*/

inline int get_bit_index(int size, int index)
{
	return index + (1<<(16-size));
}

inline int get_bit(mem_map_t* map, int size, int index)
{
	int bit_index = get_bit_index(size, index);
	int bit = 1<<(bit_index&7);

	return map->binary_map[(bit_index>>3)] & bit;
}

inline void set_bit(mem_map_t* map, int size, int index, int value)
{
	int bit_index = get_bit_index(size, index);
	int bit = 1<<(bit_index&7);

	map->binary_map[(bit_index>>3)] &= ~bit;
	map->binary_map[(bit_index>>3)] |= value ? bit : 0;
}

// return the size of the biggest allocatable block in the given block
uint8_t get_max_size(mem_map_t* map, int size, int index)
{
	int bit = get_bit(map, size, index);

	// if this block is used, return 0
	if (bit==0) {
		return 0;

	} else {
		uint8_t lsize, rsize;
		switch (size) {
		case 16: return map->quick_max_size[0x0000      ];
		case 12: return map->quick_max_size[0x0001+index];
		case 8:  return map->quick_max_size[0x0011+index];
		case 4:  return map->quick_max_size[0x0111+index];
		case 0:  return bit ? 1 : 0;

		default:
			// TODO optimize
			lsize = get_max_size(map, size-1, (index<<1)+0);
			rsize = get_max_size(map, size-1, (index<<1)+1);
			if ((lsize==size) && (rsize==size)) {
				return size;
			} else {
				return (lsize<rsize) ? lsize : rsize;
			}
		}
	}
}

// update _in the LUTs_ the size of the biggest free block in the given block
void set_max_size(mem_map_t* map, int size, int index, int max)
{
	switch (size) {
	// only for LUTs
	case 16: map->quick_max_size[0x0000      ] = max;
	case 12: map->quick_max_size[0x0001+index] = max;
	case 8:  map->quick_max_size[0x0011+index] = max;
	case 4:  map->quick_max_size[0x0111+index] = max;
	default: break;
	}
}






mem_map_t* map;
void* memory;

void kmalloc_init(void *memory_bottom)
{
	map = (mem_map_t*)memory_bottom;
	memory = memory_bottom+sizeof(mem_map_t);

	// align to 16 bytes boundary
	memory = (void*)((((uint32_t	)memory)+15) & ~15);
}

// go up the tree, patching the max free values
void patch_tree_upwards(mem_map_t* map, int size, int index)
{
	while (size<=17) {
		++size;
		index >>= 1;
		int lfree = get_max_size(map, size, index);
		int rfree = get_max_size(map, size, index+1);
		int free = (lfree>rfree) ? lfree : rfree;
		set_max_size(map, size, index, free);
	}
}

void* kmalloc(uint32_t bytes)
{
	int target_size = 0;
	bytes >>= 4; // 16 bytes are the minimum
	while (bytes) {
		bytes >>= 1;
		target_size++;
	}

	if (target_size>16) {
		kpanic("tried to alloc too bit a block");
	}

	if (get_max_size(map,16,0)<target_size) {
		kpanic("could not alloc block");
	}

	// go down the tree, to find the best fit
	int size = 16;
	int index = 0;
	while (size!=target_size) {
		--size;
		index <<= 1;
		int lfree = get_max_size(map, size, index);
		int rfree = get_max_size(map, size, index+1);
		// choose right only if left doesn't fit, or if right is a better fit 
		if ((lfree<size) || ((rfree>=size) && (rfree<lfree))) {
			index++;
		}
	}

	// compute offset
	int offset = index<<(size+4);

	// mark block as used
	set_bit(map, size, index, 0);

	// patch LUTs
	set_max_size(map, size, index, 0);
	patch_tree_upwards(map, size, index);

	return memory+offset;
}

void kfree(void* address)
{
	int offset = address-memory;

	// look at nodes for that address, find the one with bit clear
	int size,index;
	for (size=0; size<=16; size++) {
		index = offset>>(size+4);
		if (get_bit(map, size, index)==0) {
			break;
		}
	}

	if (size>16) {
		kerror("could not free mem");
	}

	// mark block as free
	set_bit(map, size, index, 1);

	// patch LUTs
	set_max_size(map, size, index, size+1);
	patch_tree_upwards(map, size, index);
}

void memcpy(void* to, void* from, uint32_t size)
{
	uint32_t i;
	uint8_t* from8 = from;
	uint8_t* to8   = to;

	for (i=0; i<size; i++) {
		*to8++ = *from8++;
	}
}


