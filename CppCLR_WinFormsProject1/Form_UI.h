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
	private: System::Windows::Forms::Button^ button_StepUp;
	private: System::Windows::Forms::Button^ button_StepDown;


	private: System::Windows::Forms::Button^ button_SimulateStartStop;

	private: System::Windows::Forms::Timer^ simulationTimer;
	private: System::Windows::Forms::HScrollBar^ scrollBar_SimMinPercent;
	private: System::Windows::Forms::HScrollBar^ scrollBar_SimMaxPercent;
	private: System::Windows::Forms::Label^ label_SimMinPercentValue;
	private: System::Windows::Forms::Label^ label_SimMaxPercentValue;
	private: System::Windows::Forms::Label^ label_StepPercentage;
	private: System::Windows::Forms::TextBox^ textBox_StepSizePercent;


	static array<String^>^ fibLevelStrings = gcnew array<String^>{ "0.0%", "23.6%", "38.2%", "50.0%", "61.8%", "76.4%", "100.0%" };
	
	// *** Simulation and Stepping State Variables ***
	private:
		bool isSimulating;                  // Is the timer running?
		bool activeWaveDefined;             // Is there a wave selected/drawn to simulate/step?
		double activeWaveStartXVal;         // X-axis value of the starting point
		double activeWaveStartYVal;         // Y-axis value of the starting point (for diagonal)
		double activeWaveEndXVal;           // X-axis value of the ending point
		double activeWaveBaseEndYVal;       // Original Y-axis value of the ending point
		double activeWaveCurrentEndYVal;    // Current Y-axis value during simulation/stepping
		double simMinYVal;                  // Calculated Min Y for simulation range
		double simMaxYVal;                  // Calculated Max Y for simulation range
		double stepSizeAxisUnits;           // Step size calculated in axis units
		double currentSimDirection;         // +1 or -1 for simulation direction

	public:
		Form_UI(void) { //
			InitializeComponent(); //
			isSimulating = false;
			activeWaveDefined = false;
			// Disable controls initially (can also be done in designer)
			button_SimulateStartStop->Enabled = false;
			button_StepUp->Enabled = false;
			button_StepDown->Enabled = false;
			scrollBar_SimMinPercent->Enabled = false;
			scrollBar_SimMaxPercent->Enabled = false;
			textBox_StepSizePercent->Enabled = false;
			// Initialize labels for scrollbars
			label_SimMinPercentValue->Text = scrollBar_SimMinPercent->Value.ToString() + "%";
			label_SimMaxPercentValue->Text = scrollBar_SimMaxPercent->Value.ToString() + "%";
			rubberBandSelection = gcnew RubberBandSelection(); //
			InitializeAnnotations(); // Call the new initializer
		}

		Form_UI(String^ filepath, DateTime startDate, DateTime endDate) //
		{
			InitializeComponent(); //
			isSimulating = false;
			activeWaveDefined = false;
			// Disable controls initially (can also be done in designer)
			button_SimulateStartStop->Enabled = false;
			button_StepUp->Enabled = false;
			button_StepDown->Enabled = false;
			scrollBar_SimMinPercent->Enabled = false;
			scrollBar_SimMaxPercent->Enabled = false;
			textBox_StepSizePercent->Enabled = false;
			// Initialize labels for scrollbars
			label_SimMinPercentValue->Text = scrollBar_SimMinPercent->Value.ToString() + "%";
			label_SimMaxPercentValue->Text = scrollBar_SimMaxPercent->Value.ToString() + "%";
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

	void ClearRetracementVisuals() {
		// Stop simulation if it's running when visuals are cleared
		if (isSimulating) {
			StopSimulation();
		}
		// Reset active wave state
		activeWaveDefined = false;

		// Hide annotations and clear markers (existing code)
		try {
			if (selectionRectAnnotation != nullptr) selectionRectAnnotation->Visible = false;
			if (diagonalLineAnnotation != nullptr) diagonalLineAnnotation->Visible = false;
			// ... (rest of annotation hiding) ...
			if (chart_CandlestickChart != nullptr && chart_CandlestickChart->Series != nullptr && chart_CandlestickChart->Series->IndexOf("Confirmations") != -1) {
				chart_CandlestickChart->Series["Confirmations"]->Points->Clear();
			}
		}
		catch (Exception^ ex) {
			System::Diagnostics::Debug::WriteLine("Error in ClearRetracementVisuals: " + ex->Message);
		}

		// Disable simulation/stepping controls as no wave is active
		button_SimulateStartStop->Enabled = false;
		button_StepUp->Enabled = false;
		button_StepDown->Enabled = false;
		scrollBar_SimMinPercent->Enabled = false;
		scrollBar_SimMaxPercent->Enabled = false;
		textBox_StepSizePercent->Enabled = false;
	}

	// *** NEW: Calculates Confirmations and Plots Markers ***
	private: int CalculateAndShowConfirmations(double minXVal, double maxXVal, double minYVal, double maxYVal)
	{
		int confirmationCount = 0;
		// --- Clear previous markers ---
		Series^ confirmationSeries = nullptr;
		// Defensive checks for chart and series objects
		if (chart_CandlestickChart != nullptr && chart_CandlestickChart->Series != nullptr && chart_CandlestickChart->Series->IndexOf("Confirmations") != -1) {
			confirmationSeries = chart_CandlestickChart->Series["Confirmations"];
			confirmationSeries->Points->Clear();
		}
		else {
			System::Diagnostics::Debug::WriteLine("Warning: 'Confirmations' series not found for plotting markers.");
			// Cannot plot, but can still calculate count if data is available
		}

		// --- Check necessary data ---
		if (filteredCandlesticks == nullptr || filteredCandlesticks->Count == 0 ||
			fibLineAnnotations == nullptr || chart_CandlestickChart == nullptr || chart_CandlestickChart->ChartAreas->Count == 0)
		{
			// Update TextBox to 0 if we can't calculate
			if (textBox_confirmations != nullptr) {
				textBox_confirmations->Text = "0";
			}
			return 0; // Cannot calculate
		}

		ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
		Axis^ axisY = chartArea->AxisY;
		if (axisY == nullptr) { // Axis null check
			if (textBox_confirmations != nullptr) textBox_confirmations->Text = "0";
			return 0;
		}


		// --- Get visible Fibonacci Y-Axis values ---
		List<double>^ fibYValues = gcnew List<double>();
		for each (HorizontalLineAnnotation ^ line in fibLineAnnotations) {
			// Also check line is not null itself
			if (line != nullptr && line->Visible) {
				fibYValues->Add(line->Y);
			}
		}
		if (fibYValues->Count == 0) {
			if (textBox_confirmations != nullptr) textBox_confirmations->Text = "0";
			return 0; // No levels visible
		}


		// --- Calculate tolerance ---
		double tolerance = 0.0;
		try { // Add try block for safety when accessing axis properties
			if (axisY->Maximum > axisY->Minimum && !Double::IsNaN(axisY->Maximum) && !Double::IsNaN(axisY->Minimum) &&
				!Double::IsInfinity(axisY->Maximum) && !Double::IsInfinity(axisY->Minimum))
			{
				tolerance = (axisY->Maximum - axisY->Minimum) * 0.002; // 0.2%
			}
		}
		catch (Exception^ ex) {
			// Log error if axis properties fail
			System::Diagnostics::Debug::WriteLine("Error accessing axis properties for tolerance: " + ex->Message);
		}
		if (tolerance <= 0 || Double::IsNaN(tolerance) || Double::IsInfinity(tolerance)) {
			tolerance = 0.05; // Fallback small absolute value
		}


		// --- Iterate through candlesticks ---
		for each (Candlestick ^ candle in filteredCandlesticks) {
			if (candle == nullptr) continue;

			double candleXVal = candle->Timestamp.ToOADate();
			// Check X range
			if (candleXVal >= minXVal && candleXVal <= maxXVal) {
				// Check Y range against Fib levels
				for each (double fibY in fibYValues) {
					// Ensure Fib level itself is within the effective Y bounds of the selection
					if (fibY >= minYVal && fibY <= maxYVal) {
						bool touched = false;
						double touchY = Double::NaN;

						// Check High, Low, Open, Close within tolerance
						if (Math::Abs(candle->High - fibY) <= tolerance) { touched = true; touchY = candle->High; }
						else if (Math::Abs(candle->Low - fibY) <= tolerance) { touched = true; touchY = candle->Low; }
						else if (Math::Abs(candle->Open - fibY) <= tolerance) { touched = true; touchY = candle->Open; }
						else if (Math::Abs(candle->Close - fibY) <= tolerance) { touched = true; touchY = candle->Close; }
						// Check if Fib level is within the candle body/wick (High/Low range)
						else if (candle->Low <= fibY && candle->High >= fibY) { touched = true; touchY = fibY; }

						if (touched) {
							confirmationCount++;
							// Add marker ONLY if series exists and touch coordinate is valid
							if (confirmationSeries != nullptr && !Double::IsNaN(touchY)) {
								// Add marker safely
								try {
									confirmationSeries->Points->AddXY(candleXVal, touchY);
								}
								catch (Exception^ ex) {
									System::Diagnostics::Debug::WriteLine("Error adding confirmation marker: " + ex->Message);
								}
							}
							break; // Count only one confirmation per candle
						}
					} // end fibY bounds check
				} // end fibY loop
			} // end X range check
		} // end candle loop

		// *** RETURN the count ***
		// The caller will update the TextBox
		return confirmationCount;
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
private: System::ComponentModel::IContainer^ components;






	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^ chartArea2 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^ legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^ series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series4 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^ series5 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
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
			this->button_StepUp = (gcnew System::Windows::Forms::Button());
			this->button_StepDown = (gcnew System::Windows::Forms::Button());
			this->button_SimulateStartStop = (gcnew System::Windows::Forms::Button());
			this->simulationTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->scrollBar_SimMinPercent = (gcnew System::Windows::Forms::HScrollBar());
			this->scrollBar_SimMaxPercent = (gcnew System::Windows::Forms::HScrollBar());
			this->label_SimMinPercentValue = (gcnew System::Windows::Forms::Label());
			this->label_SimMaxPercentValue = (gcnew System::Windows::Forms::Label());
			this->label_StepPercentage = (gcnew System::Windows::Forms::Label());
			this->textBox_StepSizePercent = (gcnew System::Windows::Forms::TextBox());
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
			this->dateTimePicker_startDate->Value = System::DateTime(2021, 2, 1, 0, 0, 0, 0);
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
			chartArea2->Visible = false;
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
			series3->Enabled = false;
			series3->Legend = L"Legend1";
			series3->MarkerSize = 8;
			series3->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Diamond;
			series3->Name = L"Peaks";
			series4->ChartArea = L"Chart_OHLC";
			series4->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Point;
			series4->Color = System::Drawing::Color::Red;
			series4->Enabled = false;
			series4->Legend = L"Legend1";
			series4->MarkerSize = 8;
			series4->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Diamond;
			series4->Name = L"Valleys";
			series5->ChartArea = L"Chart_OHLC";
			series5->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::Point;
			series5->Legend = L"Legend1";
			series5->MarkerColor = System::Drawing::Color::Fuchsia;
			series5->MarkerSize = 6;
			series5->MarkerStyle = System::Windows::Forms::DataVisualization::Charting::MarkerStyle::Circle;
			series5->Name = L"Confirmations";
			this->chart_CandlestickChart->Series->Add(series1);
			this->chart_CandlestickChart->Series->Add(series2);
			this->chart_CandlestickChart->Series->Add(series3);
			this->chart_CandlestickChart->Series->Add(series4);
			this->chart_CandlestickChart->Series->Add(series5);
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
			this->comboBox_downwardWaves->Location = System::Drawing::Point(787, 239);
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
			this->label_downwardComboBox->Location = System::Drawing::Point(784, 214);
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
			this->textBox_confirmations->Location = System::Drawing::Point(860, 390);
			this->textBox_confirmations->Name = L"textBox_confirmations";
			this->textBox_confirmations->Size = System::Drawing::Size(100, 20);
			this->textBox_confirmations->TabIndex = 14;
			// 
			// label_confirmations
			// 
			this->label_confirmations->AutoSize = true;
			this->label_confirmations->Location = System::Drawing::Point(784, 393);
			this->label_confirmations->Name = L"label_confirmations";
			this->label_confirmations->Size = System::Drawing::Size(70, 13);
			this->label_confirmations->TabIndex = 15;
			this->label_confirmations->Text = L"Confirmations";
			// 
			// button_StepUp
			// 
			this->button_StepUp->Enabled = false;
			this->button_StepUp->Location = System::Drawing::Point(774, 428);
			this->button_StepUp->Name = L"button_StepUp";
			this->button_StepUp->Size = System::Drawing::Size(49, 47);
			this->button_StepUp->TabIndex = 16;
			this->button_StepUp->Text = L"+";
			this->button_StepUp->UseVisualStyleBackColor = true;
			this->button_StepUp->Click += gcnew System::EventHandler(this, &Form_UI::button_StepUp_Click);
			// 
			// button_StepDown
			// 
			this->button_StepDown->Enabled = false;
			this->button_StepDown->Location = System::Drawing::Point(841, 428);
			this->button_StepDown->Name = L"button_StepDown";
			this->button_StepDown->Size = System::Drawing::Size(49, 47);
			this->button_StepDown->TabIndex = 17;
			this->button_StepDown->Text = L"-";
			this->button_StepDown->UseVisualStyleBackColor = true;
			this->button_StepDown->Click += gcnew System::EventHandler(this, &Form_UI::button_StepDown_Click);
			// 
			// button_SimulateStartStop
			// 
			this->button_SimulateStartStop->Location = System::Drawing::Point(911, 428);
			this->button_SimulateStartStop->Name = L"button_SimulateStartStop";
			this->button_SimulateStartStop->Size = System::Drawing::Size(49, 47);
			this->button_SimulateStartStop->TabIndex = 18;
			this->button_SimulateStartStop->Text = L"Start";
			this->button_SimulateStartStop->UseVisualStyleBackColor = true;
			this->button_SimulateStartStop->Click += gcnew System::EventHandler(this, &Form_UI::button_SimulateStartStop_Click);
			// 
			// simulationTimer
			// 
			this->simulationTimer->Tick += gcnew System::EventHandler(this, &Form_UI::simulationTimer_Tick);
			// 
			// scrollBar_SimMinPercent
			// 
			this->scrollBar_SimMinPercent->Enabled = false;
			this->scrollBar_SimMinPercent->Location = System::Drawing::Point(860, 519);
			this->scrollBar_SimMinPercent->Name = L"scrollBar_SimMinPercent";
			this->scrollBar_SimMinPercent->Size = System::Drawing::Size(80, 17);
			this->scrollBar_SimMinPercent->TabIndex = 19;
			this->scrollBar_SimMinPercent->Value = 5;
			this->scrollBar_SimMinPercent->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &Form_UI::scrollBar_SimPercent_Scroll);
			// 
			// scrollBar_SimMaxPercent
			// 
			this->scrollBar_SimMaxPercent->Enabled = false;
			this->scrollBar_SimMaxPercent->Location = System::Drawing::Point(860, 568);
			this->scrollBar_SimMaxPercent->Name = L"scrollBar_SimMaxPercent";
			this->scrollBar_SimMaxPercent->Size = System::Drawing::Size(80, 17);
			this->scrollBar_SimMaxPercent->TabIndex = 20;
			this->scrollBar_SimMaxPercent->Value = 5;
			this->scrollBar_SimMaxPercent->Scroll += gcnew System::Windows::Forms::ScrollEventHandler(this, &Form_UI::scrollBar_SimPercent_Scroll);
			// 
			// label_SimMinPercentValue
			// 
			this->label_SimMinPercentValue->AutoSize = true;
			this->label_SimMinPercentValue->Location = System::Drawing::Point(777, 519);
			this->label_SimMinPercentValue->Name = L"label_SimMinPercentValue";
			this->label_SimMinPercentValue->Size = System::Drawing::Size(27, 13);
			this->label_SimMinPercentValue->TabIndex = 21;
			this->label_SimMinPercentValue->Text = L"10%";
			// 
			// label_SimMaxPercentValue
			// 
			this->label_SimMaxPercentValue->AutoSize = true;
			this->label_SimMaxPercentValue->Location = System::Drawing::Point(777, 568);
			this->label_SimMaxPercentValue->Name = L"label_SimMaxPercentValue";
			this->label_SimMaxPercentValue->Size = System::Drawing::Size(27, 13);
			this->label_SimMaxPercentValue->TabIndex = 22;
			this->label_SimMaxPercentValue->Text = L"10%";
			// 
			// label_StepPercentage
			// 
			this->label_StepPercentage->AutoSize = true;
			this->label_StepPercentage->Location = System::Drawing::Point(780, 609);
			this->label_StepPercentage->Name = L"label_StepPercentage";
			this->label_StepPercentage->Size = System::Drawing::Size(49, 13);
			this->label_StepPercentage->TabIndex = 23;
			this->label_StepPercentage->Text = L"Step (%):";
			// 
			// textBox_StepSizePercent
			// 
			this->textBox_StepSizePercent->Enabled = false;
			this->textBox_StepSizePercent->Location = System::Drawing::Point(860, 605);
			this->textBox_StepSizePercent->Name = L"textBox_StepSizePercent";
			this->textBox_StepSizePercent->Size = System::Drawing::Size(100, 20);
			this->textBox_StepSizePercent->TabIndex = 24;
			this->textBox_StepSizePercent->Text = L"1.0";
			this->textBox_StepSizePercent->TextChanged += gcnew System::EventHandler(this, &Form_UI::textBox_StepSizePercent_TextChanged);
			// 
			// Form_UI
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1043, 656);
			this->Controls->Add(this->textBox_StepSizePercent);
			this->Controls->Add(this->label_StepPercentage);
			this->Controls->Add(this->label_SimMaxPercentValue);
			this->Controls->Add(this->label_SimMinPercentValue);
			this->Controls->Add(this->scrollBar_SimMaxPercent);
			this->Controls->Add(this->scrollBar_SimMinPercent);
			this->Controls->Add(this->button_SimulateStartStop);
			this->Controls->Add(this->button_StepDown);
			this->Controls->Add(this->button_StepUp);
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

	/*
	** Obsolete Function ** 
	
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
		// rect->ClipToChartArea = "Chart_OHLC";
		// rect->AxisX = chart_CandlestickChart->ChartAreas["Chart_OHLC"]->AxisX;
		// rect->AxisY = chart_CandlestickChart->ChartAreas["Chart_OHLC"]->AxisY;

		rect->X = x;
		rect->Y = top; // Set to the top of the rectangle
		rect->Width = width;
		rect->Height = -(top - bottom);

		rect->LineColor = direction == 0 ? Color::Green : Color::Red;
		rect->BackColor = direction == 0 ? Color::FromArgb(60, Color::Green) : Color::FromArgb(60, Color::Red);
		rect->LineDashStyle = ChartDashStyle::Dash;

		chart_CandlestickChart->Annotations->Add(rect);
	}
	*/
	
	//// Take a comboBoxSelection or wave, and for each wave, draw a rectangle annotation from the first end to
	//// the last end point. Downward waves are colored red and upward waves are colored green.
	//private: void updateWaveAnnotations() {
	//	Wave^ upwardWave = dynamic_cast<Wave^>(comboBox_upwardWaves->SelectedItem);
	//	Wave^ downwardWave = dynamic_cast<Wave^>(comboBox_downwardWaves->SelectedItem);

	//	if (upwardWave != nullptr) {
	//		drawWave(upwardWave);
	//	}

	//	if (downwardWave != nullptr) {
	//		drawWave(downwardWave);
	//	}
	//}

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
	// *** Handles wave selection from either ComboBox ***
		   private: void HandleWaveSelection(Wave^ selectedWave) {
			   // Stop any ongoing simulation first
			   StopSimulation();
			   // Clear previous visuals immediately
			   ClearRetracementVisuals();

			   // --- Validate Input ---
			   if (selectedWave == nullptr || selectedWave->pv1 == nullptr || selectedWave->pv2 == nullptr ||
				   chart_CandlestickChart == nullptr || chart_CandlestickChart->Series == nullptr ||
				   chart_CandlestickChart->Series->IndexOf("Series_OHLC") == -1 ||
				   filteredCandlesticks == nullptr) {

				   activeWaveDefined = false; // Ensure flag is false if wave is invalid
				   // Keep controls disabled (already handled by ClearRetracementVisuals)
				   if (textBox_confirmations != nullptr) textBox_confirmations->Text = ""; // Clear text
				   return; // Cannot process if wave or data is invalid
			   }

			   int idx1 = selectedWave->pv1->index;
			   int idx2 = selectedWave->pv2->index;

			   Series^ ohlcSeries = chart_CandlestickChart->Series["Series_OHLC"];
			   int pointCount = ohlcSeries->Points->Count;
			   int filteredCount = filteredCandlesticks->Count;

			   // Validate indices against BOTH lists they are used for
			   if (idx1 < 0 || idx1 >= pointCount || idx2 < 0 || idx2 >= pointCount || // OHLC index check
				   idx1 >= filteredCount || idx2 >= filteredCount)                     // FilteredList index check
			   {
				   System::Diagnostics::Debug::WriteLine("Wave selection index out of bounds.");
				   activeWaveDefined = false; // Ensure flag is false if indices are invalid
				   // Keep controls disabled
				   if (textBox_confirmations != nullptr) textBox_confirmations->Text = "";
				   return;
			   }

			   // --- Get Coordinates ---
			   DataPoint^ dpStart = nullptr;
			   DataPoint^ dpEnd = nullptr;
			   double yVal1 = Double::NaN;
			   double yVal2 = Double::NaN;

			   try { // Add try-catch for safety when accessing points/data
				   dpStart = ohlcSeries->Points[idx1];
				   dpEnd = ohlcSeries->Points[idx2];

				   // Check if peak/valley information is reliable before accessing High/Low
				   if (selectedWave->pv1->IsPeak || selectedWave->pv1->IsValley) {
					   yVal1 = selectedWave->pv1->IsPeak ? filteredCandlesticks[idx1]->High : filteredCandlesticks[idx1]->Low;
				   }
				   else { throw gcnew InvalidOperationException("Wave PV1 is neither Peak nor Valley."); }

				   if (selectedWave->pv2->IsPeak || selectedWave->pv2->IsValley) {
					   yVal2 = selectedWave->pv2->IsPeak ? filteredCandlesticks[idx2]->High : filteredCandlesticks[idx2]->Low;
				   }
				   else { throw gcnew InvalidOperationException("Wave PV2 is neither Peak nor Valley."); }

				   // Check for NaN coordinates
				   if (dpStart == nullptr || dpEnd == nullptr || Double::IsNaN(yVal1) || Double::IsNaN(yVal2)) {
					   throw gcnew InvalidOperationException("Failed to retrieve valid wave coordinates.");
				   }

			   }
			   catch (Exception^ ex) {
				   System::Diagnostics::Debug::WriteLine("Error getting wave coordinates: " + ex->Message);
				   activeWaveDefined = false;
				   if (textBox_confirmations != nullptr) textBox_confirmations->Text = "";
				   return; // Exit if coordinates are invalid
			   }


			   // --- Store Active Wave Info ---
			   activeWaveDefined = true;
			   activeWaveStartXVal = dpStart->XValue;
			   activeWaveStartYVal = yVal1; // Start Y is defined by the first PV's value
			   activeWaveEndXVal = dpEnd->XValue;
			   activeWaveBaseEndYVal = yVal2; // Store the wave's defined end Y
			   activeWaveCurrentEndYVal = yVal2; // Initialize current Y

			   // --- Initial Draw ---
			   DrawFibRetracement(activeWaveStartXVal, activeWaveStartYVal, activeWaveEndXVal, activeWaveCurrentEndYVal);

			   // --- Calculate Bounds for Confirmation Check ---
			   double minXVal = Math::Min(activeWaveStartXVal, activeWaveEndXVal);
			   double maxXVal = Math::Max(activeWaveStartXVal, activeWaveEndXVal);
			   double minYVal = Math::Min(activeWaveStartYVal, activeWaveCurrentEndYVal); // Use absolute min/max
			   double maxYVal = Math::Max(activeWaveStartYVal, activeWaveCurrentEndYVal);

			   // --- Calculate, Show Confirmations, Update TextBox ---
			   // Ensure bounds are valid
			   if (!Double::IsNaN(minXVal) && !Double::IsNaN(maxXVal) && !Double::IsNaN(minYVal) && !Double::IsNaN(maxYVal))
			   {
				   int confirmationCount = CalculateAndShowConfirmations(minXVal, maxXVal, minYVal, maxYVal);
				   if (textBox_confirmations != nullptr) {
					   textBox_confirmations->Text = confirmationCount.ToString();
				   }
			   }
			   else {
				   if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error";
			   }

			   // --- Enable Controls ---
			   button_SimulateStartStop->Enabled = true;
			   button_StepUp->Enabled = true;
			   button_StepDown->Enabled = true;
			   scrollBar_SimMinPercent->Enabled = true;
			   scrollBar_SimMaxPercent->Enabled = true;
			   textBox_StepSizePercent->Enabled = true;
		   }

	// The display function takes care of setting up the info and displaying everything on the chart
	private: List<Candlestick^>^ display(String^ fileName) {

		// Reverse the list if necessary (beacuse we need the list from old date to newer date)
		if (candlesticks[0]->Timestamp > candlesticks[1]->Timestamp) {
			candlesticks->Reverse();
		}

		// Filtering the candlesticks based on the date we want to display
		this->filteredCandlesticks = filterCandlesticksByDate(candlesticks, dateTimePicker_startDate->Value, dateTimePicker_endDate->Value);

		// --- Clear previous data markers and WAVE annotations ONLY ---
		chart_CandlestickChart->Series["Peaks"]->Points->Clear(); //
		chart_CandlestickChart->Series["Valleys"]->Points->Clear(); //
		// Clear wave annotations explicitly if needed, or rely on selection logic
		for (int i = chart_CandlestickChart->Annotations->Count - 1; i >= 0; i--) { //
			Annotation^ annotation = chart_CandlestickChart->Annotations[i]; //
			if (annotation->Name != nullptr && annotation->Name->StartsWith("WaveAnnotation_")) { //
				chart_CandlestickChart->Annotations->RemoveAt(i); //
			}
		}
		// Also clear any existing retracement/selection visuals
		ClearRetracementVisuals();

		// Clear combo boxes before potentially repopulating
		comboBox_upwardWaves->Items->Clear();
		comboBox_downwardWaves->Items->Clear();
		comboBox_upwardWaves->Text = "";
		comboBox_downwardWaves->Text = "";

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
		// updateWaveAnnotations(); *** Obsolete Function ***


		return (filteredCandlesticks);
	}

		   // Inside Form_UI class

