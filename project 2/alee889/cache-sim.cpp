#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;
string readFile, writeFile ;
int numberLines = 0 ;

void dMap() {
	string line, type ;
	unsigned long long addr ;
	int dBlocks[ 4 ] = { 32, 128, 512, 1024 } ;
	int arrCaches[ 4 ][ 1024 ] ;
	int associativities[ 4 ] = { 2, 4, 8, 16 } ;
	int sASets[ 4 ] = { 256, 128, 64, 32 } ;
	int dMapHits[ 4 ] = { 0, 0, 0, 0 } ;
	int sAHits[ 4 ] = { 0, 0, 0, 0 } ;

	vector<vector<int>> twoAssoc( 256 ) ;
	vector<vector<int>> fourAssoc( 128 ) ;
	vector<vector<int>> eightAssoc( 64 ) ;
	vector<vector<int>> sixteenAssoc( 32 ) ;
	vector<vector<vector<int>>*> sACaches{ &twoAssoc, &fourAssoc, &eightAssoc, &sixteenAssoc } ;
	vector<vector<int>> twoALRU( 256 );
	vector<vector<int>> fourALRU( 128 ) ;
	vector<vector<int>> eightALRU( 64 ) ;
	vector<vector<int>> sixteenALRU( 32 ) ;
	vector<vector<vector<int>>*> sALRUs{ &twoALRU, &fourALRU, &eightALRU, &sixteenALRU } ;
	
	for( int i = 0; i < 4; i++ ) {
		vector<vector<int>>* x = sACaches[ i ] ;
		vector<vector<int>>* y = sALRUs[ i ] ;
		/*
		for( int j = 0; j < sASets[ i ]; j++ ) {
			for( int k = 0; k < associativities[ i ]; k++ ) {
				(*x)[ j ][ k ] = 0 ;
				(*y)[ j ][ k ] = 0 ;
			}
		}
*/
	}

	printf( "" + fourAssoc[ 125 ][ 2 ] ) ;
	
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> type >> hex >> addr ;
		
		numberLines++ ;

		//======================== direct mapped =====================
		for( int i = 0; i < 4; i++ ) {
			unsigned long long index = ( addr / 32 ) % dBlocks[ i ] ;
			if( arrCaches[ i ][ index ] != ( addr / 32 )) {
				arrCaches[ i ][ index ] = ( addr / 32 ) ;
			}
			else {
				dMapHits[ i ]++ ;
			}
		}
		//============================================================
/*
		//=====================set associative========================
		for( int i = 0; i < 4; i++ ) {
			unsigned long long index = ( addr / 32 ) & sASets[ i ] ;
			bool hit = false ;
			int minLRU = 0;
			vector<vector<int>>* x = sACaches[ i ] ;
			vector<vector<int>>* y = sALRUs[ i ] ;

			for( int j = 0; j < associativities[ i ]; j++ ) {
				if((*y)[ index ][ j ] < (*y)[ index ][ minLRU ]) {
					minLRU = j ;
				}
				if((*x)[ index ][ j ] == addr / 32 ) {
					sAHits[ i ]++ ;
					(*y)[ index ][ j ]++ ;
					hit = true ;
					break ;
				}
			}

			if( !hit ) {
				(*y)[ index ][ minLRU ]++ ;
				(*x)[ index ][ minLRU ] = addr / 32 ;
			}
		}
		//============================================================
*/
	}
	
	infile.close() ;
	ofstream outfile( writeFile ) ;
	for( int i = 0; i < 3; i++ ) {
		outfile << to_string( dMapHits[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << to_string( dMapHits[ 3 ]) + "," + to_string( numberLines ) + ";" << endl ;
	for( int i = 0; i < 3; i++ ) {
		outfile << to_string( sAHits[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << to_string( sAHits[ 3 ]) + "," + to_string( numberLines ) + ";" << endl ;
	outfile.close() ;
}

int main( int argc, char *argv[]) {
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	dMap() ;

	return 0;
}