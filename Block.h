#ifndef BLOCK_H_
#define BLOCK_H_
#include <vector>
#include <math.h>
#include <cstddef>
class Block
{
private:
	int wordcount;
	std::vector <int> Words;
public:
	int timestamp;
	unsigned int tag;
	int valid;
	int dirty;
	Block(int block_size);
	int getWord(unsigned int address);
	std::vector <int> getWords(unsigned int address);
	void writeWord(unsigned int address, int data);
	void writeWords(unsigned int address, std::vector <int> data);
	~Block();
};

#endif
