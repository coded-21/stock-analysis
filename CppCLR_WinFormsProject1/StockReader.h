#pragma once
#include "Candlestick.h"

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;

public ref class StockReader {
public:
	// A static prototype function; we can call it without instantiating an object of the StockReader class
	static List<Candlestick^>^ ReadFromCSV(String^ filePath);
};