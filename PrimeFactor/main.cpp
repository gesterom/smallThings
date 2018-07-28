#include "stream.hpp"
#include <list>
#include <functional>
#include <chrono>
#include <iomanip>

int checkPrime( IStream<int>& s, int number ) {
	while ( true ) {
		try {
			int t = s.get();
			if( number % t == 0 ) {
				return t;
			if(number/t < t)
				return 0;
			}
		}
		catch( EndStreamException * e ) {
			delete e;
			return 0;
		}
	}
}

struct statistics
{
	std::chrono::time_point<std::chrono::high_resolution_clock> sec;
	std::chrono::time_point<std::chrono::high_resolution_clock> started;
  int lastPrimeNumber=0;
	int sizeOfPrimeNumbersGroup=0;
	int maxRun=0;
	int lastNumber=0;
	int lastProgres=0;
};

class PrimeSolver {
	BufferedStreamDecorator<int>* primeStream;
	FileStream<int>* outputStream;
	int lastNumber;

	statistics stat;

	public:
	PrimeSolver( std::string filePrime, std::string fileOutput, int maxRam ) {

		auto r = new RamStream<int>();
		std::fstream prime( filePrime );
		std::fstream output( fileOutput );

		if( prime.good() && output.good() ) {
			prime.seekg( 0, std::ios_base::end );
			unsigned long e = prime.tellg();
			stat.sizeOfPrimeNumbersGroup = e/sizeof(int);
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
	void print(int i)
	{
		auto now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> test = now-stat.sec;
		if( test.count()  >= 2.5)
		{
			std::cout<<"====================================================================================="<<std::endl<<std::endl<<std::endl;;
			//system("clear");

			//Print stat;
			std::cout<<"Calcucate numbers : "<<i<<std::endl;
			std::cout<<"Last prime number : "<<stat.lastPrimeNumber<<std::endl;
			std::cout<<"Progres in 2.5 s : "<<i - stat.lastProgres<<std::endl;
			std::chrono::duration<double> ttt = (now -stat.started);
			std::cout<<"Elapsed : "<<ttt.count()<<" s | "<<ttt.count()/(double)60 <<" min | "<<ttt.count()/(double)60/60<<" h |"<<ttt.count()/(double)60/60/24<<" days"<<std::endl;
			std::cout<<"The number of prime numbers : "<<stat.sizeOfPrimeNumbersGroup<<std::endl;
			std::cout<<"Percent of prime numbers : "<<std::setprecision(6)<<(stat.sizeOfPrimeNumbersGroup/(double)(stat.maxRun)) * 100<<"%"<<std::endl;
			std::cout<<"Avg per number : "<< std::chrono::duration_cast<std::chrono::milliseconds>(test).count()/(double)( i - stat.lastProgres)<<" ms "<<std::endl;
			std::cout<<std::endl;
			auto z = ( test.count() / ( i - stat.lastProgres) ) * (stat.maxRun - i);
			std::cout<<"Predicted complete time : "<< (double)z << " s | "<<(double)z/60 << " min | "<<(double)z/60/60<<" h | "<<(double)z/60/60/24<<" days " <<std::endl;
			std::time_t c = std::chrono::high_resolution_clock::to_time_t( now + std::chrono::seconds((long)z) );
			std::cout<<"Predicted complete date : "<<std::put_time(std::localtime(&c),"%H:%M:%S %e %A %B %Y")<<std::endl;
			std::cout<<"Progress : "<<(i/(double)(stat.maxRun))*100<<"%"<<std::endl;
			
			stat.lastProgres = i;


			stat.sec = std::chrono::high_resolution_clock::now();
		}
	}
	void calcucate( int i) {
		int divider = checkPrime( *primeStream, i );
		outputStream->put( divider );
		if( divider == 0 ) {
			primeStream->put( i );
			stat.lastPrimeNumber = i;
			stat.sizeOfPrimeNumbersGroup++;
		}
		primeStream->reset();
	}
	void run( int maxRun ) {
		stat.started= std::chrono::high_resolution_clock::now();
		stat.sec= std::chrono::high_resolution_clock::now();
		stat.maxRun = maxRun;
		for( int i = lastNumber ; i < maxRun; i++ ) {
			calcucate( i );
			print( i );
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
		if( number == 0 || number == 1 )
			return {};

		std::cout << number << std::endl;

		file.seekg( ( number - 2 ) * sizeof( int )  );

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
		std::cout << "usage : " << std::endl;
		std::cout << args[0] << " -f <number> ; #print number factorization" << std::endl;;
		std::cout << args[0] << " <number> [prime.prime_FileName] [prime.factor_FileName] [maxNumbersSavedInRam] # generate prime.prime file contain prime number in binary form" << std::endl;;
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

		if( l.size() != 0 ) {
			std::string res;

			for( auto i : l ) {
				res += std::to_string( i );
				res += " * ";
			}

			res = res.substr( 0, res.size() - 3 );
			std::cout << res;
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





