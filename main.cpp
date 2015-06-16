#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <fstream>
#include "Block.h"
#include "Set.h"
#include "Cache.h"

using namespace std;

unsigned int atoh(string tempAddress)
{
	unsigned int address;
	std::stringstream ss;
	ss << std::hex << tempAddress;
	ss >> address;
	return address;
}

int main(int argc, char * argv[])
{
	// Command line: ./<executable> <configuration_file> <trace_file> <output_file>
	ifstream inputfile1 (argv[1]);
	ifstream inputfile2 (argv[2]);
	ofstream outputfile (argv[3]);

	cout << "Working...." << endl;

	vector <string> a (5);
	unsigned int cache_size, assoc, block_size, write_policy, victim_blockcount;

	if (inputfile1.is_open())	/////////////////////////////////////////// Parse Configuration File
	{
		int i = 0;
		string discard;
		while ( inputfile1.good() )
		{
			inputfile1 >> discard;
			inputfile1 >> a[i++];

			if (i == 5)
				break;
		}
		inputfile1.close();
	}

	cache_size = atoi(a[0].c_str());
	block_size = atoi(a[2].c_str());
	write_policy = atoi(a[3].c_str());
	victim_blockcount = atoi(a[4].c_str());
	if (a[1] == "Full")
		assoc = cache_size/block_size;
	else
		assoc = atoi(a[1].c_str());

	Cache DataC (cache_size, assoc, block_size);
	Cache VictimC (block_size*victim_blockcount, victim_blockcount, block_size);

	string tempAccess, tempData, tempAddress;
	int data;
	unsigned int address;
	int MAXREF;
	int L1reads, Vreads, L1writes, Vwrites;
	int L1readHit, L1readMiss, L1writeHit, L1writeMiss;
	int VreadHit, VreadMiss, VwriteHit, VwriteMiss;
	int fromMem, toMem;
	float L1missRate, VmissRate;

	if (inputfile2.is_open())	/////////////////////////////////////////////////// Parse Trace File
	{
		MAXREF = 0;

		L1readHit = L1readMiss = L1writeHit = L1writeMiss =  L1reads = L1writes =
				VreadHit = VreadMiss = VwriteHit = VwriteMiss = Vreads = Vwrites =
						fromMem = toMem = 0;

		while ( inputfile2.good() )
		{
			MAXREF++;
			tempAccess = tempData = tempAddress = "";
			inputfile2 >> tempAccess;
			inputfile2 >> tempAddress;
			inputfile2 >> tempData;
			address = atoh(tempAddress);
			data = atoi(tempData.c_str());

			if ((tempAccess != "") && (tempAddress != ""))
			{
				if (tempAccess == "l") /////////////////////////////////////////// Data Read
				{
					if (DataC.readBlock(address) != NULL) 			// L1 Hit
					{
						L1readHit++;
					}
					else if (VictimC.readBlock(address) != NULL)	// L1 Miss Victim Hit
					{
						L1readMiss++;
						VreadHit++;
						Block * temp = DataC.writeBlock(address,(*VictimC.readBlock(address)).getWords(address));
						(*VictimC.readBlock(address)).writeWords(address,(*temp).getWords(address));
					}
					else											// L1 Miss Victim Miss
					{
						L1readMiss++;
						VreadMiss++;
						vector <int> dummy (block_size/4);
						dummy[(address & ((block_size/4)-1))] = data;
						//From Memory - 1 block
						fromMem++;
						Block * temp = DataC.writeBlock(address,dummy);
						Block * temp1 = VictimC.writeBlock(address,(*temp).getWords(address));
						//for write back, dirty = 1, To Memory - 1 block
						//for write through, valid = 1, To Memory - 1 block
						if ((*temp1).dirty == 1)
						{
							toMem++;
						}
						else if (write_policy == 2)
						{
							if ((*temp1).valid == 1)
							{
								toMem++;
							}
						}
					}
				}
				else if (tempAccess == "s") //////////////////////////////////////////// Data Write
				{
					if (write_policy == 1)			///////////////// Write Back with Write Allocate
					{
						if (DataC.writebackBlock(address,data) != NULL)			// L1 Hit
						{
							L1writeHit++;
						}
						else if (VictimC.writebackBlock(address,data) != NULL)	// L1 Miss Victim Hit
						{
							L1writeMiss++;
							VwriteHit++;
							Block * temp = DataC.writeBlock(address,(*VictimC.readBlock(address)).getWords(address));
							(*VictimC.readBlock(address)).writeWords(address,(*temp).getWords(address));
						}
						else													// L1 Miss Victim Miss
						{
							L1writeMiss++;
							VwriteMiss++;
							vector <int> dummy (block_size/4);
							dummy[(address & ((block_size/4)-1))] = data;
							// To Memory - 1 block
							toMem++;
							Block * temp = DataC.writeBlock(address,dummy);
							Block * temp1 = VictimC.writeBlock(address,(*temp).getWords(address));
							//for write back, dirty = 1, To Memory - 1 block
							//for write through, valid = 1, To Memory - 1 block
							if ((*temp1).dirty == 1)
							{
								toMem++;
							}
							else if (write_policy == 2)
							{
								if ((*temp1).valid == 1)
								{
									toMem++;
								}
							}
						}
					}
					else if (write_policy == 2) 	/////////// Write Through with No Write Allocate
					{
						if (DataC.writethroughBlock(address,data) != NULL)			// L1 Hit
						{
							L1writeHit++;
							//To Memory - 1 block
							toMem++;
						}
						else if (VictimC.writethroughBlock(address,data) != NULL)	// L1 Miss Victim Hit
						{
							L1writeMiss++;
							VwriteHit++;
						}
						else														// L1 Miss Victim Miss
						{
							L1writeMiss++;
							VwriteMiss++;
							// To Memory - 1 block
							toMem++;
						}
					}
					else
						cout << "Error! Wrong Format!!";
				}
				else
				{
					cout << "Error! Wrong Format";
					return 0;
				}
			}
			else
				MAXREF--;
			DataC.timestampUpdate();
			VictimC.timestampUpdate();
		}
		inputfile2.close();
	}
	cout << "..Done" << endl;

	L1reads = L1readHit + L1readMiss;
	L1writes = L1writeHit + L1writeMiss;
	Vreads = VreadHit + VreadMiss;
	Vwrites = VwriteHit + VwriteMiss;
	L1missRate = (float) (L1readMiss + L1writeMiss) / (L1reads + L1writes);
	VmissRate =  (float) (VreadMiss + VwriteMiss) / (Vreads + Vwrites);

	cout << "Total Data Accesses:\t" << MAXREF << "\n" <<
			"------------------L1 Cache------------------" << "\n" <<
			"-----L1 Reads:\t" << L1reads << "\n" <<
			"Hits:\t\t" << L1readHit << "\n" <<
			"Misses:\t\t" << L1readMiss << "\n" <<
			"-----L1 Writes:\t" << L1writes << "\n" <<
			"Hits:\t\t" << L1writeHit << "\n" <<
			"Misses:\t\t" << L1writeMiss << "\n" <<
			"L1 Miss Rate:\t\t" << L1missRate << "\n" <<
			"----------------Victim Cache---------------" << "\n" <<
			"Victim Reads:\t" << Vreads << "\n" <<
			"Hits:\t\t" << VreadHit << "\n" <<
			"Misses:\t\t" << VreadMiss << "\n" <<
			"Victim Writes:\t" << Vwrites << "\n" <<
			"Hits:\t\t" << VwriteHit << "\n" <<
			"Misses:\t\t" << VwriteMiss << "\n" <<
			"Victim Miss Rate:\t" << VmissRate << "\n" <<
			"--------------Memory Traffic--------------" << "\n" <<
			"(Memory->Cache):\t" << fromMem*block_size << " bytes" << "\n" <<
			"(Cache-> Memory):\t" << toMem*block_size << " bytes" << endl;

	string policy;
	if (write_policy == 1)
		policy = "Write Back with Write Allocate";
	else
		policy = "Write Through with No Write Allocate";


	if (outputfile.is_open())
	{
		outputfile << "****INPUT DATA****" << "\n\n" <<
				"DataCacheSize(Bytes): " << cache_size << "\n" <<
				"Associativity: "<< assoc << "\n" <<
				"BlockSize(Bytes): " << block_size << "\n" <<
				"WritePolicy: " << policy << "\n" <<
				"VictimCacheSize(Blocks): " << victim_blockcount << "\n" <<
				"Trace File: " << argv[2] << endl << endl;

		outputfile << "****OUTPUT DATA****" << "\n\n" <<
				"Total Data Accesses:\t" << MAXREF << "\n" <<
				"------------------L1 Cache------------------" << "\n" <<
				"-----L1 Reads:\t" << L1reads << "\n" <<
				"Hits:\t\t" << L1readHit << "\n" <<
				"Misses:\t\t" << L1readMiss << "\n" <<
				"-----L1 Writes:\t" << L1writes << "\n" <<
				"Hits:\t\t" << L1writeHit << "\n" <<
				"Misses:\t\t" << L1writeMiss << "\n" <<
				"L1 Miss Rate:\t\t" << L1missRate << "\n" <<
				"----------------Victim Cache---------------" << "\n" <<
				"Victim Reads:\t" << Vreads << "\n" <<
				"Hits:\t\t" << VreadHit << "\n" <<
				"Misses:\t\t" << VreadMiss << "\n" <<
				"Victim Writes:\t" << Vwrites << "\n" <<
				"Hits:\t\t" << VwriteHit << "\n" <<
				"Misses:\t\t" << VwriteMiss << "\n" <<
				"Victim Miss Rate:\t" << VmissRate << "\n" <<
				"--------------Memory Traffic--------------" << "\n" <<
				"(Memory->Cache):\t" << fromMem*block_size << " bytes" << "\n" <<
				"(Cache-> Memory):\t" << toMem*block_size << " bytes" << endl;
		outputfile.close();
	}

	return 0;
}
