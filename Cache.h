#ifndef CACHE_H_
#define CACHE_H_
#include "Set.h"

class Cache
{
private:
	int cache_size;
	int assoc;
	int block_size;
	int setcount;
	int offsetwidth;
	std::vector <Set> Sets;
public:
	Cache(int a, int b, int c);
	Block * readBlock(unsigned int address);
	void timestampUpdate();
	Block * writeBlock(unsigned int address, std::vector <int> data);
	Block * writethroughBlock(unsigned int address, int data);
	Block * writebackBlock(unsigned int address, int data);
	~Cache();
};

#endif
