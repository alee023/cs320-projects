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
	int[ 4 ] dBlocks = { 32, 128, 512, 1024 } ;
	int arrCaches[ 4 ][ 1024 ] ;
	int dMapHits = 0 ;
	
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
				dMapHits++ ;
			}
		}
		
	}
	
	infile.close() ;
	ofstream outfile( writeFile ) ;
	outfile << to_string( dMapHits ) + "," + to_string( numberLines ) + ";" << endl ;
	outfile.close() ;
}

int main( int argc, char *argv[]) {
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	dMap() ;

	return 0;
}