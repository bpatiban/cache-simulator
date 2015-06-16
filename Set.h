#ifndef SET_H_
#define SET_H_
#include "Block.h"
class Set
{
private:
	int blkcount;
	int offsetwidth;
	int indexwidth;
	int tagwidth;
	std::vector <Block> Blocks;
public:
	Set(int cache_size, int assoc, int block_size);
	unsigned int LRUblock(void);
	Block * readBlock(unsigned int address);
	Block * writeBlock(unsigned int address, std::vector <int> data);
	Block * writethroughBlock(unsigned int address, int data);
	Block * writebackBlock(unsigned int address, int data);
	void timestampUpdate();
	~Set();
};

#endif
