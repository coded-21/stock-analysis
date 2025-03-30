#pragma once
#include "StockReader.h"
#include "PeakValley.h"
#include "Wave.h"

using namespace System::Windows::Forms::DataVisualization::Charting;

namespace CppCLRWinFormsProject {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form_UI
	/// </summary>
	public ref class Form_UI : public System::Windows::Forms::Form
	{
		// Initializing the StockReader and a list of Candlesticks
		// property StockReader^ stockReader;
		property List<Candlestick^>^ candlesticks;
		property List<Candlestick^>^ filteredCandlesticks;
		property Dictionary<int, List<PeakValley^>^>^ finalMarginMap;
		property Dictionary<int, List<Wave^>^>^ finalWaveMap;
		property String^ filePath;
	public:
		Form_UI(void) {
			InitializeComponent();
		}

		Form_UI(String^ filepath, DateTime startDate, DateTime endDate)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			// Constructing the StockReader
			// stockReader = gcnew StockReader();

			// Save the filepath
			this->filePath = filePath;
			dateTimePicker_startDate->Value = startDate;


			candlesticks = StockReader::ReadFromCSV(filepath);
			display(filepath);

			// Set the title (caption) of the main window to "Stock Price Analyzer" for funsies
			this->Text = "Stock Price Analyzer";

			// Then load
			// loadTicker(filePath);

			// Tell thd form to show itself
			Show();

		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form_UI()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::OpenFileDialog^ openFileDialog_LoadTicker;
	private: System::Windows::Forms::Button^ button_LoadTicker;



	private: System::Windows::Forms::DateTimePicker^ dateTimePicker_startDate;
	private: System::Windows::Forms::DateTimePicker^ dateTimePicker_endDate;
	private: System::Windows::Forms::Label^ label_startDate;
	private: System::Windows::Forms::Label^ label_EndDate;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^ chart_CandlestickChart;
	private: System::Windows::Forms::HScrollBar^ hScrollBar1;
	private: System::Windows::Forms::ComboBox^ comboBox_upwardWaves;
	private: System::Windows::Forms::ComboBox^ comboBox_downwardWaves;
	private: System::Windows::Forms::Label^ label_upwardComboBox;
	private: System::Windows::Forms::Label^ label_downwardComboBox;






	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea2 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series4 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			this->openFileDialog_LoadTicker = (gcnew System::Windows::Forms::OpenFileDialog());
			this->button_LoadTicker = (gcnew System::Windows::Forms::Button());
			this->dateTimePicker_startDate = (gcnew System::Windows::Forms::DateTimePicker());
			this->dateTimePicker_endDate = (gcnew System::Windows::Forms::DateTimePicker());
			this->label_startDate = (gcnew System::Windows::Forms::Label());
			this->label_EndDate = (gcnew System::Windows::Forms::Label());
			this->chart_CandlestickChart = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->hScrollBar1 = (gcnew System::Windows::Forms::HScrollBar());
			this->comboBox_upwardWaves = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox_downwardWaves = (gcnew System::Windows::Forms::ComboBox());
			this->label_upwardComboBox = (gcnew System::Windows::Forms::Label());
			this->label_downwardComboBox = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chart_CandlestickChart))->BeginInit();
			this->SuspendLayout();
			// 
			// openFileDialog_LoadTicker
			// 
			this->openFileDialog_LoadTicker->FileName = L"openFileDialog_LoadTicker";
			this->openFileDialog_LoadTicker->Multiselect = true;
			this->openFileDialog_LoadTicker->FileOk += gcnew System::ComponentModel::CancelEventHandler(this, &Form_UI::openFileDialog_LoadTicker_FileOk);
			// 
			// button_LoadTicker
			// 
			this->button_LoadTicker->Location = System::Drawing::Point(566, 609);
			this->button_LoadTicker->Name = L"button_LoadTicker";
			this->button_LoadTicker->Size = System::Drawing::Size(91, 35);
			this->button_LoadTicker->TabIndex = 0;
			this->button_LoadTicker->Text = L"Load Ticker";
			this->button_LoadTicker->UseVisualStyleBackColor = true;
			this->button_LoadTicker->Click += gcnew System::EventHandler(this, &Form_UI::button_LoadTicker_Click);
			// 
			// dateTimePicker_startDate
			// 
			this->dateTimePicker_startDate->Location = System::Drawing::Point(32, 624);
			this->dateTimePicker_startDate->Name = L"dateTimePicker_startDate";
			this->dateTimePicker_startDate->Size = System::Drawing::Size(200, 20);
			this->dateTimePicker_startDate->TabIndex = 3;
			this->dateTimePicker_startDate->Value = System::DateTime(2024, 1, 1, 0, 0, 0, 0);
			// 
			// dateTimePicker_endDate
			// 
			this->dateTimePicker_endDate->Location = System::Drawing::Point(322, 624);
			this->dateTimePicker_endDate->Name = L"dateTimePicker_endDate";
			this->dateTimePicker_endDate->Size = System::Drawing::Size(200, 20);
			this->dateTimePicker_endDate->TabIndex = 4;
			// 
			// label_startDate
			// 
			this->label_startDate->AutoSize = true;
			this->label_startDate->Location = System::Drawing::Point(29, 608);
			this->label_startDate->Name = L"label_startDate";
			this->label_startDate->Size = System::Drawing::Size(55, 13);
			this->label_startDate->TabIndex = 5;
			this->label_startDate->Text = L"Start Date";
			// 
			// label_EndDate
			// 
			this->label_EndDate->AutoSize = true;
			this->label_EndDate->Location = System::Drawing::Point(319, 608);
			this->label_EndDate->Name = L"label_EndDate";
			this->label_EndDate->Size = System::Drawing::Size(52, 13);
			this->label_EndDate->TabIndex = 6;
			this->label_EndDate->Text = L"End Date";
			// 
			// chart_CandlestickChart
			// 
			chartArea1->Name = L"Chart_OHLC";
			chartArea2->AlignWithChartArea = L"Chart_OHLC";
			chartArea2->Name = L"Chart_Volume";
			this->chart_CandlestickChart->ChartAreas->Add(chartArea1);
			this->chart_CandlestickChart->ChartAreas->Add(chartArea2);
			legend1->BackColor = System::Drawing::Color::White;
			legend1->Enabled = false;
			legend1->LegendStyle = System::Windows::Forms::DataVisualization::Charting::LegendStyle::Column;
			legend1->Name = L"Legend1";
			legend1->ShadowOffset = 3;
			this->chart_CandlestickChart->Legends->Add(legend1);
			this->chart_CandlestickChart->Location = System::Drawing::Point(0, 0);
			this->chart_CandlestickChart->Name = L"chart_CandlestickChart";
			series1->ChartArea = L"Chart_OHLC";
			series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Candlestick;
			series1->CustomProperties = L"PriceDownColor=Red, PriceUpColor=ForestGreen";
			series1->Legend = L"Legend1";
			series1->Name = L"Series_OHLC";
			series1->XValueMember = L"Timestamp";
			series1->XValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::DateTime;
			series1->YValueMembers = L"High,Low,Open,Close";
			series1->YValuesPerPoint = 4;
			series2->ChartArea = L"Chart_Volume";
			series2->IsXValueIndexed = true;
			series2->Legend = L"Legend1";
			series2->Name = L"Series_Volume";
			series2->XValueMember = L"Timestamp";
			series2->XValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Date;
			series2->YValueMembers = L"Volume";
			series3->ChartArea = L"Chart_OHLC";
			series3->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Point;
			series3->Color = System::Drawing::Color::Lime;
			series3->Legend = L"Legend1";
			series3->MarkerSize = 8;
			series3->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Diamond;
			series3->Name = L"Peaks";
			series4->ChartArea = L"Chart_OHLC";
			series4->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Point;
			series4->Color = System::Drawing::Color::Red;
			series4->Legend = L"Legend1";
			series4->MarkerSize = 8;
			series4->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Diamond;
			series4->Name = L"Valleys";
			this->chart_CandlestickChart->Series->Add(series1);
			this->chart_CandlestickChart->Series->Add(series2);
			this->chart_CandlestickChart->Series->Add(series3);
			this->chart_CandlestickChart->Series->Add(series4);
			this->chart_CandlestickChart->Size = System::Drawing::Size(747, 536);
			this->chart_CandlestickChart->TabIndex = 7;
			this->chart_CandlestickChart->Text = L"chart1";
			// 
			// hScrollBar1
			// 
			this->hScrollBar1->LargeChange = 1;
			this->hScrollBar1->Location = System::Drawing::Point(32, 580);
			this->hScrollBar1->Maximum = 5;
			this->hScrollBar1->Minimum = 1;
			this->hScrollBar1->Name = L"hScrollBar1";
			this->hScrollBar1->Size = System::Drawing::Size(490, 17);
			this->hScrollBar1->TabIndex = 8;
			this->hScrollBar1->Value = 1;
			this->hScrollBar1->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &Form_UI::hScrollBar1_Scroll);
			// 
			// comboBox_upwardWaves
			// 
			this->comboBox_upwardWaves->FormattingEnabled = true;
			this->comboBox_upwardWaves->Location = System::Drawing::Point(784, 56);
			this->comboBox_upwardWaves->Name = L"comboBox_upwardWaves";
			this->comboBox_upwardWaves->Size = System::Drawing::Size(244, 21);
			this->comboBox_upwardWaves->TabIndex = 9;
			this->comboBox_upwardWaves->SelectedIndexChanged += gcnew System::EventHandler(this, &Form_UI::comboBox_upwardWaves_SelectedIndexChanged);
			// 
			// comboBox_downwardWaves
			// 
			this->comboBox_downwardWaves->FormattingEnabled = true;
			this->comboBox_downwardWaves->Location = System::Drawing::Point(787, 354);
			this->comboBox_downwardWaves->Name = L"comboBox_downwardWaves";
			this->comboBox_downwardWaves->Size = System::Drawing::Size(244, 21);
			this->comboBox_downwardWaves->TabIndex = 10;
			this->comboBox_downwardWaves->SelectedIndexChanged += gcnew System::EventHandler(this, &Form_UI::comboBox_downwardWaves_SelectedIndexChanged);
			// 
			// label_upwardComboBox
			// 
			this->label_upwardComboBox->AutoSize = true;
			this->label_upwardComboBox->Location = System::Drawing::Point(784, 37);
			this->label_upwardComboBox->Name = L"label_upwardComboBox";
			this->label_upwardComboBox->Size = System::Drawing::Size(81, 13);
			this->label_upwardComboBox->TabIndex = 11;
			this->label_upwardComboBox->Text = L"Upward Waves";
			// 
			// label_downwardComboBox
			// 
			this->label_downwardComboBox->AutoSize = true;
			this->label_downwardComboBox->Location = System::Drawing::Point(787, 326);
			this->label_downwardComboBox->Name = L"label_downwardComboBox";
			this->label_downwardComboBox->Size = System::Drawing::Size(95, 13);
			this->label_downwardComboBox->TabIndex = 12;
			this->label_downwardComboBox->Text = L"Downward Waves";
			// 
			// Form_UI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1043, 656);
			this->Controls->Add(this->label_downwardComboBox);
			this->Controls->Add(this->comboBox_downwardWaves);
			this->Controls->Add(this->comboBox_upwardWaves);
			this->Controls->Add(this->hScrollBar1);
			this->Controls->Add(this->chart_CandlestickChart);
			this->Controls->Add(this->label_EndDate);
			this->Controls->Add(this->label_startDate);
			this->Controls->Add(this->dateTimePicker_endDate);
			this->Controls->Add(this->dateTimePicker_startDate);
			this->Controls->Add(this->button_LoadTicker);
			this->Controls->Add(this->label_upwardComboBox);
			this->Name = L"Form_UI";
			this->Text = L"Form_UI";
			this->Load += gcnew System::EventHandler(this, &Form_UI::Form_UI_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->chart_CandlestickChart))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button_LoadTicker_Click(System::Object^ sender, System::EventArgs^ e) {
		// Bring up the OpenFileDialogue File Picker
		openFileDialog_LoadTicker->ShowDialog();
	}
	private: System::Void openFileDialog_LoadTicker_FileOk(System::Object^ sender, System::ComponentModel::CancelEventArgs^ e) {
		//textBox_FileSelected->Text = openFileDialog_LoadTicker->FileName;

		loadTicker(openFileDialog_LoadTicker->FileNames[0]);

		for (int i = 1; i < openFileDialog_LoadTicker->FileNames->Length; ++i) {
			Form_UI^ newForm = gcnew Form_UI(openFileDialog_LoadTicker->FileNames[i], dateTimePicker_startDate->Value, dateTimePicker_endDate->Value);
		}

		candlesticks = loadTicker(openFileDialog_LoadTicker->FileName);
	}

	// Normalize the OHLC Chart Area to improve user experience and get better visuals
	private: System::Void normalizeChart(List<Candlestick^>^ candlestickList) {

		int chart_max = candlestickList[0]->High; // The max value of our candlesticks will be stored in chart_max
		int chart_min = candlestickList[0]->Low; // The min value of our candlestics will be stored in chart_min
		int padding = 10; // Add a little bit of padding to the min and max for better visual appeal

		for each (Candlestick ^candle in candlestickList)
		{
			// Update the chart maximum value
			if (candle->High > chart_max) { chart_max = candle->High; }

			// Update the chart minimum value
			if (candle->Low < chart_min) { chart_min = candle->Low; }
		}

		// Select the Chart_OHLC area from our chart and update the minimum and maximum values of it
		chart_CandlestickChart->ChartAreas->FindByName("Chart_OHLC")->AxisY->Minimum = chart_min - padding;
		chart_CandlestickChart->ChartAreas->FindByName("Chart_OHLC")->AxisY->Maximum = chart_max + padding;
	}

	// Filter the candlesticks so that only the ones within the specified date range show up
	private: List<Candlestick^>^ filterCandlesticksByDate(List<Candlestick^>^ listOfCandlesticks, DateTime startDate, DateTime endDate) {
		List<Candlestick^>^ filteredCandlesticks = gcnew List<Candlestick^>();

		// This loop goes through all the candlesticks and adds the ones that are within our range to the filtered list
		for each (Candlestick ^candle in listOfCandlesticks)
		{
			if (candle->Timestamp >= startDate && candle->Timestamp <= endDate)
			{
				filteredCandlesticks->Add(candle); // Add the candlestick to our filtered list
			}

			// Exit the loop early when no more candlesticks can be added
			if (candle->Timestamp >= endDate) {
				break;
			}
		}

		// return the filtered candlesticks
		return filteredCandlesticks;
	}

	private: List<PeakValley^>^ generatePeaksAndValleys(List<Candlestick^>^ candles)
	{
		List<PeakValley^>^ result = gcnew List<PeakValley^>();

		if (candles == nullptr || candles->Count < 3)
			return result;

		array<bool>^ isPeak = gcnew array<bool>(candles->Count);
		array<bool>^ isValley = gcnew array<bool>(candles->Count);

		for (int i = 1; i < candles->Count - 1; ++i)
		{
			double prevHigh = candles[i - 1]->High;
			double nextHigh = candles[i + 1]->High;
			double currentHigh = candles[i]->High;

			double prevLow = candles[i - 1]->Low;
			double nextLow = candles[i + 1]->Low;
			double currentLow = candles[i]->Low;

			if (currentHigh > prevHigh && currentHigh > nextHigh)
				isPeak[i] = true;

			if (currentLow < prevLow && currentLow < nextLow)
				isValley[i] = true;
		}

		for (int i = 1; i < candles->Count - 1; ++i)
		{
			if (isPeak[i])
			{
				int leftMargin = 0;
				for (int j = i - 1; j >= 0; --j)
				{
					if (isPeak[j]) break;
					++leftMargin;
				}

				int rightMargin = 0;
				for (int j = i + 1; j < candles->Count; ++j)
				{
					if (isPeak[j]) break;
					++rightMargin;
				}

				result->Add(gcnew PeakValley(i, true, false, leftMargin, rightMargin, candles[i]->Timestamp));
			}

			if (isValley[i])
			{
				int leftMargin = 0;
				for (int j = i - 1; j >= 0; --j)
				{
					if (isValley[j]) break;
					++leftMargin;
				}

				int rightMargin = 0;
				for (int j = i + 1; j < candles->Count; ++j)
				{
					if (isValley[j]) break;
					++rightMargin;
				}

				result->Add(gcnew PeakValley(i, false, true, leftMargin, rightMargin, candles[i]->Timestamp));
			}
		}

		return result;
	}

	private: List<PeakValley^>^ createMarginList(List<PeakValley^>^ PVList, int margin) {
		List<PeakValley^>^ newPVList = gcnew List<PeakValley^>();

		for each(PeakValley^ peakValley in PVList)
		{
			if (peakValley->MarginLeft > margin && peakValley->MarginRight > margin)
			{
				newPVList->Add(peakValley);
			}
		}
		return newPVList;
	}

	// Returen False if comparing two peaks or two valleys; then, return True if the wave is valid and False if it is not
	private: bool isValidWave(PeakValley^ rightPV, PeakValley^ leftPV) {
		if ((rightPV->IsPeak && leftPV->IsPeak) || (rightPV->IsValley && leftPV->IsValley)) return false;

		// Validate wave by checking (index of right - index of left) < marginLeft of rightPV & marginRight of leftPV
		int indexDifference = rightPV->index - leftPV->index;
		if (indexDifference < rightPV->MarginLeft && indexDifference < leftPV->MarginRight) return true;

		return false;
	}
	
	// For each Peak/Valley object in the list, loop through every other Peak or Valley and determine if it creates a valid wave;
	// if it does, add that to the newWaveList. Return the newWaveList at the end of the operation
	private: List<Wave^>^ createWaveList(List<PeakValley^>^ pvList) {
		List<Wave^>^ newWaveList = gcnew List<Wave^>();

		for (int i = pvList->Count-1; i >= 0; i--)
		{
			for (int j = i; j >= 0; j--)
			{
				if (i == j) continue; // Don't check with itself
				if (isValidWave(pvList[i], pvList[j]) == true) {
					newWaveList->Add(gcnew Wave(pvList[i], pvList[j])); // Wave itself sorts if it is a downward or upward wave
				}
			}
		}

		return newWaveList;
	}

	private: Dictionary<int, List<PeakValley^>^>^ createMarginMap(List<PeakValley^>^ allPVList) {
		Dictionary<int, List<PeakValley^>^>^ marginMap = gcnew Dictionary<int, List<PeakValley^>^>();
		marginMap[1] = allPVList;

		for (int i = 2; i < (hScrollBar1->Maximum + 1); i++)
		{
			marginMap[i] = createMarginList(marginMap[1], i);
		}

		return marginMap;
	}

	// Populate WaveMap with valid waves for each margin
	private: Dictionary<int, List<Wave^>^>^ createWaveMap() {
		Dictionary<int, List<Wave^>^>^ waveMap = gcnew Dictionary<int, List<Wave^>^>();

		for (int i = 1; i < finalMarginMap->Count; i++)
		{
			waveMap[i] = createWaveList(finalMarginMap[i]);
		}

		return waveMap;
	}

	private: void updateComboBoxes(int margin) {
		comboBox_upwardWaves->Items->Clear();
		comboBox_downwardWaves->Items->Clear();

		if (!finalWaveMap->ContainsKey(margin)) return;

		for each (Wave^ wave in finalWaveMap[margin])
		{
			if (wave->direction == 0)
			{
				comboBox_upwardWaves->Items->Add(wave);
			}
			else {
				comboBox_downwardWaves->Items->Add(wave);
			}
		}
	}

	private: void updatePeakValleyAnnotations(int margin) {
		double offset = 5; // Adjust this value based on your chart's scale

		for each(PeakValley ^ pv in this->finalMarginMap[margin])
		{
			DataPoint^ dp = chart_CandlestickChart->Series["Series_OHLC"]->Points[pv->index];
			double xValue = dp->XValue;

			if (pv->IsPeak)
			{
				double y = dp->YValues[0] + offset; // High + offset
				chart_CandlestickChart->Series["Peaks"]->Points->AddXY(xValue, y);
			}

			if (pv->IsValley)
			{
				double y = dp->YValues[1] - offset; // Low - offset
				chart_CandlestickChart->Series["Valleys"]->Points->AddXY(xValue, y);
			}
		}
	}

	private: void drawWave(Wave^ wave) {
		int startIdx = wave->pv2->index;
		int endIdx = wave->pv1->index;
		int direction = wave->direction;

		startIdx = 5;    // test code
		endIdx = 10;      // test code

		double xStart = chart_CandlestickChart->Series["Series_OHLC"]->Points[startIdx]->XValue;
		double xEnd = chart_CandlestickChart->Series["Series_OHLC"]->Points[endIdx]->XValue;

		List<PeakValley^>^ currentPVList = finalMarginMap[hScrollBar1->Value];
		
		double yLow = direction == 0 ? filteredCandlesticks[startIdx]->Low : filteredCandlesticks[endIdx]->Low;
		double yHigh = direction == 0 ? filteredCandlesticks[startIdx]->High : filteredCandlesticks[endIdx]->High;

		// test code start
		for (int i = startIdx; i <= endIdx; i++) {
			DataPoint^ dp = chart_CandlestickChart->Series["Series_OHLC"]->Points[i];
			double low = dp->YValues[3];   // Low
			double high = dp->YValues[2];  // High

			if (low < yLow) yLow = low;
			if (high > yHigh) yHigh = high;
		}
		// test code end

		// Create the rectangle annotation
		RectangleAnnotation^ rect = gcnew RectangleAnnotation();
		rect->AxisX = chart_CandlestickChart->ChartAreas[0]->AxisX;
		rect->AxisY = chart_CandlestickChart->ChartAreas[0]->AxisY;

		rect->X = xStart;
		rect->Y = yHigh; // top
		rect->Width = xEnd - xStart;
		rect->Height = yHigh - yLow;

		rect->LineColor = direction == 0 ? Color::Green : Color::Red;
		rect->BackColor = direction == 0 ? Color::FromArgb(60, Color::Green) : Color::FromArgb(60, Color::Red); // semi-transparent fill
		rect->LineDashStyle = ChartDashStyle::Dash;

		chart_CandlestickChart->Annotations->Add(rect);
	}

	// Take a comboBoxSelection or wave, and for each wave, draw a rectangle annotation from the first end to
		   // the last end point. Downward waves are colored red and upward waves are colored green.

	private: void updateWaveAnnotations() {
		Wave^ upwardWave = dynamic_cast<Wave^>(comboBox_upwardWaves->SelectedItem);
		Wave^ downwardWave = dynamic_cast<Wave^>(comboBox_downwardWaves->SelectedItem);

		if (upwardWave != nullptr) {
			drawWave(upwardWave);
		}

		if (downwardWave != nullptr) {
			drawWave(downwardWave);
		}
	}

	private: List<Candlestick^>^ loadTicker(String^ fileName) {
		// Read the file, create a list of candlesticks from it and store that in candlesticks
		candlesticks = StockReader::ReadFromCSV(fileName);
		display(fileName);
		return candlesticks;
	}

	private: List<Candlestick^>^ display(String^ fileName) {

		// Reverse the list if necessary (beacuse we need the list from old date to newer date)
		if (candlesticks[0]->Timestamp > candlesticks[1]->Timestamp) {
			candlesticks->Reverse();
		}

		// Filtering the candlesticks based on the date we want to display
		this->filteredCandlesticks = filterCandlesticksByDate(candlesticks, dateTimePicker_startDate->Value, dateTimePicker_endDate->Value);

		// ** NEW ** Code for calculating peaks and valleys
		// create a peak valley class, iterate through the list of filtered candlesticks, and return a list of peaks
		// and valleys
		List<PeakValley^>^ allPeaksAndValleys = generatePeaksAndValleys(filteredCandlesticks);

		this->finalMarginMap = createMarginMap(allPeaksAndValleys);
		this->finalWaveMap = createWaveMap();


		// DISPLAYING THE INFORMATION

		// Create a binding list
		BindingList <Candlestick^>^ bindingList = gcnew BindingList<Candlestick^>(filteredCandlesticks);

		// Normalize the chart, setting the min and max height according to the min and max of the candlesticks we display
		normalizeChart(filteredCandlesticks);

		// Add a title
		chart_CandlestickChart->Titles->Clear(); // Clear the previous titles
		chart_CandlestickChart->Titles->Add(fileName); // Add the new title

		// Connecting it to the Chart
		chart_CandlestickChart->DataSource = bindingList; // Set the data scource of the chart as the bindingList
		chart_CandlestickChart->DataBind(); // DataBind function completes the binding

		// Adding annotations
		updatePeakValleyAnnotations(hScrollBar1->Value);
		updateComboBoxes(hScrollBar1->Value);
		updateWaveAnnotations();


		return (filteredCandlesticks);
	}

private: System::Void Form_UI_Load(System::Object^ sender, System::EventArgs^ e) {
	return;
	
	this->WindowState = FormWindowState::Maximized;

	openFileDialog_LoadTicker->ShowDialog();
}
private: System::Void hScrollBar1_Scroll(System::Object^ sender, System::Windows::Forms::ScrollEventArgs^ e) {
	// Clear old annotations
	chart_CandlestickChart->Annotations->Clear();

	// Also clear old peaks/valleys markers!
	chart_CandlestickChart->Series["Peaks"]->Points->Clear();
	chart_CandlestickChart->Series["Valleys"]->Points->Clear();

	// Update markers for the new margin value
	updatePeakValleyAnnotations(hScrollBar1->Value);

	updateComboBoxes(hScrollBar1->Value);

	// Redraw the chart
	chart_CandlestickChart->Invalidate();
	chart_CandlestickChart->Update();
}
private: System::Void comboBox_upwardWaves_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	updateWaveAnnotations();
}
private: System::Void comboBox_downwardWaves_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	updateWaveAnnotations();
}
};
}