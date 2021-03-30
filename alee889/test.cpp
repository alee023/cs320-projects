#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;

int main( int argc, char *argv[]) {
  unsigned long long addr;
  string behavior, line;
  unsigned long long target;

  ifstream infile( "test_input.txt" );
  ofstream outfile( "test_output.txt" ) ;
  while( getline( infile, line )) {
    stringstream s( line );
    s >> std::hex >> addr >> behavior >> std::hex >> target;
    outfile << addr;
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
  return 0;
}