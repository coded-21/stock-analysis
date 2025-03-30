#pragma once
using namespace System;
#include "string"
#include "Candlestick.h"

public ref class PeakValley
{
public:
	property int index;
	property bool IsPeak;
	property bool IsValley;
	property int MarginLeft;
	property int MarginRight;
	property System::DateTime Timestamp;

	PeakValley(int index, bool isPeak, bool isValley, int marginLeft, int marginRight, DateTime timestamp) {
		this->index = index;
		this->IsPeak = isPeak;
		this->IsValley = isValley;
		this->MarginLeft = marginLeft;
		this->MarginRight = marginRight;
		this->Timestamp = timestamp;
	}
};

