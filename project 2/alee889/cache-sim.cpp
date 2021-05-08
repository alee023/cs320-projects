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
			int indexBits = log2( dBlocks[ i ]) ;
			unsigned long long tag = addr >> ( indexBits + 5 ) ; 
			unsigned long long index = ( addr / 32 ) % dBlocks[ i ] ;
			if( arrCaches[ i ][ index ] != tag ) {
				arrCaches[ i ][ index ] = tag ;
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
string sAssoc( int associativity ) {
	string line, type ;
	unsigned long long addr ;
	int numHits = 0 ;		
	int clock = 0 ;
 
	int numSets = ( 16 * 1024 )/( associativity * 32 ) ;
	int indexBits = log2( numSets ) ;
	int cache[ numSets ][ associativity ] ;
	int lru[ numSets ][ associativity ] ;

	for( int i = 0; i < numSets; i++ ) {
		for( int j = 0; j < associativity; j++ ) {
			cache[ i ][ j ] = 0 ;
			lru[ i ][ j ] = 0 ;
		}
	}
	
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;

		int index = ( addr / 32 ) % numSets ;
		unsigned long long tag = addr >> ( indexBits + 5 ) ;
		bool found = false ;
		int minIndex = 0 ;

		for( int i = 0; i < associativity; i++ ) {
			if( lru[ index ][ i ] < lru[ index ][ minIndex ]) {
				minIndex = i ;
			}
			if( cache[ index ][ i ] == tag ) {
				found = true ;
				numHits++ ;
				lru[ index ][ i ] = ++clock ;
			}
		}

		if( !found ) {
			lru[ index ][ minIndex ] = ++clock ;
			cache[ index ][ minIndex ] = tag ;
		}
	}

	infile.close() ;

	return( to_string( numHits )) ;
}

//====================== fully assoc ===============================
string fAssocLRU() {
	string line, type ;
	unsigned long long addr ;
	unsigned long long numHits = 0 ;		
	unsigned long long clock = 0 ;
	int cache[ 512 ] ;
	int lru[ 512 ] ;

	for( int i = 0; i < 512; i++ ) {
		cache[ i ] = 0 ;
		lru[ i ] = 0 ;
	}

	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;

		bool found = false ;
		int minIndex = 0 ;
		for( int i = 0; i < 512; i++ ) {
			if( lru[ i ] < lru[ minIndex ]) {
				minIndex = i ;
			}
			if( cache[ i ] == addr / 32 ) {
				found = true ;
				numHits++ ;
				lru[ i ] = ++clock ;
			}
		}

		if( !found ) {
			lru[ minIndex ] = ++clock ;
			cache[ minIndex ] = addr / 32 ;
		}
	}

	infile.close() ;

	return( to_string( numHits )) ;
}

//================set assoc no alloc ==================================
string noAlloc( int associativity ) {
	string line, type ;
	unsigned long long addr ;
	unsigned long long numHits = 0 ;		
	unsigned long long clock = 0 ;

	int numSets = ( 16 * 1024 )/( associativity * 32 ) ;
	int indexBits = log2( numSets ) ;
	int cache[ numSets ][ associativity ] ;
	int lru[ numSets ][ associativity ] ;

	for( int i = 0; i < numSets; i++ ) {
		for( int j = 0; j < associativity; j++ ) {
			cache[ i ][ j ] = 0 ;
			lru[ i ][ j ] = 0 ;
		}
	}
	
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;

		int index = ( addr / 32 ) % numSets ;
		unsigned long long tag = addr >> ( indexBits + 5 ) ;
		bool found = false ;
		int minIndex = 0 ;

		for( int i = 0; i < associativity; i++ ) {
			if( lru[ index ][ i ] < lru[ index ][ minIndex ]) {
				minIndex = i ;
			}
			if( cache[ index ][ i ] == tag ) {
				found = true ;
				numHits++ ;
				lru[ index ][ i ] = ++clock ;
			}
		}

		if( !found && type == "L" ) {
			lru[ index ][ minIndex ] = ++clock ;
			cache[ index ][ minIndex ] = tag ;
		}
	}

	infile.close() ;

	return( to_string( numHits )) ;
}

