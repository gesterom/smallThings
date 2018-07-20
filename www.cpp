#include <iostream>
#include <list>
#include <fstream>

template<typename T>
class ListElement {
	T value;
	ListElement<T>* next;
  public:
	ListElement( T value ) {
		this->next = nullptr;
		this->value = value;
	}
	void setNext(ListElement<T>* n)
	{
		delete this->next;
		this->next = n;
	}
	ListElement<T>& getNext()
	{
		return *next;
	}
	bool hasNext()
	{
		return this->next!=nullptr;
	}
	bool isNull()
	{
		return this==nullptr;
	}
	T get()
	{
		return this->value;
	}
};

template<typename T>
class FileList {
	std::fstream file;

	int index;
	int size;
	int ramSize;
	int maxRamSize;

	ListElement<T>* head;
	ListElement<T>& curent;
	ListElement<T>& tail;
	
	template<typename Tem>
	class Iterator {

		FileList<Tem>& list;

		Iterator(FileList<Tem>& list) : list(list)
		{
			
		}

		public:

		bool operator!=( Iterator& other )
		{
			return not list.curent.isNull();
		}
		Iterator& operator++()
		{
			list.index++;
			list.curent = list.curent.getNext();
		}
		T operator*()
		{
			if(list.index<list.ramSize)
				return list.curent.get();
			else if ( list.index < list.maxRamSize)
			{
				throw new int;
			}
			else {
				
			}
		}
	};

	Iterator<T> begin()
	{
		return Iterator(*this);  
	}
	Iterator<T> end()
	{
		return Iterator(*this);
	}
};


int main( int argv, char** args ) {
	return 0;
}





