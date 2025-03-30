#include "pch.h"
#include "StockReader.h"
#include "Candlestick.h"

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;


// This function reads the CSV file and takes each line, converts it into a candlestick, and adds that candlestick to a list; it returns the list of candlesticks
List<Candlestick^>^ StockReader::ReadFromCSV(String^ filePath) {
	List<Candlestick^>^ candlesticks = gcnew List<Candlestick^>();

	try {
		// Open the CSV file
		StreamReader^ reader = gcnew StreamReader(filePath);

		// Read the first line and skip it, so that the headers are not included
		String^ line = reader->ReadLine();

		while (!reader->EndOfStream) {
			line = reader->ReadLine(); // Read the lines one by one
			if (!String::IsNullOrWhiteSpace(line)) { // Check whether the string is not NULL or whitespace
				// Create a candlestick object from the line and append it to the list of candlesticks
				Candlestick^ newCandlestick = gcnew Candlestick(line); // Using the string constructor
				candlesticks->Add(newCandlestick); // Add the new candlestick to our list
			}
		}

		reader->Close(); // Close the StreamReader once it's done reading the file
	} catch (Exception^ ex) {
		Console::WriteLine("Error reading CSV file: " + ex->Message); // If we catch an error, we print out so to the console
	}

	return candlesticks; // Return the candlesticks
}