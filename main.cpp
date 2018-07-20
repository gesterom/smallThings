#include "stream.hpp"
#include <list>
#include <functional>


int checkPrime( IStream<int>& s, int number ) {
	while ( true ) {
		try {
			int t = s.get();
			if( number % t == 0 ) {
				return t;
			}
		}
		catch( ... ) {
			return 0;
		}
	}
}

template<typename T>
void printStream(IStream<T>& s)
{
	while ( true ) {
		try {
			int t = s.get();
			std::cout<<t<<"-";
		}
		catch( ... ) {
			std::cout<<"end"<<std::endl;
			return;
		}
	}
	s.reset();
}

class PrimeSolver {
	BufferedStreamDecorator<int>* primeStream;
	FileStream<int>* outputStream;
	int lastNumber;
  public:
	PrimeSolver( std::string filePrime, std::string fileOutput, int maxRam ) {
		auto r = new RamStream<int>();
		std::fstream prime( filePrime );
		std::fstream output( fileOutput );

		if( prime.good() && output.good() ) {
			prime.seekg( 0, std::ios_base::end );
			unsigned long e = prime.tellg();
			prime.seekg( 0, std::ios_base::beg );

			for( int i = 0; i < maxRam && i * sizeof( int ) < e ; i++ ) {
				int t;
				prime.read( ( char* )&t, sizeof( t ) );
				r->put( t );
			}

			output.seekp( 0, std::ios_base::end );
			int end = output.tellp();
			lastNumber = end / sizeof( int ) + 2;
		}
		else {
			std::ofstream file( filePrime );
			std::ofstream out( fileOutput );
			lastNumber = 2;
		}

		prime.close();
		output.close();
		primeStream = new BufferedStreamDecorator<int>( r, new FileStream<int>( filePrime ), maxRam );
		outputStream = new FileStream<int>( fileOutput );
	}
	void run( int maxRun ) {
		
		for( int i = lastNumber ; i < maxRun; i++ ) {
			int divider = checkPrime( *primeStream, i );
			outputStream->put( divider );

			if( divider == 0 ) {
				primeStream->put( i );
			}
			primeStream->reset();
		}
	}
	~PrimeSolver() {
		delete primeStream;
		delete outputStream;
	}
};

class DataFactorReader {
	std::fstream file;
  public:
	DataFactorReader( std::string filePath ) {
		file.open( filePath );
	}
	std::list<int> getFactor( int number ) {
		if(number == 0 || number == 1 ) return {};
		std::cout<<number<<std::endl;
		file.seekg( (number-2) * sizeof( int )  );
		int t;
		file.read( ( char* )&t, sizeof( int ) );

		if( t == 0 ) {
			return {number};
		}

		auto res = getFactor( number / t );
		res.push_front( t );
		return res;
	}
	~DataFactorReader() {
		file.close();
	}
};

int main( int argv, char** args ) {



	std::string prime = "prime.prime";
	std::string output = "prime.factor";
	int maxRam = 65535;


	if( argv < 2 ) {
		return -1;
	}

	if( argv >= 4 ) {
		prime = args[2];
		output = args[3];
	}

	if( argv >= 5 ) {
		maxRam = std::atoi( args[4] );
	}

	std::string factor = "-f";

	if( factor == args[1] and argv >= 3 ) {
		DataFactorReader data( output );
		auto l = data.getFactor( std::atoi( args[2] ) );
		std::cout << args[2] << " : ";

		if( l.size() != 0 )
		{
			std::string res;
			for( auto i : l ) {
				res+=std::to_string(i);
				res+=" * ";
			}
			res = res.substr(0,res.size()-3);
			std::cout<<res;
		}	
		else {
			std::cout << "prime";
		}

		std::cout << std::endl;
	}
	else {
		PrimeSolver p( prime, output, maxRam );
		p.run( std::atoi( args[1] ) );
	}

	return 0;
}





