#pragma once
#include <ostream>
#include <vector>
#include <iostream>

// RLE Compression/Decompression
struct RleData
{
	// Stores either compressed or decompressed data
	std::vector<int8_t> mData;
	
	// Compresses input data and stores it in mData
	void Compress(const std::vector<int8_t>& input);

	// Decompresses input data and stores it in mData
	void Decompress(const std::vector<int8_t>& input, size_t outSize);

	// Outputs mData
	friend std::ostream& operator<< (std::ostream& stream, const RleData& rhs);
	
	static size_t MaxRunSize() { return 127; }
};

