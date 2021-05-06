#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include <math.h>

using namespace std;
string readFile, writeFile ;
int numberLines = 0 ;


//======================== direct mapped =====================
void dMap() {
	string line, type ;
	unsigned long long addr ;
	int dBlocks[ 4 ] = { 32, 128, 512, 1024 } ;
	int arrCaches[ 4 ][ 1024 ] ;
	int dMapHits[ 4 ] = { 0, 0, 0, 0 } ;
	
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;
		
		numberLines++ ;

		for( int i = 0; i < 4; i++ ) {
			unsigned long long index = ( addr / 32 ) % dBlocks[ i ] ;
			if( arrCaches[ i ][ index ] != ( addr / 32 )) {
				arrCaches[ i ][ index ] = ( addr / 32 ) ;
			}
			else {
				dMapHits[ i ]++ ;
			}
		}
	}
	
	infile.close() ;
	ofstream outfile( writeFile ) ;
	for( int i = 0; i < 3; i++ ) {
		outfile << to_string( dMapHits[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << to_string( dMapHits[ 3 ]) + "," + to_string( numberLines ) + ";" << endl ;
	outfile.close() ;
}

//=========================set assoc ==================================
int sAssoc( int associativity ) {
	string line, type ;
	unsigned long long addr ;
	int numHits = 0 ;

	int numSets = ( 16 * 1024 )/( associativity * 32 ) ;
	int cache[ numSets ][ associativity ] ;
	int lru[ numSets ][ associativity ] ;

	for( int i = 0; i < numSets; i++ ) {
		for( int j = 0; j < associativity; j++ ) {
			lru[ i ][ j ] = 0 ;
		}
	}
	
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;

		int index = ( addr / 32 ) % numSets ;
		bool found = false ;
		int minIndex = 0 ;
		int timer = 0 ;

		for( int i = 0; i < associativity; i++ ) {
			if( cache[ index ][ i ] ==  addr / 32 ) {
				found = true ;
				numHits++ ;
				lru[ index ][ i ] = ++timer ;
				break ;
			}
		}

		if( !found ) {
			for( int i = 0; i < associativity; i++ ) {
				if( lru[ index ][ i ] < lru[ index ][ minIndex ]) {
					minIndex = i ;
				}
			}
			lru[ index ][ minIndex ] = ++timer ;
			cache[ index ][ minIndex ] = addr / 32 ;
		}
	}

	infile.close() ;

	return( numHits ) ;
}

int main( int argc, char *argv[]) {
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	dMap() ;
	ofstream outfile( writeFile, fstream::app ) ;
	outfile << to_string( sAssoc( 2 )) + "," + to_string( numberLines ) + "; " ;
	outfile << to_string( sAssoc( 4 )) + "," + to_string( numberLines ) + "; " ;
	outfile << to_string( sAssoc( 8 )) + "," + to_string( numberLines ) + "; " ;
	outfile << to_string( sAssoc( 16 )) + "," + to_string( numberLines ) + "; " ;

	return 0;
}