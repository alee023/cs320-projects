#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<unordered_map>
#include<bitset>
#include<vector>

using namespace std;
string readFile, writeFile ;
int numberLines = 0 ;

unordered_map<unsigned long long, int> one4, one5, one7, one8, one9, one10, one11 ;
unordered_map<unsigned long long, int> two4, two5, two7, two8, two9, two10, two11 ;
vector< unordered_map<unsigned long long, int>*> oneMaps{ &one4, &one5, &one7, &one8, &one9, &one10, &one11 } ;
vector< unordered_map<unsigned long long, int>*> twoMaps{ &two4, &two5, &two7, &two8, &two9, &two10, &two11 } ;

unordered_map<unsigned long long, int> ghr3, ghr4, ghr5, ghr6, ghr7, ghr8, ghr9, ghr10, ghr11 ;
vector< unordered_map<unsigned long long, int>*> ghrMaps{ &ghr3, &ghr4, &ghr5, &ghr6, &ghr7, &ghr8, &ghr9, &ghr10, &ghr11 } ;
unsigned long long ghrs [ 9 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;

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

void setMaps( unordered_map<unsigned long long, int>* x, unsigned long long addr, string behavior, int* arr, int i ) {
	if( behavior == "T" ) {
		if((*x)[ addr ] == 0 ) { // 00-SNT
			(*x)[ addr ] = 1 ;
		}
		else if((*x)[ addr ] == 1 ) { // 01-NT
			(*x)[ addr ] = 10 ;
		}
		else if((*x)[ addr ] == 10 ) { // 10-T
			(*x)[ addr ] = 11 ;
			arr[ i ]++ ;
		}
		else if((*x)[ addr ] == 11 ) { // 11-ST
			arr[ i ]++ ;
		}
	}
	else if( behavior == "NT" ) {
		if((*x)[ addr ] == 0 ) { // 00-SNT
			arr[ i ]++ ;
		}
		else if((*x)[ addr ] == 1 ) { // 01-NT
			(*x)[ addr ] = 0 ;
			arr[ i ]++ ;
		}
		else if((*x)[ addr ] == 10 ) { // 10-T
			(*x)[ addr ] = 1 ;
		}
		else if((*x)[ addr ] == 11 ) { // 11-ST
			(*x)[ addr ] = 10 ;
		}
	}
}

void bimodal() {
	string behavior, line, ignore ;
	unsigned long long addr;
	int correct1Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ;
	int correct2Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ;
	int correctGshare [9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
	int divisors[ 7 ] = { 16, 32, 128, 256, 512, 1024, 2048 } ;
	int ghrBits[ 9 ] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 } ;

	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> hex >> addr >> behavior >> ignore ;
		addr %= 2048 ;

		for( int i = 0; i < 7; i++ ) { // i corresponds with index used to access array divisors
			unordered_map<unsigned long long, int>* x = oneMaps[ i ] ;
			unordered_map<unsigned long long, int>* y = twoMaps[ i ] ;
			// cout << to_string( i + 1 ) + ": " << endl ;
			unsigned long long specificAddr = addr % divisors[ i ];

			if( !(x->count( specificAddr ))) {
				(*x)[ specificAddr ] = 1 ;
				(*y)[ specificAddr ] = 11 ;
			}

			if( behavior == "T" ) {
				if((*x)[ specificAddr ] == 0 ) {
					(*x)[ specificAddr ] = 1 ;
				}
				else correct1Counters[ i ]++ ;

				setMaps( y, specificAddr, behavior, correct2Counters, i ) ;
			}

			else { // behavior is NT
				if( (*x)[ specificAddr ] == 0 ) {
					correct1Counters[ i ]++ ;
				}
				else (*x)[ specificAddr ] = 0 ;

				setMaps( y, specificAddr, behavior, correct2Counters, i ) ;
			}
			/*
			if( i == 6 ) { // gshare
				for( int j = 0; j < 9; j++ ) { // for each 9 ghrs
					unsigned long long xorAddr = specificAddr ^ ghrs[ j ] ;
					unordered_map<unsigned long long, int>* gShareTable = ghrMaps[ j ] ;

					if( !(gShareTable->count( xorAddr ))) {
						(*gShareTable)[ xorAddr ] = 11 ;
					}

					if( behavior == "T" ) {
						// update GHR
						ghrs[ j ] <<= 1 ; // shift by 1
						ghrs[ j ] |= 1 ; // add 1
						ghrs[ j ] %= ghrBits[ j ] ;
					}
					else { // NT
						// update GHR
						ghrs[ j ] <<= 1 ; // shift by 1
						ghrs[ j ] %= ghrBits[ j ] ;
					}
				}
			}
			*/
		}
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