#include "Cache.h"

Cache::Cache(int a, int b, int c)
{
		cache_size = a;
		assoc = b;
		block_size = c;
		setcount = cache_size/(assoc*block_size);
		offsetwidth = log2(block_size);
		for (int i=0; i <= (setcount-1); i++)
			Sets.push_back(Set(cache_size, assoc, block_size));
}

Block * Cache::readBlock(unsigned int address)
{
		unsigned int index = (address >> offsetwidth) & (setcount-1);
		return Sets[index].readBlock(address);
}
void Cache::timestampUpdate()
{
		for (int i=0; i <= (setcount-1); i++)
			Sets[i].timestampUpdate();
}

Block *  Cache::writeBlock(unsigned int address, std::vector <int> data)
{
		unsigned int index = (address >> offsetwidth) & (setcount-1);
		return Sets[index].writeBlock(address, data);
}

Block * Cache::writethroughBlock(unsigned int address, int data)
{
		unsigned int index = (address >> offsetwidth) & (setcount-1);
		return Sets[index].writethroughBlock(address, data);
}

Block *  Cache::writebackBlock(unsigned int address, int data)
{
		unsigned int index = (address >> offsetwidth) & (setcount-1);
		return Sets[index].writebackBlock(address, data);
}

Cache::~Cache()
{
		Sets.clear();
}

