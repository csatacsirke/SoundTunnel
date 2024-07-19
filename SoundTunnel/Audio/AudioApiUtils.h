#pragma once



#include <Audioclient.h>

#include <Mmdeviceapi.h>
#include <Functiondiscoverykeys_devpkey.h>

#include <vector>
#include <memory>
#include <exception>
#include <thread>
#include <algorithm>
#include <expected>


//using namespace std;
//using std::vector;

// https://msdn.microsoft.com/en-us/library/windows/desktop/dd370800(v=vs.85).aspx


#define EXIT_ON_ERROR(hr) if(FAILED(hr)) {ASSERT(0); return hr;}


// TODO : ezt a p�ld�b�l m�soltam, faszsetudja mi a j�
// update: min�l kisebb ann�l kisebb a latency
// REFERENCE_TIME time units per second and per millisecond
//#define __REFTIMES_PER_SEC  (10000000/16)
#define __REFTIMES_PER_SEC  (10000000)
#define __REFTIMES_PER_MILLISEC  (__REFTIMES_PER_SEC/1000)
// TODO tal�n 20 ms-  re?
//static const REFERENCE_TIME BUFFER_DURATION_HNS = __REFTIMES_PER_SEC / 16;
static const REFERENCE_TIME BUFFER_DURATION_HNS = __REFTIMES_PER_MILLISEC * 20;



class CPropVariant : public PROPVARIANT {

public:
	CPropVariant() {
		PropVariantInit(this);
	}

	~CPropVariant() {
		PropVariantClear(this);
	}



};


// TODO: namespace?
struct Chunk {
	const byte* ptr;
	size_t size;
};

class BufferIterator {
	const std::vector<byte>& buffer;
	size_t offset = 0;
	//size_t chunkSize = 1;
public:
	//BufferIterator(const std::vector<byte>& buffer, size_t chuckSize) : buffer(buffer), chunkSize(chuckSize) {
	//	NULL;
	//}

	BufferIterator(const std::vector<byte>& buffer) : buffer(buffer) {
		NULL;
	}


	bool HasNext() const {
		const byte* start = buffer.data();
		const byte* ptr = start + offset;
		const byte* end = start + buffer.size();

		return (end > ptr);
	}

	//Chunk NextChunk() {
	//	return NextChunk(chunkSize);
	//}

	Chunk NextChunk(size_t maxSize) {
		const byte* start = buffer.data();
		const byte* ptr = start + offset;
		const byte* end = start + buffer.size();

		offset += maxSize;

		if (end > ptr) {
			return { ptr, std::min<size_t>(end - ptr, maxSize) };
		} else {
			return { ptr, 0 };
		}

	}


};
