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

// MAPS: key is last x-bits of PC, value is prediction
// bimodal tables 
unordered_map<unsigned long long, int> one4, one5, one7, one8, one9, one10, one11 ;
unordered_map<unsigned long long, int> two4, two5, two7, two8, two9, two10, two11 ;
vector< unordered_map<unsigned long long, int>*> oneMaps{ &one4, &one5, &one7, &one8, &one9, &one10, &one11 } ;
vector< unordered_map<unsigned long long, int>*> twoMaps{ &two4, &two5, &two7, &two8, &two9, &two10, &two11 } ;

// gshare table
unordered_map<unsigned long long, int> ghr3, ghr4, ghr5, ghr6, ghr7, ghr8, ghr9, ghr10, ghr11 ;
vector< unordered_map<unsigned long long, int>*> ghrMaps{ &ghr3, &ghr4, &ghr5, &ghr6, &ghr7, &ghr8, &ghr9, &ghr10, &ghr11 } ;
unsigned long long ghrs [ 9 ] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;

unordered_map<unsigned long long, int> selectorTable ; 
unordered_map<unsigned long long, unsigned long long> btb ;

// sets table values; used by 2 bit bimodal and gshare
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
	int correct1Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ; // 1 bit bimodal counters 
	int correct2Counters [7] = { 0, 0, 0, 0, 0, 0, 0 } ; // 2 bit bimodal counters
	int correctGshare [9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 } ; // gshare counters
	int divisors[ 7 ] = { 16, 32, 128, 256, 512, 1024, 2048 } ; // used to modulo the addr to get last divisors[i] bits; btb uses 512 which is at index 4
	int ghrBits[ 9 ] = { 8, 16, 32, 64, 128, 256, 512, 1024, 2048 } ; // modulo to get the last ghrBits[i] bits of the newly shifted ghr
	int biPrediction, gSharePrediction, btbPrediction ;
	int correctSelector = 0 ;
	int attemptedBTB = 0;
	int correctBTB = 0 ;

	ifstream infile( readFile ) ;
	while( getline( infile, line )) {
		stringstream s( line ) ;
		s >> hex >> addr >> behavior >> hex >> target ;
		addr %= 2048 ;

		// static predictors
		if( behavior == "T" ) {
			TCounter++ ;
		}
		else if( behavior == "NT" ) {
			NTCounter++ ;
		}

		numberLines++ ;

		// bimodal, gshare, tournament, btb
		for( int i = 0; i < 7; i++ ) { // i corresponds with index used to access array divisors
			unordered_map<unsigned long long, int>* x = oneMaps[ i ] ;
			unordered_map<unsigned long long, int>* y = twoMaps[ i ] ;
			unsigned long long specificAddr = addr % divisors[ i ]; // get last x bits of PC

			if( !(x->count( specificAddr ))) { // if the addr isn't in the 1 bit bimodal table
				(*x)[ specificAddr ] = 1 ;
				(*y)[ specificAddr ] = 11 ; // ... then it must also be missing from the 2 bit bimodal table
				if( i == 4 ) { // ... as well as the btb table
					btb[ specificAddr ] = 0 ;
				}
				if( i == 6 ) { // ... and the gshare table
					selectorTable[ specificAddr ] = 0 ;
				}
			}

			if( i == 4 ) {
				btbPrediction = (*x)[ specificAddr ] ; // get bimodal prediction (if looking at last 9 bits) before it is updated below; used for btb
			}
			// update 1 bit bimodal table
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

			if( i == 6 ) { // get bimodal prediction (if looking at last 11 bits) before it is updated below; used for gshare
				biPrediction = (*y)[ specificAddr ] ; 
			}
			setMaps( y, specificAddr, behavior, correct2Counters, i ) ; // set 2 bit counter table
			
			if( i == 4 ) { // looking at last 9 bits of PC --> BTB code
				if( btbPrediction == 1 ) { 
					attemptedBTB++ ; 
					if( btb[ specificAddr ] == target ) {
						correctBTB++ ;
					}
				}
				if( behavior == "T" ) {
					btb[ specificAddr ] = target ; // update btb table
				}			
			}

			if( i == 6 ) { // looking at last 9 bits of PC --> gshare code
				for( int j = 0; j < 9; j++ ) { // for each 9 ghrs
					unsigned long long xorAddr = specificAddr ^ ghrs[ j ] ;
					unordered_map<unsigned long long, int>* gShareTable = ghrMaps[ j ] ;

					if( !(gShareTable->count( xorAddr ))) { 
						(*gShareTable)[ xorAddr ] = 11 ;
					}

					if( j == 8 ) { // get prediction corresponding w 11 bit ghr before table is updated; used later in tournament predictor
						gSharePrediction = (*gShareTable)[ xorAddr ] ;
					}
					setMaps( gShareTable, xorAddr, behavior, correctGshare, j ) ; // update table
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
					if(( behavior == "T" && ( gSharePrediction == 10 || gSharePrediction == 11 )) || ( behavior == "NT" && ( gSharePrediction == 0 || gSharePrediction == 1 ))) { // if prediction matches outcome
						correctSelector++ ;
						if( selectorTable[ specificAddr ] == 1 && ( gSharePrediction/10 != biPrediction/10 )) { // selector = 01 so update towards 00
							selectorTable[ specificAddr ] = 0 ; 
						}
					}
					else if(( behavior == "T" && ( gSharePrediction == 0 || gSharePrediction == 1 )) || ( behavior == "NT" && ( gSharePrediction == 10 || gSharePrediction == 11 ))) { // prediction is wrong
						if( selectorTable[ specificAddr ] == 0 && ( gSharePrediction/10 != biPrediction/10 )) { // 00
							selectorTable[ specificAddr ] = 1 ; // set to 01
						}
						else if( selectorTable[ specificAddr ] == 1 && ( gSharePrediction/10 != biPrediction/10 )){ // 01
							selectorTable[ specificAddr ] = 10 ; 
						}
					}
				}
				else if( selectorTable[ specificAddr ] == 10 || selectorTable[ specificAddr ] == 11 ) { // prefer bimod 
					if(( behavior == "T" && ( biPrediction == 10 || biPrediction == 11 )) || ( behavior == "NT" && ( biPrediction == 0 || biPrediction == 1 ))) { // correct prediction
						correctSelector++ ;
						if( selectorTable[ specificAddr ] == 10 && ( gSharePrediction/10 != biPrediction/10 )) {
							selectorTable[ specificAddr ] = 11 ; 
						}
					}
					else if(( behavior == "T" && ( biPrediction == 0 || biPrediction == 1 )) || ( behavior == "NT" && ( biPrediction == 10 || biPrediction == 11 ))) { // wrong prediction
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
	ofstream outfile( writeFile, fstream::app ) ;
	outfile << to_string( TCounter ) + "," + to_string( numberLines ) + ";" << endl ;
	outfile << to_string( NTCounter ) + "," + to_string( numberLines ) + ";" << endl ;

	for( int i = 0; i < 7 ; i++ ) {
		outfile << to_string( correct1Counters[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << endl ;
	for( int i = 0; i < 7 ; i++ ) {
		outfile << to_string( correct2Counters[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << endl ;
	for( int i = 0; i < 9 ; i++ ) {
		outfile << to_string( correctGshare[ i ]) + "," + to_string( numberLines ) + "; " ;
	}
	outfile << endl ;
	outfile << to_string( correctSelector ) + "," + to_string( numberLines ) + "; " << endl ;
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