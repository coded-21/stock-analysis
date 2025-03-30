#pragma once
using namespace System;
#include "string"


public ref class Candlestick
{
public:
    property unsigned long Volume;
    property double Close;
    property double Low;
    property double High;
    property double Open;
    property System::DateTime Timestamp;

    // Default constructor
    Candlestick() {}

    // Constructor with parameters
    Candlestick(double open, double high, double low, double close, unsigned long volume, System::DateTime timestamp) {
        Timestamp = timestamp;
        Open = open;
        High = high;
        Low = low;
        Close = close;
        Volume = volume;
    }

    // Constructor that takes a string and processes it to display a candlestick
    Candlestick(String^ candlestickString);
};
