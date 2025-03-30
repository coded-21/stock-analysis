#include "pch.h"
#include "Candlestick.h"

// Constructor that takes a string of the form ("timestamp, open, high, low, close, volume") and returns a candlestick
Candlestick::Candlestick(String^ candlestickString) {

    // Create an array of separators that will be used to seperate the string into tokens
    array<wchar_t>^ seperators = gcnew array<wchar_t>{',', '\"'};

    // Preprocess the string (split it, and remove empty entries, if any)
    array<String^>^ tokens = candlestickString->Split(seperators, StringSplitOptions::RemoveEmptyEntries);
    
    // If we have 6 tokens, it means that the submitted string was in the correct format
    if (tokens->Length == 6) {

        // Parse the tokens into types that we need them to be in
        this->Timestamp = DateTime::Parse(tokens[0]);
        this->Open = Convert::ToDouble(tokens[1]);
        this->High = Convert::ToDouble(tokens[2]);
        this->Low = Convert::ToDouble(tokens[3]);
        this->Close = Convert::ToDouble(tokens[4]);
        this->Volume = Convert::ToInt64(tokens[5]);
    }
    else {
        throw gcnew ArgumentException("The input string is in the wrong format."); // Throw an exception if we catch an error
    }
}