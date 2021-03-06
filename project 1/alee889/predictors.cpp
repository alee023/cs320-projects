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

unordered_map<unsigned long long, int> selectorTable ; 
unordered_map<unsigned long long, unsigned long long> btb ;

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

void predictors() {
	string behavior, line ;
	unsigned long long addr, target ;
	int TCounter = 0 ;
	int NTCounter = 0 ;
	int correct1Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ;
	int correct2Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ;
	int correctGshare [9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
	int divisors[ 7 ] = { 16, 32, 128, 256, 512, 1024, 2048 } ; // btb uses 512 which is at index 4
	int ghrBits[ 9 ] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 } ;
	int biPrediction, gSharePrediction, btbPrediction ;
	int correctSelector = 0 ;
	int attemptedBTB = 0;
	int correctBTB = 0 ;

	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> hex >> addr >> behavior >> hex >> target ;
		addr %= 2048 ;

		if( behavior == "T" ) {
			TCounter++ ;
		}
		else if( behavior == "NT" ) {
			NTCounter++ ;
		}
		numberLines++ ;

		for( int i = 0; i < 7; i++ ) { // i corresponds with index used to access array divisors
			unordered_map<unsigned long long, int>* x = oneMaps[ i ] ;
			unordered_map<unsigned long long, int>* y = twoMaps[ i ] ;
			// cout << to_string( i + 1 ) + ": " << endl ;
			unsigned long long specificAddr = addr % divisors[ i ];

			if( !(x->count( specificAddr ))) {
				(*x)[ specificAddr ] = 1 ;
				(*y)[ specificAddr ] = 11 ;
				if( i == 4 ) {
					btb[ specificAddr ] = 0 ;
				}
				if( i == 6 ) { // add addr key to selector table if taking 11 bits of PC
					selectorTable[ specificAddr ] = 0 ;
				}
			}

			if( i == 4 ) {
				btbPrediction = (*x)[ specificAddr ] ; // get 9 bit PC bimodal prediction
			}

			if( behavior == "T" ) {
				if((*x)[ specificAddr ] == 0 ) {
					(*x)[ specificAddr ] = 1 ;
				}
				else correct1Counters[ i ]++ ;

			}
			else { // behavior is NT
				if( (*x)[ specificAddr ] == 0 ) {
					correct1Counters[ i ]++ ;
				}
				else (*x)[ specificAddr ] = 0 ;
			}
			if( i == 6 ) { 
				biPrediction = (*y)[ specificAddr ] ; // get 11 bit PC bimodal prediction
			}
			setMaps( y, specificAddr, behavior, correct2Counters, i ) ; // set 2 bit table
			
			if( i == 4 ) { // btb
				if( btbPrediction == 1 ) { // if predicted to be true
					attemptedBTB++ ; // ... then attempt btb prediction
					if( btb[ specificAddr ] == target ) {
						correctBTB++ ;
					}
				}
				if( behavior == "T" ) {
					btb[ specificAddr ] = target ; // update btb table
				}			
			}

			if( i == 6 ) { // gshare
				for( int j = 0; j < 9; j++ ) { // for each 9 ghrs
					unsigned long long xorAddr = specificAddr ^ ghrs[ j ] ;
					unordered_map<unsigned long long, int>* gShareTable = ghrMaps[ j ] ;

					if( !(gShareTable->count( xorAddr ))) {
						(*gShareTable)[ xorAddr ] = 11 ;
					}

					if( j == 8 ) {
						gSharePrediction = (*gShareTable)[ xorAddr ] ;
					}
					setMaps( gShareTable, xorAddr, behavior, correctGshare, j ) ;
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

				// selector code
				if( selectorTable[ specificAddr ] == 0 || selectorTable[ specificAddr ] == 1 ) { // gshare
					if(( behavior == "T" && ( gSharePrediction == 10 || gSharePrediction == 11 )) || ( behavior == "NT" && ( gSharePrediction == 0 || gSharePrediction == 1 ))) {
						correctSelector++ ;
						if( selectorTable[ specificAddr ] == 1 && ( gSharePrediction/10 != biPrediction/10 )) { // 01
							selectorTable[ specificAddr ] = 0 ; 
						}
					}
					else if(( behavior == "T" && ( gSharePrediction == 0 || gSharePrediction == 1 )) || ( behavior == "NT" && ( gSharePrediction == 10 || gSharePrediction == 11 ))) {
						if( selectorTable[ specificAddr ] == 0 && ( gSharePrediction/10 != biPrediction/10 )) { // 00
							selectorTable[ specificAddr ] = 1 ; // set to 01
						}
						else if( selectorTable[ specificAddr ] == 1 && ( gSharePrediction/10 != biPrediction/10 )){ // 01
							selectorTable[ specificAddr ] = 10 ; 
						}
					}
				}
				else if( selectorTable[ specificAddr ] == 10 || selectorTable[ specificAddr ] == 11 ) { // prefer bimod 
					if(( behavior == "T" && ( biPrediction == 10 || biPrediction == 11 )) || ( behavior == "NT" && ( biPrediction == 0 || biPrediction == 1 ))) {
						correctSelector++ ;
						if( selectorTable[ specificAddr ] == 10 && ( gSharePrediction/10 != biPrediction/10 )) {
							selectorTable[ specificAddr ] = 11 ; 
						}
					}
					else if(( behavior == "T" && ( biPrediction == 0 || biPrediction == 1 )) || ( behavior == "NT" && ( biPrediction == 10 || biPrediction == 11 ))) {
						if( selectorTable[ specificAddr ] == 11 && ( gSharePrediction/10 != biPrediction/10 )) {
							selectorTable[ specificAddr ] = 10 ;
						}
						else if( selectorTable[ specificAddr ] == 10 && ( gSharePrediction/10 != biPrediction/10 )){ // 10
							selectorTable[ specificAddr ] = 1 ; // set to 01
						}
					}
				}
			}
			
		}
	}

	infile.close() ;
	ofstream outfile( writeFile ) ;
	outfile << to_string( TCounter ) + "," + to_string( numberLines ) + ";" << endl ;
	outfile << to_string( NTCounter ) + "," + to_string( numberLines ) + ";" << endl ;

	for( int i = 0; i < 6 ; i++ ) {
		outfile << to_string( correct1Counters[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << to_string( correct1Counters[ 6 ]) + "," + to_string( numberLines ) + ";" << endl ;
	for( int i = 0; i < 6 ; i++ ) {
		outfile << to_string( correct2Counters[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << to_string( correct2Counters[ 6 ]) + "," + to_string( numberLines ) + ";" << endl ;
	for( int i = 0; i < 8 ; i++ ) {
		outfile << to_string( correctGshare[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << to_string( correctGshare[ 8 ]) + "," + to_string( numberLines ) + ";" << endl ;
	outfile << to_string( correctSelector ) + "," + to_string( numberLines ) + ";" << endl ;
	outfile << to_string( correctBTB ) + "," + to_string( attemptedBTB ) + ";" << endl ;
	outfile.close() ;
}

int main( int argc, char *argv[]) {
	unsigned long long addr;
	string behavior, line;
	unsigned long long target;
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	predictors() ;

	return 0;
}