 
#include "Set.h"
Set::Set(int cache_size, int assoc, int block_size)
{
		blkcount = assoc;
		offsetwidth = log2(block_size);
		indexwidth = log2(cache_size/(block_size*assoc));
		tagwidth = 32 - offsetwidth - indexwidth;

		int tempsum = 0;
		for (int i=0; i<=tagwidth-1; i++)
		{
			tempsum = tempsum + pow(2,i);
		}
		tagwidth = tempsum +1;

		for (int i=0; i <= (blkcount-1); i++)
			Blocks.push_back(Block(block_size));
}

unsigned int Set::LRUblock(void)
{
		unsigned int z;
		int x = Blocks[0].timestamp;
		for (int i=0; i <= (blkcount-1); i++)
		{
			if (Blocks[i].timestamp > x)
			{
				x = Blocks[i].timestamp;
				z = i;
			}
		}
		return z;

}

Block * Set::readBlock(unsigned int address)
{
		unsigned int tag = ((address >> offsetwidth) >> indexwidth) & (tagwidth-1);
		int i = 0;
		while (i <= (blkcount-1))
		{
			if ((tag == Blocks[i].tag) && (Blocks[i].valid == 1))
			{
				Blocks[i].getWord(address);
				return &Blocks[i];
				break;
			}
			i++;
		}
		return NULL;
}

Block * Set::writeBlock(unsigned int address, std::vector <int> data)
{
		unsigned int y = LRUblock();
		unsigned int tag = ((address >> offsetwidth) >> indexwidth) & (tagwidth-1);
		Block * temp = &Blocks[y];
		Blocks[y].writeWords(address, data);
		Blocks[y].tag = tag;
		return temp;
}

Block * Set::writethroughBlock(unsigned int address, int data)
{
		unsigned int tag = ((address >> offsetwidth) >> indexwidth) & (tagwidth-1);
		int i = 0;
		while (i <= (blkcount-1))
		{
			if ((tag == Blocks[i].tag) && (Blocks[i].valid == 1))
			{
				Blocks[i].writeWord(address,data);
				return &Blocks[i];
				break;
			}
			i++;
		}
		return NULL;
}

Block * Set::writebackBlock(unsigned int address, int data)
{
			unsigned int tag = ((address >> offsetwidth) >> indexwidth) & (tagwidth-1);
			int i = 0;
			while (i <= (blkcount-1))
			{
				if ((tag == Blocks[i].tag) && (Blocks[i].valid == 1))
				{
					Blocks[i].writeWord(address,data);
					Blocks[i].dirty = 1;
					return &Blocks[i];
					break;
				}
				i++;
			}
			return NULL;
}

void Set::timestampUpdate()
{
		for (int i=0; i <= (blkcount-1); i++)
			Blocks[i].timestamp++;
}

Set::~Set()
{
		Blocks.clear();
}