private: System::Void Form_UI_Load(System::Object^ sender, System::EventArgs^ e) {
	// --- Auto-load ABBV on startup ---

	// 1. Define the default file and directory
	String^ defaultFileName = "ABBV-Day.csv"; // Exact filename provided by user
	String^ initialDataDirectory = ""; // Initialize path variable

	try {
		// Get the directory where the application executable is located
		String^ exeDirectory = Application::StartupPath;
		// Get the parent directory (one level up from the executable's directory)
		System::IO::DirectoryInfo^ parentInfo = System::IO::Directory::GetParent(exeDirectory);

		if (parentInfo == nullptr) {
			throw gcnew System::IO::DirectoryNotFoundException("Could not determine the parent directory of the application.");
		}
		String^ parentDirectoryPath = parentInfo->FullName;

		// Combine the parent path with the "Stock Data" folder name
		initialDataDirectory = System::IO::Path::Combine(parentDirectoryPath, "Stock Data");

	}
	catch (Exception^ ex) {
		MessageBox::Show("Error determining initial data directory:\n" + ex->Message +
			"\nPlease select the file manually.",
			"Directory Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		// Default to application directory if path calculation fails, or leave blank
		initialDataDirectory = Application::StartupPath;
	}


	// 2. Configure the OpenFileDialog
	if (openFileDialog_LoadTicker != nullptr) // Null check for safety
	{
		openFileDialog_LoadTicker->InitialDirectory = initialDataDirectory;
		openFileDialog_LoadTicker->FileName = defaultFileName; // Pre-selects the file
		openFileDialog_LoadTicker->Filter = "CSV Files (*.csv)|*.csv|All Files (*.*)|*.*";
		openFileDialog_LoadTicker->Title = "Select Stock File (Default: ABBV)";

		// 3. Automatically show the dialog
		System::Windows::Forms::DialogResult result = openFileDialog_LoadTicker->ShowDialog();

		// 4. Process the result
		if (result == System::Windows::Forms::DialogResult::OK)
		{
			// Check if the selected file actually exists before loading
			String^ selectedFilePath = openFileDialog_LoadTicker->FileName;
			if (System::IO::File::Exists(selectedFilePath))
			{
				try
				{
					// Load the selected ticker (should be the default ABBV)
					loadTicker(selectedFilePath);
				}
				catch (Exception^ ex)
				{
					MessageBox::Show("Error loading initial file (" + System::IO::Path::GetFileName(selectedFilePath) + "):\n" + ex->Message,
						"File Load Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
				}
			}
			else
			{
				MessageBox::Show("Default file not found in the expected directory:\n" + selectedFilePath +
					"\nPlease select a file manually.",
					"File Not Found", MessageBoxButtons::OK, MessageBoxIcon::Warning);
				// Optionally trigger the load button click to immediately allow manual selection
				// button_LoadTicker_Click(nullptr, nullptr);
			}
		}
		else
		{
			// User cancelled the initial dialog
			MessageBox::Show("No file selected on startup. Use 'Load Ticker' button to load data.",
				"Startup Info", MessageBoxButtons::OK, MessageBoxIcon::Information);
		}
	}
	else {
		MessageBox::Show("OpenFileDialog component is not initialized.", "Initialization Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}

	// Date range should be set by designer or constructor based on previous user statement
}

private: System::Void hScrollBar1_Scroll(System::Object^ sender, System::Windows::Forms::ScrollEventArgs^ e) {
	// *** NEW LOGIC ***

	// 1. Clear previous retracement visuals (rectangle, lines, fibs, confirmations)
	ClearRetracementVisuals();

	// 2. Clear old peaks/valleys markers (done within updatePeakValleyAnnotations now)
	// chart_CandlestickChart->Series["Peaks"]->Points->Clear(); // No longer needed here
	// chart_CandlestickChart->Series["Valleys"]->Points->Clear(); // No longer needed here

	// 3. Update peak/valley markers for the new margin value (this clears old ones too)
	updatePeakValleyAnnotations(hScrollBar1->Value);

	// 4. Update ComboBox contents based on the new margin
	updateComboBoxes(hScrollBar1->Value);

	// 5. Do NOT automatically draw a wave from the combo box here.
	//    The user needs to click the combo box again if they want to see a wave
	//    for the new margin level.

	// 6. Remove unnecessary Invalidate/Update calls
	// chart_CandlestickChart->Invalidate(); // Generally not needed
	// chart_CandlestickChart->Update();     // Generally not needed
}

// Combo Box Event Handlers

private: System::Void comboBox_upwardWaves_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	HandleWaveSelection(dynamic_cast<Wave^>(comboBox_upwardWaves->SelectedItem));
	comboBox_downwardWaves->SelectedIndex = -1; // Clear other selection
}
private: System::Void comboBox_downwardWaves_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
	HandleWaveSelection(dynamic_cast<Wave^>(comboBox_downwardWaves->SelectedItem));
	comboBox_upwardWaves->SelectedIndex = -1; // Clear other selection
}


private: System::Void button_updateButton_Click(System::Object^ sender, System::EventArgs^ e) {
	if (String::IsNullOrEmpty(filePath)) {
		MessageBox::Show("Please load a ticker file first.", "No File Loaded", MessageBoxButtons::OK, MessageBoxIcon::Information);
		return;
	}

	try {
		// *** NEW LOGIC ***

		// 1. Clear previous retracement visuals (rectangle, lines, fibs, confirmations)
		ClearRetracementVisuals();

		// 2. Clear peak/valley markers (will be redrawn by display->updatePeakValleyAnnotations)
		if (chart_CandlestickChart->Series->IndexOf("Peaks") != -1)
			chart_CandlestickChart->Series["Peaks"]->Points->Clear();
		if (chart_CandlestickChart->Series->IndexOf("Valleys") != -1)
			chart_CandlestickChart->Series["Valleys"]->Points->Clear();

		// 3. DO NOT clear all annotations here:
		// chart_CandlestickChart->Annotations->Clear(); // REMOVED

		// --- Proceed with re-filtering and re-displaying ---

		// Ensure fresh read of original data
		candlesticks = StockReader::ReadFromCSV(filePath);

		// Call display, which handles filtering, calcs, markers, combo boxes, binding etc.
		display(System::IO::Path::GetFileName(filePath));

		// The display function now handles the logic below, so it can be removed from here:
		/*
		this->filteredCandlesticks = filterCandlesticksByDate(candlesticks,
			dateTimePicker_startDate->Value,
			dateTimePicker_endDate->Value);

		if (filteredCandlesticks == nullptr || filteredCandlesticks->Count < 3) { // Added null check
			MessageBox::Show("Please select a wider range.",
				"Not enough points", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			// Ensure chart is cleared if not enough points
			if(chart_CandlestickChart->Series->IndexOf("Series_OHLC") != -1)
				 chart_CandlestickChart->Series["Series_OHLC"]->Points->Clear();
			// Clear maps
			finalMarginMap = gcnew Dictionary<int, List<PeakValley^>^>();
			finalWaveMap = gcnew Dictionary<int, List<Wave^>^>();
			return;
		}

		List<PeakValley^>^ allPeaksAndValleys = generatePeaksAndValleys(filteredCandlesticks);
		this->finalMarginMap = createMarginMap(allPeaksAndValleys);
		this->finalWaveMap = createWaveMap();

		BindingList<Candlestick^>^ bindingList = gcnew BindingList<Candlestick^>(filteredCandlesticks);
		normalizeChart(filteredCandlesticks);
		chart_CandlestickChart->DataSource = bindingList;
		chart_CandlestickChart->DataBind();

		// Adjust scrollbar if needed (might be better placed after map creation checks)
		if (finalMarginMap != nullptr && hScrollBar1->Value > finalMarginMap->Count && finalMarginMap->Count > 0) {
			hScrollBar1->Value = finalMarginMap->Count; // Adjust to max valid margin
		} else if (finalMarginMap == nullptr || finalMarginMap->Count == 0) {
			hScrollBar1->Value = hScrollBar1->Minimum; // Reset if no margins
		}


		// Update markers and combo boxes (use the adjusted scrollbar value)
		int currentMargin = hScrollBar1->Value;
		if (finalMarginMap != nullptr && finalMarginMap->ContainsKey(currentMargin)) {
			updatePeakValleyAnnotations(currentMargin);
		}
		if (finalWaveMap != nullptr && finalWaveMap->ContainsKey(currentMargin)) {
			updateComboBoxes(currentMargin);
		}
		*/

		// Ensure combo boxes are cleared after update (display might repopulate, but selection should be cleared)
		comboBox_upwardWaves->SelectedIndex = -1;
		comboBox_downwardWaves->SelectedIndex = -1;
		comboBox_upwardWaves->Text = ""; // Also clear text
		comboBox_downwardWaves->Text = "";


	}
	catch (System::IO::FileNotFoundException^ ex) { // Specific exception
		MessageBox::Show("Error loading file: " + ex->Message, "File Not Found", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (System::IndexOutOfRangeException^ ex) { // Changed order
		MessageBox::Show("Error processing data: " + ex->Message + "\nCheck date range or file format.",
			"Processing Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
	catch (System::Exception^ ex) { // General exception last
		MessageBox::Show("An error occurred: " + ex->Message,
			"Error", MessageBoxButtons::OK, MessageBoxIcon::Error);
	}
}

// Helper methods for simulation
private:
	// --- Calculates absolute step size in axis units ---
	bool CalculateStepSize() {
		if (!activeWaveDefined) return false;

		double stepPercent;
		if (!Double::TryParse(textBox_StepSizePercent->Text, stepPercent) || stepPercent <= 0) {
			MessageBox::Show("Invalid Step Size percentage.", "Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			return false;
		}

		double totalHeight = Math::Abs(activeWaveBaseEndYVal - activeWaveStartYVal); // Use original height
		if (totalHeight <= Double::Epsilon) {
			// Avoid division by zero or tiny steps if initial wave has no height
			// Use a small absolute value based on axis range?
			if (chart_CandlestickChart->ChartAreas->Count > 0 && chart_CandlestickChart->ChartAreas[0]->AxisY != nullptr) {
				Axis^ axisY = chart_CandlestickChart->ChartAreas[0]->AxisY;
				stepSizeAxisUnits = (axisY->Maximum - axisY->Minimum) * 0.005; // 0.5% of axis range
			}
			else {
				stepSizeAxisUnits = 0.1; // Fallback absolute step
			}
		}
		else {
			stepSizeAxisUnits = totalHeight * (stepPercent / 100.0);
		}
		return stepSizeAxisUnits > Double::Epsilon; // Ensure step is positive
	}


	// --- Starts the simulation ---
	void StartSimulation() {
		if (!activeWaveDefined || !CalculateStepSize()) { // Ensure wave is defined and step size is valid
			StopSimulation(); // Make sure UI is in stopped state
			return;
		}

		// Calculate original wave height
		double totalHeight = Math::Abs(activeWaveBaseEndYVal - activeWaveStartYVal);

		// If totalHeight is zero, simulation doesn't make sense.
		if (totalHeight <= Double::Epsilon) {
			MessageBox::Show("Cannot simulate wave with zero vertical height.", "Simulation Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			StopSimulation(); // Ensure controls are re-enabled correctly
			return;
		}

		// --- *** MODIFIED BOUNDS CALCULATION *** ---
		// Calculate bounds RELATIVE to the original End Y Value (activeWaveBaseEndYVal)
		// Min % below EndY, Max % above EndY
		double minPercent = scrollBar_SimMinPercent->Value / 100.0;
		double maxPercent = scrollBar_SimMaxPercent->Value / 100.0;

		simMinYVal = activeWaveBaseEndYVal - (totalHeight * minPercent);
		simMaxYVal = activeWaveBaseEndYVal + (totalHeight * maxPercent);
		// --- *** END MODIFIED BOUNDS CALCULATION *** ---


		// Ensure Max >= Min visually (simulation still goes min to max)
		// Although calculated relative to EndY, the simulation range itself must be valid.
		// If maxPercent is 0% and minPercent is 10%, simMaxYVal would be < simMinYVal.
		// In such a case, we might clamp or default, but for now, let's just ensure the simulation
		// range itself is ordered correctly for the bouncing logic.
		if (simMaxYVal < simMinYVal) {
			// This could happen if Min% is large and Max% is small.
			// Swap them for the simulation bouncing logic to work,
			// although the percentages might feel counter-intuitive to the user.
			// Or, show an error and don't start. Let's show an error.
			MessageBox::Show("Max % range must result in a higher value than Min % range.", "Simulation Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			StopSimulation();
			return;

			// Alternative: Swap them to allow simulation anyway
			// double temp = simMaxYVal;
			// simMaxYVal = simMinYVal;
			// simMinYVal = temp;
		}
		// Avoid zero range for simulation
		if (Math::Abs(simMaxYVal - simMinYVal) < Double::Epsilon) {
			MessageBox::Show("Simulation range (Min % to Max %) results in zero height. Adjust percentages.", "Simulation Error", MessageBoxButtons::OK, MessageBoxIcon::Warning);
			StopSimulation();
			return;
		}


		// Initialize state
		isSimulating = true;
		activeWaveCurrentEndYVal = simMinYVal; // Start simulation from the calculated minimum Y bound
		currentSimDirection = 1.0;            // Start going up

		// Update UI
		button_SimulateStartStop->Text = "Stop";
		button_StepUp->Enabled = false;
		button_StepDown->Enabled = false;
		scrollBar_SimMinPercent->Enabled = false;
		scrollBar_SimMaxPercent->Enabled = false;
		textBox_StepSizePercent->Enabled = false;

		// Start Timer
		simulationTimer->Enabled = true; // Ensure timer is enabled before starting
		simulationTimer->Start();
	}

	// --- Stops the simulation ---
	void StopSimulation() {
		isSimulating = false;
		simulationTimer->Stop();
		simulationTimer->Enabled = false; // Good practice to disable

		// Update UI
		button_SimulateStartStop->Text = "Start";
		// Only enable stepping etc. if a wave is still defined
		bool enableControls = activeWaveDefined;
		button_StepUp->Enabled = enableControls;
		button_StepDown->Enabled = enableControls;
		scrollBar_SimMinPercent->Enabled = enableControls;
		scrollBar_SimMaxPercent->Enabled = enableControls;
		textBox_StepSizePercent->Enabled = enableControls;
	}


void DrawFibRetracement(double startXVal, double startYVal, double endXVal, double endYVal)
{
	// Ensure necessary objects exist
	if (selectionRectAnnotation == nullptr || diagonalLineAnnotation == nullptr ||
		fibLineAnnotations == nullptr || fibLabelAnnotations == nullptr ||
		chart_CandlestickChart->ChartAreas->Count == 0)
	{
		ClearRetracementVisuals(); // Hide everything if objects are missing
		return;
	}

	// Get chart area and axes
	ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
	Axis^ axisX = chartArea->AxisX;
	Axis^ axisY = chartArea->AxisY;

	// Determine axis bounds
	double minXVal = Math::Min(startXVal, endXVal);
	double maxXVal = Math::Max(startXVal, endXVal);
	double minYVal = Math::Min(startYVal, endYVal); // Absolute Min Y of the selection/wave
	double maxYVal = Math::Max(startYVal, endYVal); // Absolute Max Y of the selection/wave
	double widthX = maxXVal - minXVal;
	double heightY = maxYVal - minYVal; // Total vertical span

	// Check for near-zero size
	bool hasSize = (widthX > Double::Epsilon && heightY > Double::Epsilon);

	if (!hasSize) {
		ClearRetracementVisuals(); // Hide if no size
		return;
	}

	// --- Clear previous Confirmation Markers ---
	if (chart_CandlestickChart->Series->IndexOf("Confirmations") != -1) {
		chart_CandlestickChart->Series["Confirmations"]->Points->Clear();
	}

	// --- Update Selection Rectangle (Axis Coords) ---
	selectionRectAnnotation->X = minXVal;
	selectionRectAnnotation->Y = maxYVal; // Anchor at top Y value
	selectionRectAnnotation->Width = widthX;
	selectionRectAnnotation->Height = -heightY; // Use negative height for visual correction
	selectionRectAnnotation->Visible = true;

	// --- Update Diagonal Line (Axis Coords) ---
	// Connects the specific start/end points
	diagonalLineAnnotation->X = startXVal;
	diagonalLineAnnotation->Y = startYVal;
	diagonalLineAnnotation->Width = endXVal - startXVal;
	diagonalLineAnnotation->Height = endYVal - startYVal;
	diagonalLineAnnotation->Visible = true;

	// --- Update Fibonacci Lines & Labels (Axis Coords) ---
	for (int i = 0; i < fibLevels->Length; ++i) {
		// Calculate Y value based on the total vertical span (minYVal to maxYVal)
		double fibYValue = minYVal + fibLevels[i] * heightY;

		// Update Line
		if (i < fibLineAnnotations->Count && fibLineAnnotations[i] != nullptr) {
			HorizontalLineAnnotation^ hLine = fibLineAnnotations[i];
			hLine->Y = fibYValue;
			hLine->X = minXVal;   // Start at the left edge
			hLine->Width = widthX; // Span the width
			hLine->Visible = true;
		}

		// Update Label (Optional)
		if (i < fibLabelAnnotations->Count && fibLabelAnnotations[i] != nullptr) {
			TextAnnotation^ tLabel = fibLabelAnnotations[i];
			// Position label slightly inside the right edge
			tLabel->X = maxXVal - (axisX->Maximum - axisX->Minimum) * 0.01;
			tLabel->Y = fibYValue;
			tLabel->Visible = true;
		}
	}
}


private: System::Void chart_CandlestickChart_MouseDown(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
	if (rubberBandSelection == nullptr || chart_CandlestickChart->ChartAreas->Count == 0) return;

	// *** Clear All Previous Retracement Visuals on New Click ***
	ClearRetracementVisuals();

	// Check if click is within plot area (Optional but recommended)
	ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
	// ... (plot area check logic) ...
	// if (click is inside plot area) {
	rubberBandSelection->BeginSelection(e->Location);
	// }
}

private: System::Void chart_CandlestickChart_MouseMove(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
	if (rubberBandSelection != nullptr && rubberBandSelection->IsSelecting) {
		rubberBandSelection->UpdateSelection(e->Location);

		// --- Draw retracement based on current mouse drag (pixel to axis conversion) ---
		Point startPt = rubberBandSelection->StartPoint;
		Point endPt = rubberBandSelection->EndPoint;
		if (startPt.Equals(endPt)) { // Don't draw if no movement yet
			ClearRetracementVisuals(); // Ensure hidden
			return;
		}

		ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
		Axis^ axisX = chartArea->AxisX;
		Axis^ axisY = chartArea->AxisY;
		double startXVal, startYVal, endXVal, endYVal;
		try {
			startXVal = axisX->PixelPositionToValue(startPt.X);
			startYVal = axisY->PixelPositionToValue(startPt.Y);
			endXVal = axisX->PixelPositionToValue(endPt.X);
			endYVal = axisY->PixelPositionToValue(endPt.Y);

			// Call the unified drawing function
			DrawFibRetracement(startXVal, startYVal, endXVal, endYVal);
		}
		catch (ArgumentException^) {
			// Conversion failed (likely outside chart area), hide visuals
			ClearRetracementVisuals();
		}
	}
}

private: System::Void chart_CandlestickChart_MouseUp(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
	// Check if selection object exists
	if (rubberBandSelection == nullptr) return;

	// Check if we were actually selecting
	if (rubberBandSelection->IsSelecting) {
		rubberBandSelection->EndSelection(e->Location); // Sets IsSelecting = false

		Point startPt = rubberBandSelection->StartPoint;
		Point endPt = rubberBandSelection->EndPoint;

		// Don't process if it was just a click (no drag)
		if (startPt.Equals(endPt)) {
			ClearRetracementVisuals(); // Ensure everything is cleared/disabled on click
			return;
		}

		// Check if chart objects are ready
		if (chart_CandlestickChart == nullptr || chart_CandlestickChart->ChartAreas->Count == 0) return;
		ChartArea^ chartArea = chart_CandlestickChart->ChartAreas[0];
		if (chartArea == nullptr || chartArea->AxisX == nullptr || chartArea->AxisY == nullptr) return;
		Axis^ axisX = chartArea->AxisX;
		Axis^ axisY = chartArea->AxisY;

		// --- Convert Pixels to Axis Values ---
		double startXVal, startYVal, endXVal, endYVal;
		try {
			startXVal = axisX->PixelPositionToValue(startPt.X);
			startYVal = axisY->PixelPositionToValue(startPt.Y);
			endXVal = axisX->PixelPositionToValue(endPt.X);
			endYVal = axisY->PixelPositionToValue(endPt.Y);

			// Check for NaN results from conversion
			if (Double::IsNaN(startXVal) || Double::IsNaN(startYVal) || Double::IsNaN(endXVal) || Double::IsNaN(endYVal)) {
				throw gcnew ArgumentException("Pixel to value conversion resulted in NaN.");
			}

			// --- Store Active Wave Info ---
			activeWaveDefined = true;
			activeWaveStartXVal = startXVal;
			activeWaveStartYVal = startYVal;
			activeWaveEndXVal = endXVal;
			activeWaveBaseEndYVal = endYVal; // Store the originally drawn end Y
			activeWaveCurrentEndYVal = endYVal; // Initialize current Y

			// --- Final Draw ---
			DrawFibRetracement(startXVal, startYVal, endXVal, endYVal);

			// --- Calculate Bounds for Confirmation Check ---
			double minXVal = Math::Min(startXVal, endXVal);
			double maxXVal = Math::Max(startXVal, endXVal);
			double minYVal = Math::Min(startYVal, endYVal); // Use absolute min/max of start/end
			double maxYVal = Math::Max(startYVal, endYVal);

			// --- Calculate, Show Confirmations, Update TextBox ---
			// Ensure calculated bounds are valid
			if (!Double::IsNaN(minXVal) && !Double::IsNaN(maxXVal) && !Double::IsNaN(minYVal) && !Double::IsNaN(maxYVal))
			{
				int confirmationCount = CalculateAndShowConfirmations(minXVal, maxXVal, minYVal, maxYVal);
				if (textBox_confirmations != nullptr) {
					textBox_confirmations->Text = confirmationCount.ToString();
				}
			}
			else {
				if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error";
			}
			// MessageBox::Show("Fibonacci Confirmations within selection: " + confirmationCount, ...); // Optional


			// --- Enable Controls ---
			button_SimulateStartStop->Enabled = true;
			button_StepUp->Enabled = true;
			button_StepDown->Enabled = true;
			scrollBar_SimMinPercent->Enabled = true;
			scrollBar_SimMaxPercent->Enabled = true;
			textBox_StepSizePercent->Enabled = true;

		}
		catch (Exception^ ex) { // Catch conversion errors or others
			System::Diagnostics::Debug::WriteLine("Error during MouseUp processing: " + ex->Message);
			ClearRetracementVisuals(); // Clear potentially inconsistent visuals
			if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error";
		}
	}
}

private: System::Void simulationTimer_Tick(System::Object^ sender, System::EventArgs^ e) {
	if (!isSimulating || !activeWaveDefined) {
		StopSimulation(); // Stop if state is inconsistent
		return;
	}

	// Calculate next step only if step size is valid
	if (Math::Abs(stepSizeAxisUnits) <= Double::Epsilon) {
		System::Diagnostics::Debug::WriteLine("Simulation step size is zero or invalid.");
		StopSimulation();
		return;
	}
	activeWaveCurrentEndYVal += (stepSizeAxisUnits * currentSimDirection);

	// Check bounds and reverse direction if needed
	bool reversed = false;
	if (currentSimDirection > 0 && activeWaveCurrentEndYVal >= simMaxYVal) {
		activeWaveCurrentEndYVal = simMaxYVal; // Clamp to max
		currentSimDirection = -1.0;        // Reverse
		reversed = true;
	}
	else if (currentSimDirection < 0 && activeWaveCurrentEndYVal <= simMinYVal) {
		activeWaveCurrentEndYVal = simMinYVal; // Clamp to min
		currentSimDirection = 1.0;         // Reverse
		reversed = true;
	}

	// Draw the updated retracement
	// Check if dependent values are valid before drawing
	if (!Double::IsNaN(activeWaveStartXVal) && !Double::IsNaN(activeWaveStartYVal) &&
		!Double::IsNaN(activeWaveEndXVal) && !Double::IsNaN(activeWaveCurrentEndYVal))
	{
		DrawFibRetracement(activeWaveStartXVal, activeWaveStartYVal, activeWaveEndXVal, activeWaveCurrentEndYVal);
	}
	else {
		System::Diagnostics::Debug::WriteLine("Skipping draw in timer tick due to NaN coordinates.");
		StopSimulation(); // Stop if coordinates became invalid
		return;
	}


	// Update confirmations based on the *new* end Y value
	double finalMinY = Math::Min(activeWaveStartYVal, activeWaveCurrentEndYVal);
	double finalMaxY = Math::Max(activeWaveStartYVal, activeWaveCurrentEndYVal);
	// Use fixed start/end X for horizontal bounds during simulation/stepping
	double fixedMinX = Math::Min(activeWaveStartXVal, activeWaveEndXVal);
	double fixedMaxX = Math::Max(activeWaveStartXVal, activeWaveEndXVal);

	// Ensure calculated bounds are valid numbers before checking confirmations
	if (!Double::IsNaN(fixedMinX) && !Double::IsNaN(fixedMaxX) && !Double::IsNaN(finalMinY) && !Double::IsNaN(finalMaxY))
	{
		int count = CalculateAndShowConfirmations(fixedMinX, fixedMaxX, finalMinY, finalMaxY);
		if (textBox_confirmations != nullptr) { // Check textbox exists
			textBox_confirmations->Text = count.ToString();
		}
	}
	else {
		System::Diagnostics::Debug::WriteLine("Skipping confirmation check in timer tick due to NaN bounds.");
		if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error"; // Indicate issue
	}

	// Optional: Stop simulation on reversal if desired
	// if (reversed) { StopSimulation(); }
}

private: System::Void button_SimulateStartStop_Click(System::Object^ sender, System::EventArgs^ e) {
	if (!activeWaveDefined) {
		MessageBox::Show("Please select a wave or draw a selection first.", "No Wave Active", MessageBoxButtons::OK, MessageBoxIcon::Warning);
		return;
	}

	if (isSimulating) {
		StopSimulation();
	}
	else {
		StartSimulation();
	}
}
	   // --- Step Up (+) Button ---
private: System::Void button_StepUp_Click(System::Object^ sender, System::EventArgs^ e) {
	if (isSimulating || !activeWaveDefined) return; // Don't allow manual step during simulation

	if (CalculateStepSize()) { // Calculate step size based on current settings
		activeWaveCurrentEndYVal += stepSizeAxisUnits;
		// Optional: Add clamping if needed beyond simulation bounds
		// Example clamping: activeWaveCurrentEndYVal = Math::Min(activeWaveCurrentEndYVal, some_upper_limit);

		// Check if coordinates are valid before drawing/calculating
		if (!Double::IsNaN(activeWaveStartXVal) && !Double::IsNaN(activeWaveStartYVal) &&
			!Double::IsNaN(activeWaveEndXVal) && !Double::IsNaN(activeWaveCurrentEndYVal))
		{
			// Draw and update confirmations
			DrawFibRetracement(activeWaveStartXVal, activeWaveStartYVal, activeWaveEndXVal, activeWaveCurrentEndYVal);
			double finalMinY = Math::Min(activeWaveStartYVal, activeWaveCurrentEndYVal);
			double finalMaxY = Math::Max(activeWaveStartYVal, activeWaveCurrentEndYVal);
			double fixedMinX = Math::Min(activeWaveStartXVal, activeWaveEndXVal);
			double fixedMaxX = Math::Max(activeWaveStartXVal, activeWaveEndXVal);
			// Ensure bounds are valid before calculating
			if (!Double::IsNaN(fixedMinX) && !Double::IsNaN(fixedMaxX) && !Double::IsNaN(finalMinY) && !Double::IsNaN(finalMaxY))
			{
				int count = CalculateAndShowConfirmations(fixedMinX, fixedMaxX, finalMinY, finalMaxY);
				if (textBox_confirmations != nullptr) {
					textBox_confirmations->Text = count.ToString();
				}
			}
			else {
				if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error";
			}
		}
		else {
			System::Diagnostics::Debug::WriteLine("Skipping Step Up due to NaN coordinates.");
			if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error";
		}
	}
}


	   // --- Step Down (-) Button ---
private: System::Void button_StepDown_Click(System::Object^ sender, System::EventArgs^ e) {
	if (isSimulating || !activeWaveDefined) return; // Don't allow manual step during simulation

	if (CalculateStepSize()) { // Calculate step size based on current settings
		activeWaveCurrentEndYVal -= stepSizeAxisUnits;
		// Optional: Add clamping if needed beyond simulation bounds
		// Example clamping: activeWaveCurrentEndYVal = Math::Max(activeWaveCurrentEndYVal, some_lower_limit);

		// Check if coordinates are valid before drawing/calculating
		if (!Double::IsNaN(activeWaveStartXVal) && !Double::IsNaN(activeWaveStartYVal) &&
			!Double::IsNaN(activeWaveEndXVal) && !Double::IsNaN(activeWaveCurrentEndYVal))
		{
			// Draw and update confirmations
			DrawFibRetracement(activeWaveStartXVal, activeWaveStartYVal, activeWaveEndXVal, activeWaveCurrentEndYVal);
			double finalMinY = Math::Min(activeWaveStartYVal, activeWaveCurrentEndYVal);
			double finalMaxY = Math::Max(activeWaveStartYVal, activeWaveCurrentEndYVal);
			double fixedMinX = Math::Min(activeWaveStartXVal, activeWaveEndXVal);
			double fixedMaxX = Math::Max(activeWaveStartXVal, activeWaveEndXVal);
			// Ensure bounds are valid before calculating
			if (!Double::IsNaN(fixedMinX) && !Double::IsNaN(fixedMaxX) && !Double::IsNaN(finalMinY) && !Double::IsNaN(finalMaxY))
			{
				int count = CalculateAndShowConfirmations(fixedMinX, fixedMaxX, finalMinY, finalMaxY);
				if (textBox_confirmations != nullptr) {
					textBox_confirmations->Text = count.ToString();
				}
			}
			else {
				if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error";
			}
		}
		else {
			System::Diagnostics::Debug::WriteLine("Skipping Step Down due to NaN coordinates.");
			if (textBox_confirmations != nullptr) textBox_confirmations->Text = "Error";
		}
	}
}

private: System::Void scrollBar_SimPercent_Scroll(System::Object^ sender, System::Windows::Forms::ScrollEventArgs^ e) {
	// Update labels when scroll bars change
	label_SimMinPercentValue->Text = scrollBar_SimMinPercent->Value.ToString() + "%";
	label_SimMaxPercentValue->Text = scrollBar_SimMaxPercent->Value.ToString() + "%";
	// Ensure Max >= Min
	if (scrollBar_SimMaxPercent->Value < scrollBar_SimMinPercent->Value) {
		scrollBar_SimMaxPercent->Value = scrollBar_SimMinPercent->Value;
		label_SimMaxPercentValue->Text = scrollBar_SimMaxPercent->Value.ToString() + "%";
	}
	if (scrollBar_SimMinPercent->Value > scrollBar_SimMaxPercent->Value) {
		scrollBar_SimMinPercent->Value = scrollBar_SimMaxPercent->Value;
		label_SimMinPercentValue->Text = scrollBar_SimMinPercent->Value.ToString() + "%";
	}
}
private: System::Void textBox_StepSizePercent_TextChanged(System::Object^ sender, System::EventArgs^ e) {
	// Optional: Validate input to ensure it's a valid number
	double stepPercentTest;
	if (!Double::TryParse(textBox_StepSizePercent->Text, stepPercentTest) || stepPercentTest <= 0) {
		// Handle invalid input, maybe reset to default or show error tooltip
		// For now, we parse it inside CalculateStepSize
	}
}
};
}