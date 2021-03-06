/*
 	Ray
    Copyright (C) 2010, 2011, 2012 Sébastien Boisvert

	http://DeNovoAssembler.SourceForge.Net/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You have received a copy of the GNU General Public License
    along with this program (gpl-3.0.txt).  
	see <http://www.gnu.org/licenses/>
*/

#include "GenomeToTaxonLoader.h"
#include "types.h"

#include <assert.h>
#include <iostream>
using namespace std;

void GenomeToTaxonLoader::load(string file){

	STEPPING=1000000;

	m_current=0;
	m_size=0;

	m_stream.open(file.c_str());

	if(!m_stream){
		cout<<"File "<<file<<" is invalid."<<endl;

		m_stream.close();

	}

	int count=0;

	while(!m_stream.eof()){
		string a="";

		m_stream>>a;

		if(a!=""){
			count++;

			if(count==2){
				count=0;
				m_size++;
			}
		}

	}

	m_stream.close();

	m_stream.open(file.c_str());

}

bool GenomeToTaxonLoader::hasNext(){
	return m_current<m_size;
}

void GenomeToTaxonLoader::getNext(GenomeIdentifier*genome, TaxonIdentifier*taxon){

	string tmpGenome;
	GenomeIdentifier loadedGenome=0;
	TaxonIdentifier loadedTaxon;

	m_stream>>tmpGenome>>loadedTaxon;

	//sdbm algorithm implementation http://www.cse.yorku.ca/~oz/hash.html
	for (string::const_iterator it = tmpGenome.begin();it!= tmpGenome.end();++it){
		loadedGenome = ((int) *it) + (loadedGenome << 6)  + (loadedGenome << 16) - loadedGenome ;
	}

	loadedGenome = loadedGenome - ((loadedGenome / COLOR_NAMESPACE_MULTIPLIER)*COLOR_NAMESPACE_MULTIPLIER);

	// cout << "genome " << loadedGenome << " linked to taxon " << loadedTaxon << endl;

	(*genome)=loadedGenome;
	(*taxon)=loadedTaxon;

	m_current++;

	if(m_current== m_size){
		m_stream.close();
	}

}

