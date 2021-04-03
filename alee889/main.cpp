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
map<string, int> two4, two5, two7, two8, two9, two10, two11 ;
vector< map<string, int>*> oneMaps{ &one4, &one5, &one7, &one8, &one9, &one10, &one11 } ;
vector< map<string, int>*> twoMaps{ &two4, &two5, &two7, &two8, &two9, &two10, &two11 } ;

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
	int correct1Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ;
	int correct2Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ;
	int divisors[ 7 ] = { 16, 32, 128, 256, 512, 1024, 2048 } ;

	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> hex >> addr >> behavior >> ignore ;
		bitset<11> bin11( addr ) ;

		for( int i = 0; i < 7; i++ ) {
			map<string, int>* x = oneMaps[ i ] ;
			map<string, int>* y = twoMaps[ i ] ;
			// cout << to_string( i + 1 ) + ": " << endl ;
			bitset<11> binSpecific( addr % divisors[ i ]) ;
			binStr = binSpecific.to_string() ;
			// cout << binStr << endl ;

			if( !(x->count( binStr ))) {
				(*x)[ binStr ] = 1 ;
				(*y)[ binStr ] = 11 ;
			}
	
			// cout << to_string((*x)[ binStr ]) << endl ;

			if( behavior == "T" ) {
				if((*x)[ binStr ] == 0 ) {
					(*x)[ binStr ] = 1 ;
				}
				else correct1Counters[ i ]++ ;

				if((*y)[ binStr ] == 0 ) { // 00-SNT
					(*y)[ binStr ] = 1 ;
				}
				else if((*y)[ binStr ] == 1 ) { // 01-NT
					(*y)[ binStr ] = 10 ;
				}
				else if((*y)[ binStr ] == 10 ) { // 10-T
					(*y)[ binStr ] = 11 ;
					correct2Counters[ i ]++ ;
				}
				else if((*y)[ binStr ] == 11 ) { // 11-ST
					correct2Counters[ i ]++ ;
				}
			}
			else { // behavior is NT
				if( (*x)[ binStr ] == 0 ) {
					correct1Counters[ i ]++ ;
				}
				else (*x)[ binStr ] = 0 ;

				if((*y)[ binStr ] == 0 ) { // 00-SNT
					correct2Counters[ i ]++ ;
				}
				else if((*y)[ binStr ] == 1 ) { // 01-NT
					(*y)[ binStr ] = 0 ;
					correct2Counters[ i ]++ ;
				}
				else if((*y)[ binStr ] == 10 ) { // 10-T
					(*y)[ binStr ] = 1 ;
				}
				else if((*y)[ binStr ] == 11 ) { // 11-ST
					(*y)[ binStr ] = 10 ;
				}
			}
		}
		// cout << "--------------------------------" << endl ;
	}

	infile.close() ;
	ofstream outfile( writeFile, fstream::app ) ;

	for( int i = 0; i < 7 ; i++ ) {
		outfile << to_string( correct1Counters[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << endl ;
	for( int i = 0; i < 7 ; i++ ) {
		outfile << to_string( correct2Counters[ i ]) + "," + to_string( numberLines ) + "; " ;
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

	return 0;
}