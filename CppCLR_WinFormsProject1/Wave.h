#pragma once
#include "PeakValley.h"

public ref class Wave
{
public:
	property PeakValley^ pv1;
	property PeakValley^ pv2;
	property int direction; // 0 for upward and 1 for downward

	Wave(PeakValley^ pv1, PeakValley^ pv2) {
		this->pv1 = pv1;
		this->pv2 = pv2;

		if (pv1->Timestamp > pv2->Timestamp) // pv1 comes later
		{
			direction = pv1->IsPeak ? 0 : 1; // if the right peak/vally is a peak, then the wave is an upward wave, else downward
		}
	}

	// Override ToString to control display text
	virtual String^ ToString() override {
		auto start = pv1->Timestamp < pv2->Timestamp ? pv1 : pv2;
		auto end = pv1->Timestamp > pv2->Timestamp ? pv1 : pv2;
		return String::Format("{0} -> {1}", start->Timestamp.ToShortDateString(), end->Timestamp.ToShortDateString());
	}

};

