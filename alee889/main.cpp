#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include<bitset>
#include<vector>

using namespace std;
string readFile, writeFile ;
int numberLines = 0 ;
map<string, int> one4, one5, one7, one8, one9, one10, one11 ;
vector< map<string, int>*> oneMaps{ &one4, &one5, &one7, &one8, &one9, &one10, &one11 } ;

void AT() { // always taken
	string behavior, line, ignore ;
	int TCounter = 0 ;
	int NTCounter = 0 ;
		
	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		numberLines++ ;
		stringstream s( line ) ;
		s >> ignore >> behavior >> ignore ;

		if( behavior == "T" ) {
			TCounter++ ;
			// printf( "%d", TCounter ) ;
		}
		else if( behavior == "NT" ) {
			NTCounter++ ;
			// printf( "%d", NTCounter ) ;

		}
	}

	infile.close() ;
	ofstream outfile( writeFile ) ;
	outfile << to_string( TCounter ) + "," + to_string( numberLines ) + ";" << endl ;
	outfile << to_string( NTCounter ) + "," + to_string( numberLines ) + ";" << endl ;
	outfile.close() ;
}

void bimodal() {
	string behavior, line, ignore, binStr ;
	unsigned long long addr;
	int correctCounters [7] = { 0, 0, 0, 0, 0, 0, 0 } ;
	int divisors[ 7 ] = { 16, 32, 128, 256, 512, 1024, 2048 } ;

	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> hex >> addr >> behavior >> ignore ;
		bitset<11> bin11( addr ) ;

		for( int i = 0; i < 7; i++ ) {
			bitset<11> binSpecific( addr % correctCounters[ i ]) ;
			binStr = binSpecific.to_string() ;
			map<string, int> x = *(oneMaps[ i ]) ;
			if( !(x.count( binStr ))) {
				x[ binStr ] = 1 ;
			}

			if( behavior == "T" ) {
				if( x[ binStr ] == 0 ) {
					x[ binStr ] = 1 ;
				}
				else correctCounters[ i ]++ ;
			}
			else {
				if( x[ binStr ] == 0 ) {
					correctCounters[ i ]++ ;
				}
				else x[ binStr ] = 1 ;
			}
		}
	}

	infile.close() ;
	ofstream outfile( writeFile, fstream::app ) ;

	for( int i = 0; i < 7 ; i++ ) {
		outfile << to_string( correctCounters[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << endl ;
	outfile.close() ;
}

int main( int argc, char *argv[]) {
	unsigned long long addr;
	string behavior, line;
	unsigned long long target;
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	AT() ;
	bimodal() ;

	/*
	infile.open( "test_input.txt" );
	outfile.open( "output.txt" ) ;

	while( getline( infile, line )) {
		stringstream s( line );
		s >> std::hex >> addr >> behavior >> std::hex >> target;

		if(behavior == "T") {
			outfile << " -> taken, ";
		} 
		else {
			outfile << " -> not taken, ";
		}
		outfile << "target=" << target << endl;
	}

	infile.close() ;
	outfile.close() ;
	*/
	return 0;
}