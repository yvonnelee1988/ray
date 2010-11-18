/*
 	Ray
    Copyright (C) 2010  Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (LICENSE).  
	see <http://www.gnu.org/licenses/>


 	Funding:

Sébastien Boisvert has a scholarship from the Canadian Institutes of Health Research (Master's award: 200910MDR-215249-172830 and Doctoral award: 200902CGM-204212-172830).

*/

#ifndef _OutboxAllocator
#define _OutboxAllocator

#include<set>
#include<stdint.h>
using namespace std;

/*
 * this is an allocator that can allocate up to <m_chunks> allocations of exactly <m_max> bytes.
 * allocation and free are done both in constant time (yeah!)
 */
class OutboxAllocator{
	int m_chunks;
	int m_max;
	uint8_t*m_memory;
	uint16_t*m_availableChunks;
	int m_numberOfAvailableChunks;
	int m_numberOfBytes;
public:
	OutboxAllocator();
	void constructor(int chunks,int size);
	void*allocate(int a);
	void free(void*a);
};


#endif

