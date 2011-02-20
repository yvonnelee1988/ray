/*
 	Ray
    Copyright (C) 2011  Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (COPYING).  
	see <http://www.gnu.org/licenses/>

*/

#include <assert.h>
#include <GridTable.h>
#include <common_functions.h>
#include <crypto.h>
#include <stdlib.h>
#include <stdio.h>

void GridTable::constructor(int rank){
	m_size=0;
	m_inserted=false;
	m_gridSize=4194304;
	int bytes1=m_gridSize*sizeof(Vertex*);
	m_gridData=(Vertex**)__Malloc(bytes1);
	int bytes2=m_gridSize*sizeof(uint16_t);
	m_gridSizes=(uint16_t*)__Malloc(bytes2);
	//m_gridReservedSizes=(uint16_t*)__Malloc(bytes3);
	printf("Rank %i: allocating %i bytes for grid table\n",rank,bytes1+bytes2);
	fflush(stdout);
	showMemoryUsage(rank);
	for(int i=0;i<m_gridSize;i++){
		m_gridSizes[i]=0;
		m_gridData[i]=NULL;
		//m_gridReservedSizes[i]=0;
	}
	m_gridAllocator.constructor(m_gridSize);
	m_rank=rank;
}

uint64_t GridTable::size(){
	return m_size;
}

Vertex*GridTable::find(uint64_t key){
	uint64_t lowerKey;
	int bin=hash_function_2(key,m_wordSize,&lowerKey)%m_gridSize;

	if(key<lowerKey){
		lowerKey=key;
	}
	for(int i=0;i<m_gridSizes[bin];i++){
		Vertex*gridEntry=m_gridData[bin]+i;
		if(gridEntry->m_lowerKey==lowerKey){
			return move(bin,i);
		}
	}
	return NULL;
}

Vertex*GridTable::insert(uint64_t key){
	uint64_t lowerKey;
	m_inserted=false;
	int bin=hash_function_2(key,m_wordSize,&lowerKey)%m_gridSize;
	if(key<lowerKey){
		lowerKey=key;
	}
	//cout<<key<<" bin="<<bin<<" size="<<m_gridSizes[bin]<<endl;
	for(int i=0;i<m_gridSizes[bin];i++){
		Vertex*gridEntry=m_gridData[bin]+i;
		if(gridEntry->m_lowerKey==lowerKey){
			return move(bin,i);
		}
	}
	//if(m_gridReservedSizes[bin]==m_gridSizes[bin]){
	if(true){
		Vertex*newEntries=(Vertex*)m_gridAllocator.allocate((m_gridSizes[bin]+1)*sizeof(Vertex));
		for(int i=0;i<m_gridSizes[bin];i++){
			newEntries[i]=m_gridData[bin][i];
		}
		if(m_gridSizes[bin]!=0){
			m_gridAllocator.getStore()->addAddressToReuse(m_gridData[bin],m_gridSizes[bin]*sizeof(Vertex));
		}
		m_gridData[bin]=newEntries;
	}

	m_gridData[bin][m_gridSizes[bin]].m_lowerKey=lowerKey;
	int oldSize=m_gridSizes[bin];
	m_gridSizes[bin]++;
	//m_gridReservedSizes[bin]++;
	// check overflow
	assert(m_gridSizes[bin]>oldSize);
	m_inserted=true;
	m_size+=2;
	return move(bin,m_gridSizes[bin]-1);
}

bool GridTable::inserted(){
	return m_inserted;
}

void GridTable::remove(uint64_t a){

}

Vertex*GridTable::getElementInBin(int bin,int element){
	#ifdef ASSERT
	assert(bin<getNumberOfBins());
	assert(element<getNumberOfElementsInBin(bin));
	#endif
	return m_gridData[bin]+element;
}

int GridTable::getNumberOfElementsInBin(int bin){
	#ifdef ASSERT
	assert(bin<getNumberOfBins());
	#endif
	return m_gridSizes[bin];
}

int GridTable::getNumberOfBins(){
	return m_gridSize;
}

MyAllocator*GridTable::getAllocator(){
	return &m_gridAllocator;
}

MyAllocator*GridTable::getSecondAllocator(){
	return m_vertexTable.getAllocator();
}

void GridTable::freeze(){
	m_frozen=true;
}

void GridTable::unfreeze(){
	m_frozen=false;
}

bool GridTable::frozen(){
	return m_frozen;
}

/*
 *         0 1 2 3 4 5 6 7 
 *  input: a b c d e f g h
 *
 *  move(4)  // e
 *
 * 	    0 1 2 3 4 5 6 7
 *  output: e a b c d f g h
 *
 *
 */
Vertex*GridTable::move(int bin,int item){
	if(m_frozen){
		return m_gridData[bin]+item;
	}
	Vertex tmp;
	#ifdef ASSERT
	assert(item<getNumberOfElementsInBin(bin));
	#endif
	tmp=m_gridData[bin][item];
	for(int i=item-1;i>=0;i--){
		m_gridData[bin][i+1]=m_gridData[bin][i];
	}
	m_gridData[bin][0]=tmp;
	return m_gridData[bin];
}

void GridTable::setWordSize(int w){
	m_wordSize=w;
	m_vertexTable.setWordSize(w);
}

void GridTable::addRead(uint64_t a,ReadAnnotation*e){
	m_vertexTable.addRead(a,e);
}

ReadAnnotation*GridTable::getReads(uint64_t a){
	return m_vertexTable.getReads(a);
}

void GridTable::addDirection(uint64_t a,Direction*d){
	m_vertexTable.addDirection(a,d);
}

bool GridTable::isAssembled(uint64_t a){
	uint64_t reverse=complementVertex_normal(a,m_wordSize);
	return getDirections(a).size()>0||getDirections(reverse).size()>0;
}

vector<Direction> GridTable::getDirections(uint64_t a){
	return m_vertexTable.getDirections(a);
}

void GridTable::clearDirections(uint64_t a){
	m_vertexTable.clearDirections(a);
}

void GridTable::buildData(){
	m_vertexTable.constructor(m_rank);
}