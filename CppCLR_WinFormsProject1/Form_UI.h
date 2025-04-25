#pragma once
#include "StockReader.h"
#include "PeakValley.h"
#include "Wave.h"
#include "RubberBandSelection.h"

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

		RubberBandSelection^ rubberBandSelection;
		RectangleAnnotation^ selectionRectAnnotation; // For the main rectangle
		LineAnnotation^ diagonalLineAnnotation;       // For the diagonal line
		List<HorizontalLineAnnotation^>^ fibLineAnnotations; // For Fib lines
		List<TextAnnotation^>^ fibLabelAnnotations;      // Optional: For Fib labels

		// Define Fibonacci levels (consider making static const if appropriate)
		static array<double>^ fibLevels = gcnew array<double>{ 0.0, 0.236, 0.382, 0.50, 0.618, 0.764, 1.0 };
	private: System::Windows::Forms::TextBox^ textBox_confirmations;

	private: System::Windows::Forms::Label^ label_confirmations;

		   static array<String^>^ fibLevelStrings = gcnew array<String^>{ "0.0%", "23.6%", "38.2%", "50.0%", "61.8%", "76.4%", "100.0%" };

	public:
		Form_UI(void) { //
			InitializeComponent(); //
			rubberBandSelection = gcnew RubberBandSelection(); //
			InitializeAnnotations(); // Call the new initializer
		}

		Form_UI(String^ filepath, DateTime startDate, DateTime endDate) //
		{
			InitializeComponent(); //
			this->filePath = filepath; //
			dateTimePicker_startDate->Value = startDate; //
			candlesticks = StockReader::ReadFromCSV(filepath); //
			rubberBandSelection = gcnew RubberBandSelection(); //
			InitializeAnnotations(); // Call the new initializer
			display(filepath); //
			this->Text = "Stock Price Analyzer"; //
			Show(); //
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

	private: void InitializeAnnotations() {
		// --- Remove previous instances if re-initializing ---
		RemoveAnnotationByName("RubberBandSelectionAnnotation");
		RemoveAnnotationByName("RubberBandDiagonal");
		for (int i = 0; i < fibLevels->Length; ++i) {
			RemoveAnnotationByName("FibLine_" + i);
			RemoveAnnotationByName("FibLabel_" + i);
		}

		ChartArea^ chartArea = nullptr;
		if (chart_CandlestickChart->ChartAreas->Count > 0) {
			chartArea = chart_CandlestickChart->ChartAreas[0];
		}
		else {
			// Cannot initialize annotations without a chart area
			return;
		}

		// --- Initialize Selection Rectangle ---
		selectionRectAnnotation = gcnew RectangleAnnotation();
		selectionRectAnnotation->Name = "RubberBandSelectionAnnotation";
		selectionRectAnnotation->LineColor = System::Drawing::Color::Red;
		selectionRectAnnotation->LineWidth = 1;
		selectionRectAnnotation->LineDashStyle = ChartDashStyle::Dot;
		selectionRectAnnotation->BackColor = System::Drawing::Color::FromArgb(30, System::Drawing::Color::LightSkyBlue);
		selectionRectAnnotation->IsSizeAlwaysRelative = false;
		selectionRectAnnotation->AxisX = chartArea->AxisX; // Use Axis Coords
		selectionRectAnnotation->AxisY = chartArea->AxisY; // Use Axis Coords
		selectionRectAnnotation->Visible = false;
		selectionRectAnnotation->ClipToChartArea = chartArea->Name;
		chart_CandlestickChart->Annotations->Add(selectionRectAnnotation);

		// --- Initialize Diagonal Line ---
		diagonalLineAnnotation = gcnew LineAnnotation();
		diagonalLineAnnotation->Name = "RubberBandDiagonal";
		diagonalLineAnnotation->LineColor = Color::DarkGray;
		diagonalLineAnnotation->LineWidth = 1;
		diagonalLineAnnotation->LineDashStyle = ChartDashStyle::Dash;
		diagonalLineAnnotation->IsSizeAlwaysRelative = false;
		diagonalLineAnnotation->AxisX = chartArea->AxisX; // Use Axis Coords
		diagonalLineAnnotation->AxisY = chartArea->AxisY; // Use Axis Coords
		diagonalLineAnnotation->Visible = false;
		diagonalLineAnnotation->ClipToChartArea = chartArea->Name;
		chart_CandlestickChart->Annotations->Add(diagonalLineAnnotation);

		// --- Initialize Fibonacci Lines & Labels ---
		fibLineAnnotations = gcnew List<HorizontalLineAnnotation^>();
		fibLabelAnnotations = gcnew List<TextAnnotation^>();

		for (int i = 0; i < fibLevels->Length; ++i) {
			// Line
			HorizontalLineAnnotation^ hLine = gcnew HorizontalLineAnnotation();
			hLine->Name = "FibLine_" + i;
			hLine->LineColor = Color::DarkSlateBlue;
			hLine->LineWidth = 1;
			hLine->LineDashStyle = ChartDashStyle::Dot;
			hLine->IsSizeAlwaysRelative = false;
			hLine->AxisX = chartArea->AxisX; // Use Axis Coords
			hLine->AxisY = chartArea->AxisY; // Use Axis Coords
			hLine->Visible = false;
			hLine->ClipToChartArea = chartArea->Name;
			chart_CandlestickChart->Annotations->Add(hLine);
			fibLineAnnotations->Add(hLine);

			// Optional Label
			TextAnnotation^ tLabel = gcnew TextAnnotation();
			tLabel->Name = "FibLabel_" + i;
			tLabel->Text = fibLevelStrings[i];
			tLabel->ForeColor = Color::DarkSlateBlue;
			tLabel->Font = gcnew System::Drawing::Font("Arial", 7);
			tLabel->IsSizeAlwaysRelative = false;
			tLabel->AxisX = chartArea->AxisX; // Use Axis Coords
			tLabel->AxisY = chartArea->AxisY; // Use Axis Coords
			tLabel->Visible = false;
			tLabel->ClipToChartArea = chartArea->Name;
			// Position label slightly to the right of the start of the line
			tLabel->AnchorAlignment = ContentAlignment::MiddleLeft;
			chart_CandlestickChart->Annotations->Add(tLabel);
			fibLabelAnnotations->Add(tLabel);
		}
	}

		   // *** NEW: Helper to remove annotation by name safely ***
		   void RemoveAnnotationByName(String^ name) {
			   Annotation^ ann = chart_CandlestickChart->Annotations->FindByName(name);
			   if (ann != nullptr) {
				   chart_CandlestickChart->Annotations->Remove(ann);
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
	private: System::Windows::Forms::Button^ button_updateButton;






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
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea3 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea4 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series5 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series6 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series7 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series8 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
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
			this->button_updateButton = (gcnew System::Windows::Forms::Button());
			this->textBox_confirmations = (gcnew System::Windows::Forms::TextBox());
			this->label_confirmations = (gcnew System::Windows::Forms::Label());
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
			this->button_LoadTicker->Location = System::Drawing::Point(566, 568);
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
			chartArea3->Name = L"Chart_OHLC";
			chartArea4->AlignWithChartArea = L"Chart_OHLC";
			chartArea4->Name = L"Chart_Volume";
			chartArea4->Visible = false;
			this->chart_CandlestickChart->ChartAreas->Add(chartArea3);
			this->chart_CandlestickChart->ChartAreas->Add(chartArea4);
			legend2->BackColor = System::Drawing::Color::White;
			legend2->Enabled = false;
			legend2->LegendStyle = System::Windows::Forms::DataVisualization::Charting::LegendStyle::Column;
			legend2->Name = L"Legend1";
			legend2->ShadowOffset = 3;
			this->chart_CandlestickChart->Legends->Add(legend2);
			this->chart_CandlestickChart->Location = System::Drawing::Point(0, 0);
			this->chart_CandlestickChart->Name = L"chart_CandlestickChart";
			series5->ChartArea = L"Chart_OHLC";
			series5->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Candlestick;
			series5->CustomProperties = L"PriceDownColor=Red, PriceUpColor=ForestGreen";
			series5->Legend = L"Legend1";
			series5->Name = L"Series_OHLC";
			series5->XValueMember = L"Timestamp";
			series5->XValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::DateTime;
			series5->YValueMembers = L"High,Low,Open,Close";
			series5->YValuesPerPoint = 4;
			series6->ChartArea = L"Chart_Volume";
			series6->IsXValueIndexed = true;
			series6->Legend = L"Legend1";
			series6->Name = L"Series_Volume";
			series6->XValueMember = L"Timestamp";
			series6->XValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Date;
			series6->YValueMembers = L"Volume";
			series7->ChartArea = L"Chart_OHLC";
			series7->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Point;
			series7->Color = System::Drawing::Color::Lime;
			series7->Legend = L"Legend1";
			series7->MarkerSize = 8;
			series7->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Diamond;
			series7->Name = L"Peaks";
			series8->ChartArea = L"Chart_OHLC";
			series8->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Point;
			series8->Color = System::Drawing::Color::Red;
			series8->Legend = L"Legend1";
			series8->MarkerSize = 8;
			series8->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Diamond;
			series8->Name = L"Valleys";
			this->chart_CandlestickChart->Series->Add(series5);
			this->chart_CandlestickChart->Series->Add(series6);
			this->chart_CandlestickChart->Series->Add(series7);
			this->chart_CandlestickChart->Series->Add(series8);
			this->chart_CandlestickChart->Size = System::Drawing::Size(747, 536);
			this->chart_CandlestickChart->TabIndex = 7;
			this->chart_CandlestickChart->Text = L"chart1";
			this->chart_CandlestickChart->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form_UI::chart_CandlestickChart_MouseDown);
			this->chart_CandlestickChart->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form_UI::chart_CandlestickChart_MouseMove);
			this->chart_CandlestickChart->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &Form_UI::chart_CandlestickChart_MouseUp);
			// 
			// hScrollBar1
			// 
			this->hScrollBar1->LargeChange = 1;
			this->hScrollBar1->Location = System::Drawing::Point(32, 568);
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
			// button_updateButton
			// 
			this->button_updateButton->Location = System::Drawing::Point(566, 609);
			this->button_updateButton->Name = L"button_updateButton";
			this->button_updateButton->Size = System::Drawing::Size(91, 35);
			this->button_updateButton->TabIndex = 13;
			this->button_updateButton->Text = L"Update";
			this->button_updateButton->UseVisualStyleBackColor = true;
			this->button_updateButton->Click += gcnew System::EventHandler(this, &Form_UI::button_updateButton_Click);
			// 
			// textBox_confirmations
			// 
			this->textBox_confirmations->Location = System::Drawing::Point(787, 624);
			this->textBox_confirmations->Name = L"textBox_confirmations";
			this->textBox_confirmations->Size = System::Drawing::Size(100, 20);
			this->textBox_confirmations->TabIndex = 14;
			// 
			// label_confirmations
			// 
			this->label_confirmations->AutoSize = true;
			this->label_confirmations->Location = System::Drawing::Point(787, 590);
			this->label_confirmations->Name = L"label_confirmations";
			this->label_confirmations->Size = System::Drawing::Size(70, 13);
			this->label_confirmations->TabIndex = 15;
			this->label_confirmations->Text = L"Confirmations";
			// 
			// Form_UI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1043, 656);
			this->Controls->Add(this->label_confirmations);
			this->Controls->Add(this->textBox_confirmations);
			this->Controls->Add(this->button_updateButton);
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


	// Generates all possible peaks and valleys in the given set of candlesticks
	private: List<PeakValley^>^ generatePeaksAndValleys(List<Candlestick^>^ candles) {
		List<PeakValley^>^ result = gcnew List<PeakValley^>();

		int n = candles->Count;
		if (candles == nullptr || n < 3)
			return result;

		for (int i = 1; i < n - 1; ++i) {
			double prevHigh = candles[i - 1]->High;
			double currHigh = candles[i]->High;
			double nextHigh = candles[i + 1]->High;

			double prevLow = candles[i - 1]->Low;
			double currLow = candles[i]->Low;
			double nextLow = candles[i + 1]->Low;

			bool isPeak = (currHigh > prevHigh && currHigh > nextHigh);
			bool isValley = (currLow < prevLow && currLow < nextLow);

			if (isPeak) {
				int marginLeft = 0;
				for (int j = i - 1; j >= 0; --j) {
					marginLeft++;
					if (candles[j]->High > currHigh) break;
				}
				if (marginLeft == i)
					marginLeft = i;

				int marginRight = 0;
				for (int j = i + 1; j < n; ++j) {
					marginRight++;
					if (candles[j]->High > currHigh) break;
				}
				if (marginRight == n - i - 1)
					marginRight = n - i - 1;

				result->Add(gcnew PeakValley(i, true, false, marginLeft, marginRight, candles[i]->Timestamp));
			}

			if (isValley) {
				int marginLeft = 0;
				for (int j = i - 1; j >= 0; --j) {
					marginLeft++;
					if (candles[j]->Low < currLow) break;
				}
				if (marginLeft == i)
					marginLeft = i;

				int marginRight = 0;
				for (int j = i + 1; j < n; ++j) {
					marginRight++;
					if (candles[j]->Low < currLow) break;
				}
				if (marginRight == n - i - 1)
					marginRight = n - i - 1;

				result->Add(gcnew PeakValley(i, false, true, marginLeft, marginRight, candles[i]->Timestamp));
			}
		}

		return result;
	}

	// Takes a Peak/Valley list and creates another list from it, based on the margin set by the user
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

		for (int i = pvList->Count - 1; i >= 0; i--) {
			for (int j = i - 1; j >= 0; j--) {
				if (i == j) continue;

				PeakValley^ later = pvList[i];
				PeakValley^ earlier = pvList[j];

				if (later->index < earlier->index) {
					// swap to ensure later comes after earlier
					PeakValley^ temp = later;
					later = earlier;
					earlier = temp;
				}

				if (isValidWave(later, earlier)) {
					newWaveList->Add(gcnew Wave(later, earlier)); // Wave figures out direction
				}
			}
		}

		return newWaveList;
	}

	// Uses the margin lists to construct a marginMap that includes all the lists of margins available to us
	private: Dictionary<int, List<PeakValley^>^>^ createMarginMap(List<PeakValley^>^ allPVList) {
		Dictionary<int, List<PeakValley^>^>^ marginMap = gcnew Dictionary<int, List<PeakValley^>^>();
		marginMap[1] = allPVList;

		for (int i = 2; i < (hScrollBar1->Maximum + 1); i++)
		{
			marginMap[i] = createMarginList(marginMap[1], i);
		}

		return marginMap;
	}

	// Populate WaveMap with valid waves for each margin; essentially mapping the dictionary of lists to a map of waves
	private: Dictionary<int, List<Wave^>^>^ createWaveMap() {
		Dictionary<int, List<Wave^>^>^ waveMap = gcnew Dictionary<int, List<Wave^>^>();

		for (int i = 1; i <= finalMarginMap->Count; i++)
		{
			waveMap[i] = createWaveList(finalMarginMap[i]);
		}

		return waveMap;
	}

	// Update the combo boxes with the new waves that correspond to the new margin set by the scroll bar
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

	// Update the peak/valley list (annotations) that are displayed on the chart in real-time, based on the margin
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

	// The actual function that uses RectangleAnnotation to draw the wave
	private: void drawWave(Wave^ wave) {
		// Clear existing rectangle annotations
		for (int i = chart_CandlestickChart->Annotations->Count - 1; i >= 0; i--) {
			Annotation^ annotation = chart_CandlestickChart->Annotations[i];
			if (dynamic_cast<RectangleAnnotation^>(annotation) != nullptr) {
				chart_CandlestickChart->Annotations->RemoveAt(i);
			}
		}

		int idx1 = wave->pv1->index;
		int idx2 = wave->pv2->index;
		int startIdx = Math::Min(idx1, idx2);
		int endIdx = Math::Max(idx1, idx2);
		int direction = wave->direction;

		// Get X values (timestamp-based axis)
		double xStart = chart_CandlestickChart->Series["Series_OHLC"]->Points[startIdx]->XValue;
		double xEnd = chart_CandlestickChart->Series["Series_OHLC"]->Points[endIdx]->XValue;

		// Get Y values
		double yStart = direction == 0 ? filteredCandlesticks[startIdx]->Low : filteredCandlesticks[startIdx]->High;
		double yEnd = direction == 0 ? filteredCandlesticks[endIdx]->High : filteredCandlesticks[endIdx]->Low;

		// Determine rectangle bounds
		double top = Math::Max(yStart, yEnd);
		double bottom = Math::Min(yStart, yEnd);

		double x = Math::Min(xStart, xEnd);
		double width = xEnd - xStart;
		double height = top - bottom;  // Correct height (no duplication!)

		// Create and configure rectangle annotation
		RectangleAnnotation^ rect = gcnew RectangleAnnotation();
		rect->AnchorDataPoint = chart_CandlestickChart->Series["Series_OHLC"]->Points[0];
  		rect->IsSizeAlwaysRelative = false;
		/*rect->ClipToChartArea = "Chart_OHLC";
		rect->AxisX = chart_CandlestickChart->ChartAreas["Chart_OHLC"]->AxisX;
		rect->AxisY = chart_CandlestickChart->ChartAreas["Chart_OHLC"]->AxisY;*/

		rect->X = x;
		rect->Y = top; // Set to the top of the rectangle
		rect->Width = width;
		rect->Height = -(top - bottom);

		rect->LineColor = direction == 0 ? Color::Green : Color::Red;
		rect->BackColor = direction == 0 ? Color::FromArgb(60, Color::Green) : Color::FromArgb(60, Color::Red);
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

	// store the filename for reuse later with Update button
	private: String^ currentFileName;

	// main function that runs when we click to load the stock
	private: List<Candlestick^>^ loadTicker(String^ fileName) {
		// Read the file, create a list of candlesticks from it and store that in candlesticks
		currentFileName = fileName; // store it for reuse later
		candlesticks = StockReader::ReadFromCSV(fileName);
		display(fileName);
		return candlesticks;
	}


	// The display function takes care of setting up the info and displaying everything on the chart
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
private: System::Void button_updateButton_Click(System::Object^ sender, System::EventArgs^ e) {
	try {
		chart_CandlestickChart->Series["Peaks"]->Points->Clear();
		chart_CandlestickChart->Series["Valleys"]->Points->Clear();
		chart_CandlestickChart->Annotations->Clear();

		this->filteredCandlesticks = filterCandlesticksByDate(candlesticks,
			dateTimePicker_startDate->Value,
			dateTimePicker_endDate->Value);

		if (filteredCandlesticks->Count < 3) {
			MessageBox::Show("Please select a wider range.",
				"Not enough points", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return;
		}

		List<PeakValley^>^ allPeaksAndValleys = generatePeaksAndValleys(filteredCandlesticks);

		this->finalMarginMap = createMarginMap(allPeaksAndValleys);
		this->finalWaveMap = createWaveMap();


		BindingList<Candlestick^>^ bindingList = gcnew BindingList<Candlestick^>(filteredCandlesticks);

		normalizeChart(filteredCandlesticks);

		chart_CandlestickChart->DataSource = bindingList;
		chart_CandlestickChart->DataBind();

		if (hScrollBar1->Value > finalMarginMap->Count) {
			hScrollBar1->Value = Math::Min(hScrollBar1->Value, finalMarginMap->Count);
		}

		if (finalMarginMap->ContainsKey(hScrollBar1->Value)) {
			updatePeakValleyAnnotations(hScrollBar1->Value);
		}

		// Update combo boxes
		if (finalWaveMap->ContainsKey(hScrollBar1->Value)) {
			updateComboBoxes(hScrollBar1->Value);
		}

		// Clear combo boxes
		comboBox_upwardWaves->SelectedIndex = -1;
		comboBox_downwardWaves->SelectedIndex = -1;
	}
	catch (System::IndexOutOfRangeException^ ex) {
		MessageBox::Show("Error: " + ex->Message + "\nTry a different date range.",
			"Out of Range", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (System::Exception^ ex) {
		MessageBox::Show("error: " + ex->Message,
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
}

private: void UpdateSelectionVisuals()
{
	// Ensure necessary objects exist (Removed IsSelecting check here)
	if (rubberBandSelection == nullptr ||
		selectionRectAnnotation == nullptr || diagonalLineAnnotation == nullptr ||
		fibLineAnnotations == nullptr || fibLabelAnnotations == nullptr ||
		chart_CandlestickChart->ChartAreas->Count == 0)
	{
		// Cannot proceed if essential objects are missing
		// Ensure everything is hidden if we exit early
		if (selectionRectAnnotation != nullptr) selectionRectAnnotation->Visible = false;
		if (diagonalLineAnnotation != nullptr) diagonalLineAnnotation->Visible = false;
		if (fibLineAnnotations != nullptr) {
			for each (HorizontalLineAnnotation ^ line in fibLineAnnotations) if (line != nullptr) line->Visible = false;
		}
		if (fibLabelAnnotations != nullptr) {
			for each (TextAnnotation ^ label in fibLabelAnnotations) if (label != nullptr) label->Visible = false;
		}
		return;
	}

	// Get chart area and axes
	ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
	Axis^ axisX = chartArea->AxisX;
	Axis^ axisY = chartArea->AxisY;

	// Get pixel coordinates from rubber band selection
	Point startPt = rubberBandSelection->StartPoint;
	Point endPt = rubberBandSelection->EndPoint;

	// If start and end points are the same, nothing to draw yet. Hide and return.
	if (startPt.Equals(endPt)) {
		selectionRectAnnotation->Visible = false;
		diagonalLineAnnotation->Visible = false;
		for each (HorizontalLineAnnotation ^ line in fibLineAnnotations) line->Visible = false;
		for each (TextAnnotation ^ label in fibLabelAnnotations) label->Visible = false;
		return;
	}

	// Convert pixel coordinates to axis values
	double startXVal, startYVal, endXVal, endYVal;
	try {
		startXVal = axisX->PixelPositionToValue(startPt.X);
		startYVal = axisY->PixelPositionToValue(startPt.Y);
		endXVal = axisX->PixelPositionToValue(endPt.X);
		endYVal = axisY->PixelPositionToValue(endPt.Y);
	}
	catch (ArgumentException^) { // Handle cases where pixels are outside axis area
		// Hide annotations and return if conversion fails
		selectionRectAnnotation->Visible = false;
		diagonalLineAnnotation->Visible = false;
		for each (HorizontalLineAnnotation ^ line in fibLineAnnotations) line->Visible = false;
		for each (TextAnnotation ^ label in fibLabelAnnotations) label->Visible = false;
		return;
	}

	// Determine axis bounds
	double minXVal = Math::Min(startXVal, endXVal);
	double maxXVal = Math::Max(startXVal, endXVal);
	double minYVal = Math::Min(startYVal, endYVal); // Min axis value (visually lower)
	double maxYVal = Math::Max(startYVal, endYVal); // Max axis value (visually higher)
	double widthX = maxXVal - minXVal;
	double heightY = maxYVal - minYVal;

	// Check for near-zero size due to double precision or conversion limits
	// Use a small epsilon based on axis range if needed, or just check > 0
	bool hasSize = (widthX > Double::Epsilon && heightY > Double::Epsilon);

	if (!hasSize) {
		// If no valid size, hide everything
		selectionRectAnnotation->Visible = false;
		diagonalLineAnnotation->Visible = false;
		for each (HorizontalLineAnnotation ^ line in fibLineAnnotations) line->Visible = false;
		for each (TextAnnotation ^ label in fibLabelAnnotations) label->Visible = false;
		return;
	}

	// --- Update Annotations ONLY if size is valid ---

	// --- Update Selection Rectangle (Axis Coords) ---
	selectionRectAnnotation->X = minXVal;
	selectionRectAnnotation->Y = maxYVal; // Y is still the 'anchor' top edge in axis coords
	selectionRectAnnotation->Width = widthX;
	// *** USE NEGATIVE HEIGHT FOR VISUAL CORRECTION ***
	selectionRectAnnotation->Height = -heightY; // Use negative height based on user observation
	selectionRectAnnotation->Visible = true;

	// --- Update Diagonal Line (Axis Coords) ---
	diagonalLineAnnotation->X = startXVal;
	diagonalLineAnnotation->Y = startYVal;
	diagonalLineAnnotation->Width = endXVal - startXVal;
	diagonalLineAnnotation->Height = endYVal - startYVal;
	diagonalLineAnnotation->Visible = true; // Make visible

	// --- Update Fibonacci Lines & Labels (Axis Coords) ---
	for (int i = 0; i < fibLevels->Length; ++i) {
		double fibYValue = minYVal + fibLevels[i] * heightY;

		// Update Line
		if (i < fibLineAnnotations->Count) {
			HorizontalLineAnnotation^ hLine = fibLineAnnotations[i];
			hLine->Y = fibYValue;
			hLine->X = minXVal;
			hLine->Width = widthX;
			hLine->Visible = true; // Make visible
		}

		// Update Label (Optional)
		if (i < fibLabelAnnotations->Count) {
			TextAnnotation^ tLabel = fibLabelAnnotations[i];
			tLabel->X = maxXVal - (axisX->Maximum - axisX->Minimum) * 0.01; // Offset slightly from right edge
			tLabel->Y = fibYValue;
			tLabel->Visible = true; // Make visible
		}
	}
}


private: System::Void chart_CandlestickChart_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
	if (rubberBandSelection == nullptr || chart_CandlestickChart->ChartAreas->Count == 0) return;

	// Hide annotations from previous selection
	if (selectionRectAnnotation != nullptr) selectionRectAnnotation->Visible = false;
	if (diagonalLineAnnotation != nullptr) diagonalLineAnnotation->Visible = false;
	if (fibLineAnnotations != nullptr) {
		for each(HorizontalLineAnnotation ^ line in fibLineAnnotations) line->Visible = false;
	}
	if (fibLabelAnnotations != nullptr) {
		for each(TextAnnotation ^ label in fibLabelAnnotations) label->Visible = false;
	}


	// Check if click is within plot area (Optional but recommended)
	ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
	// ... (rest of plot area check logic from previous example) ...
	// if (click is inside plot area) {
	rubberBandSelection->BeginSelection(e->Location);
	// }
}

private: System::Void chart_CandlestickChart_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
	if (rubberBandSelection != nullptr && rubberBandSelection->IsSelecting) {
		rubberBandSelection->UpdateSelection(e->Location);
		UpdateSelectionVisuals(); // Call the new update function
	}
}

private: System::Void chart_CandlestickChart_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
	if (rubberBandSelection != nullptr && rubberBandSelection->IsSelecting) {
		rubberBandSelection->EndSelection(e->Location);
		UpdateSelectionVisuals(); // Draw final state

		// --- Confirmation Check Logic (Modified Y-Bounds Calculation) ---
		if (selectionRectAnnotation != nullptr && // Added null check
			selectionRectAnnotation->Visible && selectionRectAnnotation->Width > 0 && Math::Abs(selectionRectAnnotation->Height) > 0 && // Check Abs(Height)
			filteredCandlesticks != nullptr && filteredCandlesticks->Count > 0 &&
			fibLineAnnotations != nullptr && chart_CandlestickChart->ChartAreas->Count > 0)
		{
			ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
			Axis^ axisX = chartArea->AxisX;
			Axis^ axisY = chartArea->AxisY;

			// Get final selection bounds in Axis coordinates
			double minXVal = selectionRectAnnotation->X;
			double maxXVal = selectionRectAnnotation->X + selectionRectAnnotation->Width;

			// *** MODIFIED Y BOUNDS CALCULATION ***
			// Calculate the two Y edges based on anchor Y and the (negative) Height
			double yEdge1 = selectionRectAnnotation->Y;
			double yEdge2 = selectionRectAnnotation->Y + selectionRectAnnotation->Height; // Height is negative here
			// Determine the actual min and max Y values represented by the rectangle
			double finalMinYVal = Math::Min(yEdge1, yEdge2);
			double finalMaxYVal = Math::Max(yEdge1, yEdge2);
			// *** END MODIFIED Y BOUNDS CALCULATION ***

			// Get final Fibonacci Y-Axis values (check visibility)
			List<double>^ fibYValues = gcnew List<double>();
			for each (HorizontalLineAnnotation ^ line in fibLineAnnotations) {
				if (line != nullptr && line->Visible) { // Added null check for line
					fibYValues->Add(line->Y);
				}
			}

			int confirmationCount = 0;
			double tolerance = 0.0; // Tolerance for touching
			if (axisY->Maximum > axisY->Minimum) {
				tolerance = (axisY->Maximum - axisY->Minimum) * 0.002; // 0.2% of visible Y range
			}
			if (tolerance <= 0) tolerance = 0.05; // Fallback

			// Iterate through visible candlesticks
			for each (Candlestick ^ candle in filteredCandlesticks) {
				if (candle == nullptr) continue;

				double candleXVal = candle->Timestamp.ToOADate();
				// Check if candle's X value is within the selection rectangle's X range
				if (candleXVal >= minXVal && candleXVal <= maxXVal) {
					bool touched = false;
					for each (double fibY in fibYValues) {
						// Use the correctly calculated finalMinYVal and finalMaxYVal here indirectly via tolerance check
						if ((Math::Abs(candle->High - fibY) <= tolerance) ||
							(Math::Abs(candle->Low - fibY) <= tolerance) ||
							(Math::Abs(candle->Open - fibY) <= tolerance) ||
							(Math::Abs(candle->Close - fibY) <= tolerance) ||
							// Check if fib level falls BETWEEN corrected High and Low
							(candle->Low < fibY && candle->High > fibY))
						{
							// Ensure the touch point is also within the visual rectangle bounds
							if (fibY >= finalMinYVal && fibY <= finalMaxYVal) {
								touched = true;
								break; // Move to next candle
							}
							// Handle touches at High/Low etc even if slightly outside tolerance
							else if ((Math::Abs(candle->High - fibY) <= tolerance && candle->High >= finalMinYVal && candle->High <= finalMaxYVal) ||
								(Math::Abs(candle->Low - fibY) <= tolerance && candle->Low >= finalMinYVal && candle->Low <= finalMaxYVal) ||
								(Math::Abs(candle->Open - fibY) <= tolerance && candle->Open >= finalMinYVal && candle->Open <= finalMaxYVal) ||
								(Math::Abs(candle->Close - fibY) <= tolerance && candle->Close >= finalMinYVal && candle->Close <= finalMaxYVal))
							{
								touched = true;
								break;
							}
						}
					}
					if (touched) {
						confirmationCount++;
					}
				}
			}

			// Display the count
			/*MessageBox::Show("Fibonacci Confirmations within selection: " + confirmationCount,
				"Confirmation Count", MessageBoxButtons::OK, MessageBoxIcon::Information);*/
			this->textBox_confirmations->Text = confirmationCount.ToString();
		}
	}
}
};
}