#include <stdio.h>
#include <string>
#include <vector>
#include <cassert>
#include <list>
#include <iterator>

typedef std::vector<std::string> vs_t;

// split string p_line into a vector of strings (words)
// the delimiters are 1 or more whitespaces
vs_t split(const std::string &p_line)
{
	auto line = p_line + " ";
	vs_t res;
	bool in_str = false;
	std::string curr_word = "";
	for (auto c : line)
	{
		if (isspace(c))
		{
			if (in_str)
				res.push_back(curr_word);
			in_str = false;
			curr_word = "";
		}
		else
		{
			curr_word.push_back(c);
			in_str = true;
		}
	}
	
	return res;
}

// convert string to long
// if successful, success = True, otherwise success = False
long str2long(const std::string &s, bool &success)
{
  	char * end = 0;
  	errno = 0;
  	long res = strtol(s.c_str(), &end, 10);
  	if (*end != 0 || errno != 0)
	{
    	success = false;
    	return -1;
  	}
  	
	success = true;
  	return res;
}

// read in a line from stdin and parse it
// parsed results are passed back via parameter references
// returns true on EOF, otherwise false
// aborts program on bad input
bool parse_line(long &n1, long &n2)
{
	char buff[4096];
	vs_t words;
	static long line_no = 0;

	// read in a line and parse it into words
	// loop to skip empty lines
	while (1)
	{ 
		line_no ++;
		if (!fgets( buff, sizeof(buff), stdin))
			return true; // eof
		words = split(buff);
		if (words.size() != 0) break; // continue if line not empty
	}

	if (words.size() > 2)
	{
		printf("Error on line %ld - too many words\n", line_no);
		exit(-1);
  	}

	// convert first word into number
	bool ok;
	n1 = str2long(words[0], ok);
	if (!ok)
	{
		printf("Error on line %ld - not an integer\n", line_no);
		exit(-1);
	}

	if (n1 < 0)
	{
		if (words.size() != 1)
		{
			printf("Error on line %ld - too many numbers\n", line_no);
			exit(-1);
		}
	}
	else
	{
		// convert 2nd number
		n2 = str2long(words[1].c_str(), ok);
		if (!ok || n2 < 1)
		{
			printf("Error on line %ld - 2nd paramter not positive integer\n", line_no);
			exit(-1);
		}
	}
	
	return false;
};

// tag = -1 is a free chunk
struct Chunk
{
	long tag;
	long size;

	Chunk(long t, long s)
	{
		tag = t;
		size = s;
	}
};

// all you need to do is to fill in the implementation of MemSim class
struct MemSim
{
  	// initializes the memory simulator with a page size
  	MemSim(long page_size)
	{
    	pageSize = page_size;
		totalPages = 0;
  	}
  
	// frees all blocks with the given tag and merges adjecent free blocks
	void free(long tag)
	{
		std::list<Chunk>::iterator it;
		for (it = memList.begin(); it != memList.end(); it++)
		{
			if (it->tag == tag)
			{
				long mergeSize = 0;
				it->tag = -1;

				std::list<Chunk>::iterator prev;
				prev = it;
				if (it != memList.begin())
				{
					prev--;
					if (prev->tag == -1)
					{
						mergeSize += prev->size;
						memList.erase(prev);
					}
				}

				std::list<Chunk>::iterator next;
				next = it;
				next++;
				if (next != memList.end() && next->tag == -1)
				{
					mergeSize += next->size;
					memList.erase(next);
				}

				it->size = it->size + mergeSize;
			}
		}
	}
  
	// allocates a chunk using best-fit
	// if no suitable chunk found, requests new page(s)
	void alloc(long tag, long size)
	{
		std::list<Chunk>::iterator it;
		std::list<Chunk>::iterator bestChunk;
		long bestSize = 100000000;
		bool foundChunk = false;

		for (it = memList.begin(); it != memList.end(); it++)
		{
			// check to see if it's a free chunk
			if (it->tag == -1)
			{
				// size of free chunk is the same as requested size
				// allocate requested chunk and return right away
				if (it->size == size)
				{
					it->tag = tag;
					return;
				}
				// find smallest suitable free chunk that can hold requested chunk
				else if (it->size > size && it->size < bestSize)
				{
					bestChunk = it;
					bestSize = it->size;
					foundChunk = true;	
				}
			}
		}

		// found suitable chunk
		// allocate requested chunk and split off the remaining free chunk
		if (foundChunk)
		{
			bestChunk->size = bestChunk->size - size;
			memList.emplace(bestChunk, Chunk(tag, size));
		}
		// request for new page(s) and insert requested chunk
		else
		{
			long pagesRequired = 0;
			std::list<Chunk>::iterator end = memList.end();
			end--;

				if (end->tag == -1)
				{
					long sizeRequired = size - end->size;
					if (sizeRequired % pageSize == 0)
					{
						pagesRequired = sizeRequired / pageSize;
						end->tag = tag;
						end->size = size;
					}
					else	
					{
						pagesRequired = sizeRequired / pageSize + 1;
						end->size = pageSize * pagesRequired - sizeRequired;
						memList.emplace(end, Chunk(tag, size));
					}				
				}
				// no free chunk at the end of list
				else
				{
					if (size % pageSize == 0)
					{
						pagesRequired = size / pageSize;
						memList.emplace_back(Chunk(tag, size));
					}
					else	
					{
						pagesRequired = size / pageSize + 1;
						memList.emplace_back(Chunk(tag, size));
						memList.emplace_back(Chunk(-1, pageSize * pagesRequired - size));
					}
				}			
			
			totalPages += pagesRequired;
		}
	}

	// returns statistics about the simulation
	// - number of total page requests
	// - the largest available chunk
	void get_report()
	{
		long largest = 0;

		for (const auto &c : memList)
		{
			if (c.tag == -1 && c.size > largest)
				largest = c.size;
		}

		printf("pages requested: %ld\n", totalPages);
		printf("largest free chunk: %ld\n", largest);

		// debug
		// for (auto &i : memList)
		// {
		// 	printf("%ld: %ld\n", i.tag, i.size);
		// }
	}

	private:
		long pageSize;
		long totalPages;
		std::list<Chunk> memList;
};

int main(int argc, char **argv)
{
  	// parse command line arguments
  	// ------------------------------
  	long page_size = 1000;
  	if (argc != 2)
	{
    	printf("Assuming page size = %ld.\n", page_size);
  	}
  	else
	{
    	bool ok;
    	page_size = str2long(argv[1], ok);
    	if (!ok || page_size < 1)
		{
      		printf("Bad page size '%s'.\n", argv[1]);
      		exit(-1);
    	}
  	}

	// process every line
	// ------------------------------
  	MemSim ms(page_size);
	while (true)
	{
    	long n1, n2;
    	if (parse_line(n1, n2)) break;
    	if (n1 < 0)
      		ms.free(-n1);
    	else
      		ms.alloc(n1, n2);
  	}

	// report results
	// ------------------------------
	ms.get_report();

  	return 0;
}
