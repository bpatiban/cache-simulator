#include "Block.h"

Block::Block(int block_size)
{
		dirty = tag = timestamp = valid = 0;
		wordcount = block_size/4;
		Words.reserve(wordcount);
}

int Block::getWord(unsigned int address)
{
		unsigned int offset = address & (wordcount-1);
		valid = 1;
		timestamp = -1;
		return Words[offset];
}

std::vector <int> Block::getWords(unsigned int address)
{
		valid = 1;
		timestamp = -1;
		return Words;
}

void Block::writeWord(unsigned int address, int data)
{
		unsigned int offset = address & (wordcount-1);
		Words[offset] = data;
		valid = 1;
		timestamp = -1;
}
	
void Block::writeWords(unsigned int address, std::vector <int> data)
{
		Words = data;
		valid = 1;
		timestamp = -1;
}
	
Block::~Block()
{
		Words.clear();
}