//=====================set assoc w next line prefetch====================
string sAssocNL( int associativity ) {
	string line, type ;
	unsigned long long addr ;
	unsigned long long numHits = 0 ;		
	unsigned long long clock = 0 ;

	int numSets = ( 16 * 1024 )/( associativity * 32 ) ;
	int indexBits = log2( numSets ) ;
	int cache[ numSets ][ associativity ] ;
	int lru[ numSets ][ associativity ] ;

	for( int i = 0; i < numSets; i++ ) {
		for( int j = 0; j < associativity; j++ ) {			
			cache[ i ][ j ] = 0 ;
			lru[ i ][ j ] = 0 ;
		}
	}
	
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;

		int index = ( addr / 32 ) % numSets ;
		unsigned long long tag = addr >> ( indexBits + 5 ) ;
		bool found = false ;
		int minIndex = 0 ;

		for( int i = 0; i < associativity; i++ ) {
			if( lru[ index ][ i ] < lru[ index ][ minIndex ]) {
				minIndex = i ;
			}
			if( cache[ index ][ i ] == tag ) {
				found = true ;
				numHits++ ;
				lru[ index ][ i ] = ++clock ;
			}
		}

		if( !found ) {
			lru[ index ][ minIndex ] = ++clock ;
			cache[ index ][ minIndex ] = tag ;
		}

		// PREFETCH
		found = false ;
		int NLIndex = ( 1 + addr / 32) % numSets ;
		tag = ( addr + 32 ) >> ( indexBits + 5 ) ;
		minIndex = 0 ;
		for( int i = 0; i < associativity; i++ ) {
			if( lru[ NLIndex ][ i ] < lru[ NLIndex ][ minIndex ]) {
				minIndex = i ;
			}
			if( cache[ NLIndex ][ i ] == tag ) {
				found = true ;
				lru[ NLIndex ][ i ] = ++clock ;
			}
		}

		if( !found ) {
			lru[ NLIndex ][ minIndex ] = ++clock ;
			cache[ NLIndex ][ minIndex ] = tag ;
		}
	}

	infile.close() ;

	return( to_string( numHits )) ;
}

//=====================set assoc prefetch on miss========================
string sAssocMiss( int associativity ) {
	string line, type ;
	unsigned long long addr ;
	unsigned long long numHits = 0 ;		
	unsigned long long clock = 0 ;

	int numSets = ( 16 * 1024 )/( associativity * 32 ) ;
	int indexBits = log2( numSets ) ;
	int cache[ numSets ][ associativity ] ;
	int lru[ numSets ][ associativity ] ;

	for( int i = 0; i < numSets; i++ ) {
		for( int j = 0; j < associativity; j++ ) {			
			cache[ i ][ j ] = 0 ;
			lru[ i ][ j ] = 0 ;
		}
	}
	
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;

		int index = ( addr / 32 ) % numSets ;
		unsigned long long tag = addr >> ( indexBits + 5 ) ;
		bool found = false ;
		int minIndex = 0 ;

		for( int i = 0; i < associativity; i++ ) {
			if( lru[ index ][ i ] < lru[ index ][ minIndex ]) {
				minIndex = i ;
			}
			if( cache[ index ][ i ] == tag ) {
				found = true ;
				numHits++ ;
				lru[ index ][ i ] = ++clock ;
				break ;
			}
		}

		if( !found ) {
			lru[ index ][ minIndex ] = ++clock ;
			cache[ index ][ minIndex ] = tag ;

			// PREFETCH
			found = false ;
			int NLIndex = ( 1 + addr / 32) % numSets ;
			tag = ( addr + 32 ) >> ( indexBits + 5 ) ;
			minIndex = 0 ;
			for( int i = 0; i < associativity; i++ ) {
				if( lru[ NLIndex ][ i ] < lru[ NLIndex ][ minIndex ]) {
					minIndex = i ;
				}
				if( cache[ NLIndex ][ i ] == tag ) {
					found = true ;
					lru[ NLIndex ][ i ] = ++clock ;
				}
			}

			if( !found ) {
				lru[ NLIndex ][ minIndex ] = ++clock ;
				cache[ NLIndex ][ minIndex ] = tag ;
			}
		}
	}

	infile.close() ;

	return( to_string( numHits )) ;
}

int main( int argc, char *argv[]) {
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	dMap() ;
	ofstream outfile( writeFile, fstream::app ) ;
	outfile << sAssoc( 2 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssoc( 4 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssoc( 8 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssoc( 16 ) + "," + to_string( numberLines ) + ";" << endl ;
	
	outfile << fAssocLRU() + "," + to_string( numberLines ) + ";" << endl ;
	outfile << to_string( 0 ) + "," + to_string( numberLines ) + ";" << endl ;

	outfile << noAlloc( 2 ) + "," + to_string( numberLines ) + "; " ;
	outfile << noAlloc( 4 ) + "," + to_string( numberLines ) + "; " ;
	outfile << noAlloc( 8 ) + "," + to_string( numberLines ) + "; " ;
	outfile << noAlloc( 16 ) + "," + to_string( numberLines ) + ";" << endl ;

	outfile << sAssocNL( 2 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssocNL( 4 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssocNL( 8 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssocNL( 16 ) + "," + to_string( numberLines ) + ";" << endl ;

	outfile << sAssocMiss( 2 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssocMiss( 4 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssocMiss( 8 ) + "," + to_string( numberLines ) + "; " ;
	outfile << sAssocMiss( 16 ) + "," + to_string( numberLines ) + ";" << endl ;

	outfile.close() ;
	return 0;
}