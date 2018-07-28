#include <iostream>
#include <fstream>

#define PRINT(x) {std::cout<<#x<<" ["<<__PRETTY_FUNCTION__<<"] <"<<__FILE__<<"> "<<__LINE__<<std::endl<<x<<std::endl;}

class EndStreamException {};

template<typename T>
class IStream {
  public:
	virtual T get() = 0;
	virtual void put( T value ) = 0 ;
	virtual void skip() = 0;
	virtual void reset() = 0;
	virtual ~IStream() {}
};


template<typename T>
class RamStream : public IStream<T> {
	//template<typename T>
	struct ListElement {
		T value;
		ListElement* next = nullptr;
	};

	ListElement* head;
	ListElement* curent;
	ListElement* tail;

  public:
	RamStream() {
		head = nullptr;
		curent = nullptr;
		tail = nullptr;
	}
	virtual T get() override {
		if( curent ) {
			T res = curent->value;
			curent = curent->next;
			return res;
		}
		else {
			throw new EndStreamException();
		}
	}
	virtual void put( T value ) override {
		if( head == nullptr ) {
			head = new ListElement();
			head->value = value;
			tail = head;
			curent = head;
			return ;
		}

		tail->next = new ListElement();
		tail = tail->next;
		tail->value = value;
	}
	virtual void skip() override {
		curent = curent->next;
	}
	virtual void reset() override {
		curent = head;
	}
	virtual ~RamStream() override {
		delete head;
	}
};

template<typename T>
class FileStream : public IStream<T> {
	std::fstream file;
  public:
	FileStream( std::string filePath ) {
		file.open( filePath, std::ios_base::binary | std::ios_base::in | std::ios_base::out );
	}
	virtual T get() override {
		if( file.good() ) {
			int c = file.tellg ();
			file.seekg( 0, std::ios_base::end );
			int end = file.tellg();
			if( c > end || ( (end - c) < sizeof( T ) ) ) {
				throw new EndStreamException();
			}

			file.seekg( c );

			T res;
			file.read( (char*)&res, sizeof( T ) );
			return res;
		}
		else {
			throw new EndStreamException();
		}
	}
	virtual void put( T value ) override {
		file.seekp( 0, std::ios_base::end );
		file.write( ( char* )&value, sizeof( T ) );
	}
	virtual void skip() override {
		file.seekg( sizeof( T ), std::ios_base::cur );
	}
	virtual void reset() override {
		file.seekg( 0, std::ios_base::beg );
	}
	virtual ~FileStream() override {
		file.close();
	}
};

template<typename T>
class BufferedStreamDecorator : public IStream<T> {
	IStream<T>* buffor;
	int bufforSize;
	int maxBufforSize;
	IStream<T>* master;
  public:
	BufferedStreamDecorator( IStream<T>* buffor, IStream<T>* master, int maxSize ) {
		this->buffor = buffor;
		this->master = master;
		this->maxBufforSize = maxSize;
		this->bufforSize = 0;
	}

	virtual T get() override {
		try {
			T res = buffor->get();
			master->skip();
			return res;
		}
		catch( EndStreamException* e ) {
			delete e;
			return master->get();
		}
	}
	virtual void put( T value ) override {
		if( bufforSize < maxBufforSize ) {
			buffor->put( value );
			bufforSize++;
		}

		master->put( value );
	}
	virtual void reset() override {
		buffor->reset();
		master->reset();
	}
	virtual void skip() {
		buffor->skip();
		master->skip();
	}
	virtual ~BufferedStreamDecorator() override {
		delete buffor;
		delete master;
	}
};
