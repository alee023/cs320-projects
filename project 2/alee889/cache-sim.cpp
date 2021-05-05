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

	vector<vector<int>> twoAssoc( 256 ) ;
	vector<vector<int>> fourAssoc( 128 ) ;
	vector<vector<int>> eightAssoc( 64 ) ;
	vector<vector<int>> sixteenAssoc( 32 ) ;
	vector<vector<vector<int>>*> sACaches{ &twoAssoc, &fourAssoc, &eightAssoc, &sixteenAssoc } ;
	for( int i = 0; i < 4; i++ ) {
		vector<vector<int>>* x = sACaches[ i ] ;
		for( int j = 0; j < sASets[ i ]; j++ ) {
			for( int k = 0; k < associativities[ i ]; k++ ) {
				(*x)[ sASets[ i ]][ k ] = 0 ;
			}
		}
	}
	
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
		outfile << to_string( dMapHits[ 3 ]) + "," + to_string( numberLines ) + ";" << endl ;
	}
	outfile.close() ;
}

int main( int argc, char *argv[]) {
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	dMap() ;

	return 0;
}