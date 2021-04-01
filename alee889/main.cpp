#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;
string readFile, writeFile ;
int numberLines = 0 ;

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

int main( int argc, char *argv[]) {
	unsigned long long addr;
	string behavior, line;
	unsigned long long target;
	readFile = argv[ 1 ] ;
	writeFile = argv[ 2 ] ;

	AT() ;

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