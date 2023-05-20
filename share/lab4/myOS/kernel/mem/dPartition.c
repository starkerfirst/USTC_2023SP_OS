#include "../../include/myPrintk.h"


//dPartition 是整个动态分区内存的数据结构
typedef struct dPartition{
	unsigned long size;
	unsigned long firstFreeStart; 
} dPartition;	//共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp){
	myPrintk(0x5,"dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size,dp->firstFreeStart);
}

// EMB 是每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB{
	unsigned long size;
	union {
		unsigned long nextStart;    // if free: pointer to next block
        unsigned long userData;		// if allocated, belongs to user
	};	                           
} EMB;	//共占8个字节

#define EMB_size ((unsigned long)0x8)

void showEMB(struct EMB * emb){
	myPrintk(0x3,"EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}


unsigned long dPartitionInit(unsigned long start, unsigned long totalSize){
	// TODO
	/*功能：初始化内存。
	1. 在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2. 然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3. 返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。
	*/

	// init dp
	dPartition* dp = (dPartition*)start;
	dp->size = totalSize - dPartition_size;
	dp->firstFreeStart = start + dPartition_size;

	// init emb
	EMB* emb = (EMB*)dp->firstFreeStart;
	emb->size = dp->size - EMB_size;
	emb->nextStart = 0;

	return start; // dp handle is just at the start address
}


void dPartitionWalkByAddr(unsigned long dp){
	// TODO
	/*功能：本函数遍历输出EMB 方便调试
	1. 先打印dP的信息，可调用上面的showdPartition。
	2. 然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息
	*/
	
	dPartition* dp_ptr = (dPartition*)dp;
	showdPartition(dp_ptr);
	EMB* emb = (EMB*)dp_ptr->firstFreeStart;
	while(emb != 0){
		showEMB(emb);
		emb = (EMB*)emb->nextStart;
	}

}


//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
**/

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size){
	// TODO
	/*功能：分配一个空间
	1. 使用firstfit的算法分配空间，
	2. 成功分配返回首地址，不成功返回0
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。
	*/

	dPartition* dp_ptr = (dPartition*)dp;
	EMB* emb = (EMB*)dp_ptr->firstFreeStart;
	EMB* pre = 0;
	// align size to 8 bytes
	if (size % 8 != 0) size = (size / 8 + 1) * 8;

	while(emb != 0){
		if(emb->size >= size){
			// find a block
			if(emb->size - size > EMB_size){
				// split
				EMB* newEmb = (EMB*)((unsigned long)emb + EMB_size + size);
				newEmb->size = emb->size - size - EMB_size;
				newEmb->nextStart = emb->nextStart;
				
				if (pre == 0){
					dp_ptr->firstFreeStart = (unsigned long)newEmb;
				}
				else{
					pre->nextStart = (unsigned long)newEmb;
				}
			}
			else{
				// no split
				if(pre == 0){
					dp_ptr->firstFreeStart = emb->nextStart;
				}
				else{
					pre->nextStart = emb->nextStart;
				}
			}
			return (unsigned long)emb + EMB_size;
		}
		pre = emb;
		emb = (EMB*)emb->nextStart;
	}
	return 0;

}


unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start){
	// TODO
	/*功能：释放一个空间
	1. 按照对应的fit的算法释放空间
	2. 注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3. 需要考虑两个空闲且相邻的内存块的合并
	*/

	dPartition* dp_ptr = (dPartition*)dp;
	EMB* emb = (EMB*)(start - EMB_size);
	EMB* pre = 0;
	EMB* next = (EMB*)dp_ptr->firstFreeStart;

	if (start > dp_ptr->firstFreeStart + dp_ptr->size || start < (unsigned long)dp){
		return 0;
	}

	// find item in list before emb
	while((unsigned long)next != 0){
		if((unsigned long)next > (unsigned long)emb){
			break;
		}
		pre = next;
		next = (EMB*)next->nextStart;
	}

	// find next block
	
	if(pre == 0){
		// insert at head
		emb->nextStart = dp_ptr->firstFreeStart;
		dp_ptr->firstFreeStart = (unsigned long)emb;
		emb->size = (unsigned long)next - (unsigned long)emb - EMB_size;
	}
	else{
		emb->nextStart = pre->nextStart;
		pre->nextStart = (unsigned long)emb;
		emb->size = (unsigned long)next - (unsigned long)emb - EMB_size;
	}

	// front compaction
	if(pre != 0 && (unsigned long)pre + pre->size + EMB_size == (unsigned long)emb){
		pre->size += emb->size + EMB_size;
		pre->nextStart = emb->nextStart;
		emb = pre;
	}

	// back compaction
	if(next != 0 && (unsigned long)emb + emb->size + EMB_size == (unsigned long)next){
		emb->size += next->size + EMB_size;
		emb->nextStart = next->nextStart;
	}
	return 1;
}


// 进行封装，此处默认firstfit分配算法，当然也可以使用其他fit，不限制。
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size){
	return dPartitionAllocFirstFit(dp,size);
}

unsigned long dPartitionFree(unsigned long	 dp, unsigned long start){
	return dPartitionFreeFirstFit(dp,start);
}
