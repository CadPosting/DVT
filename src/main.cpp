#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <random>


#include "C:/libxl-win-4.1.2/libxl-4.1.2/include_cpp/libxl.h"  //#include <libxl/libxl.h>
using namespace libxl;

#include "matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;

// Operator overloading functions for DataPoint objects
class DataPoint {
public:
	//constructors
    DataPoint(double x, double y, double value) : x_(x), y_(y), value_(value) {}
    DataPoint() : x_(0), y_(0), value_(0) {}  //default constructor

    // Overloaded arithmetic operators (Operator Overloading)
    //Friend Functions
    friend DataPoint operator+(const DataPoint& lhs, const DataPoint& rhs);
    friend DataPoint operator-(const DataPoint& lhs, const DataPoint& rhs);
    
    double GetX() const {
						 return x_; 
						}
	
	double GetY() const {
						 return y_; 
						}
	double GetValue() const { 
							return value_; 
							}


private:
    double x_;
    double y_;
    double value_;
};

// Friend Functions (Operator Overloading)
DataPoint operator+(const DataPoint& lhs, const DataPoint& rhs) {
    double x = lhs.x_ + rhs.x_;
    double y = lhs.y_ + rhs.y_;
    double value = lhs.value_ + rhs.value_;
    return DataPoint(x, y, value);
}

DataPoint operator-(const DataPoint& lhs, const DataPoint& rhs) {
    double x = lhs.x_ - rhs.x_;
    double y = lhs.y_ - rhs.y_;
    double value = lhs.value_ - rhs.value_;
    return DataPoint(x, y, value);
}




// Base class for all visualization tools
class VisualizationTool {
public:
    VisualizationTool() {}
    virtual ~VisualizationTool() {}

    // Pure virtual functions to be implemented by derived classes (Virtual Functions)
    virtual void LoadData() = 0;
    virtual void GenerateVisualization() = 0;
    virtual void LoadDataFromFile(const string& filename) = 0;


    // Getter and setter functions for the chart title
	void SetChartTitle(const string& title) { m_title = title; }
    string GetChartTitle() const { return m_title; }

    // Getter and setter functions for the chart width and height
    void SetChartWidth(int width) { m_width = width; }
    int GetChartWidth() const { return m_width; }
    void SetChartHeight(int height) { m_height = height; }
    int GetChartHeight() const { return m_height; }
    
    // Getter and setter functions for accessing data points 
    vector<DataPoint>& getDataPoints() { return m_dataPoints; }  
    void setDataPoints(const vector<DataPoint>& dataPoints) { m_dataPoints = dataPoints; }

protected:
    // Protected data fields that can be accessed by derived classes
    vector<DataPoint> m_dataPoints; //vector is a template class from STL and it's used to create a dynamic array of DataPoint objects
	string m_title;
    int m_width;
    int m_height;
};




// Derived class for scatter plot visualization tool (Inheritance)
class ScatterPlot : public VisualizationTool {
public:
	DataPoint CalculateAverageDataPoint();
	// Function Overriding (Polymorphism)
    void LoadData() override; 
    void GenerateVisualization() override; 
    void LoadDataFromFile(const string& filename) override;

    // Setter and getter functions for the marker size and color
    void SetMarkerSize(int size) { m_markerSize = size; }
    int GetMarkerSize() const { return m_markerSize; }
    void SetMarkerColor(const string& color) { m_markerColor = color; }
    string GetMarkerColor() const { return m_markerColor; }

private:
    int m_markerSize;
    string m_markerColor;
    string filename;
};

// File Handling
void ScatterPlot::LoadData() {
    // Check if the data points vector is already populated
    if (!m_dataPoints.empty()) {
        return;
    }

    // Prompt the user to select data source
    cout << "Enter 1 to load data from file, or 2 to generate random data: ";
    int choice;
    cin >> choice;

    // Load data from file
    if (choice == 1) {
    	
            cout << "Enter filename: ";
            cin >> filename;
            
		try { LoadDataFromFile(filename); }
		catch (const runtime_error& e) { 
		cout << "Exception caught: " << e.what() << endl; 
		}
    }
    // Generate random data
    else if (choice == 2) {
        int numDataPoints;
        cout << "Enter number of data points to generate: ";
        cin >> numDataPoints;

        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);

        for (int i = 0; i < numDataPoints; i++) {
            double x = dis(gen);
            double y = dis(gen);
            double value = dis(gen);
            DataPoint dataPoint(x, y, value);
            m_dataPoints.push_back(dataPoint);
        }
    }
    // Invalid choice
    else {
        throw runtime_error("Invalid choice");  //Exception Handling
    }
}

// Function for loading data from excel file
void ScatterPlot::LoadDataFromFile(const string& filename) {
    libxl::Book* book = xlCreateXMLBook();
    cout << "Attempting to load file: " << filename << endl;
    if (book->load(filename.c_str())) {
    	cout << "File loaded successfully." << endl;
        libxl::Sheet* sheet = book->getSheet(0);
        if (sheet) {
            for (int row = sheet->firstRow(); row < sheet->lastRow(); ++row) {
                double x = 0.0, y = 0.0, value = 0.0;
                for (int col = sheet->firstCol(); col < sheet->lastCol(); ++col) {
                    if (sheet->cellType(row, col) == libxl::CELLTYPE_NUMBER) {
                        double cellValue = sheet->readNum(row, col);
                        if (col == sheet->firstCol()) x = cellValue;
                        else if (col == sheet->firstCol() + 1) y = cellValue;
                        else if (col == sheet->firstCol() + 2) value = cellValue;
                    }
                }
                DataPoint dataPoint(x, y, value);
                m_dataPoints.push_back(dataPoint);
            }
        }
        else {
            throw runtime_error("Failed to open sheet 0");
        }
    }
    else {
		string error_message = book->errorMessage();
		throw runtime_error("Failed to load book from file: " + filename + ". Error: " + error_message);
}

    book->release();
}

void ScatterPlot::GenerateVisualization() {
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to visualize");  //Exception Handling
    }



    vector<double> x, y;
    for (const auto& dataPoint : m_dataPoints) {
        x.push_back(dataPoint.GetX());
		y.push_back(dataPoint.GetY());

    }

 	// Calculate the average data point
    DataPoint averageDataPoint = CalculateAverageDataPoint();

    // Convert single values to vectors with one element
    vector<double> avgX = {averageDataPoint.GetX()};
    vector<double> avgY = {averageDataPoint.GetY()};

    // Add the average data point to the plot
    plt::scatter(avgX, avgY, m_markerSize * 2, {{"color", "blue"}});
 

    plt::scatter(x, y, m_markerSize, {{"color", m_markerColor}});
    plt::title(m_title);
    plt::xlabel("X");
    plt::ylabel("Y");
    plt::grid(true);
    plt::show();
}

DataPoint ScatterPlot::CalculateAverageDataPoint() {
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to calculate average");
    }

    DataPoint averageDataPoint;
    for (const auto& dataPoint : m_dataPoints) {
        averageDataPoint = averageDataPoint + dataPoint;
    }
    
    double size = static_cast<double>(m_dataPoints.size());
    averageDataPoint = DataPoint(averageDataPoint.GetX() / size, averageDataPoint.GetY() / size, averageDataPoint.GetValue() / size);

    return averageDataPoint;
}




// Derived class for bar chart visualization tool (Inheritance)
class BarChart : public VisualizationTool {
public:
	DataPoint CalculateAverageDataPoint();
	// Function Overriding (Polymorphism)
    void LoadData() override; 
    void GenerateVisualization() override; 
    void LoadDataFromFile(const string& filename) override;

    // Setter and getter functions for the bar width and color
    void SetBarWidth(int width) { m_barWidth = width; }
    int GetBarWidth() const { return m_barWidth; }
    void SetBarColor(const string& color) { m_barColor = color; }
    string GetBarColor() const { return m_barColor; }

private:
    int m_barWidth = 10;
    string m_barColor;
    string filename;
};

// File Handling
void BarChart::LoadData() {
    // Check if the data points vector is already populated
    if (!m_dataPoints.empty()) {
        return;
    }

    // Prompt the user to select data source
    cout << "Enter 1 to load data from file, or 2 to generate random data: ";
    int choice;
    cin >> choice;

    // Load data from file
    if (choice == 1) {

            cout << "Enter filename: ";
            cin >> filename;
            
		try { LoadDataFromFile(filename); }
		catch (const runtime_error& e) { 
		cout << "Exception caught: " << e.what() << endl; }
    }
	 // Generate random data
	else if (choice == 2) {
		int numDataPoints;
		cout << "Enter number of data points to generate: ";
		cin >> numDataPoints;
  	  
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(1.0, 20.0);
   	 
    for (int i = 0; i < numDataPoints; i++) {
        double x = i;
        double y = dis(gen); // Assign random value to y-coordinate
        double value = dis(gen);
        DataPoint dataPoint(x, y, value);
        m_dataPoints.push_back(dataPoint);
    }
}
    // Invalid choice
    else {
        throw runtime_error("Invalid choice"); //exception handling
    }
}


// Function for loading data from excel file
void BarChart::LoadDataFromFile(const string& filename) {
    libxl::Book* book = xlCreateXMLBook();
    cout << "Attempting to load file: " << filename << endl;
    if (book->load(filename.c_str())) {
        cout << "File loaded successfully." << endl;
        libxl::Sheet* sheet = book->getSheet(0);
        if (sheet) {
            for (int row = sheet->firstRow() + 1; row < sheet->lastRow(); ++row) {
                double x = 0.0, y = 0.0, value = 0.0;
                for (int col = sheet->firstCol(); col < sheet->lastCol(); ++col) {
                    if (sheet->cellType(row, col) == libxl::CELLTYPE_NUMBER) {
                        double cellValue = sheet->readNum(row, col);
                        if (col == sheet->firstCol()) {
                            if(cellValue > 100){ // assuming 100 as the limit for suitable x values.
                                x = row - 1; // use cell number if data from column A is too large
                            } else {
                                x = cellValue;
                            }
                        }
                        else if (col == sheet->firstCol() + 1) y = cellValue;
                        else if (col == sheet->firstCol() + 2) value = cellValue;
                    }
                }
                DataPoint dataPoint(x, y, value);
                m_dataPoints.push_back(dataPoint);
            }
        }
        else {
            throw runtime_error("Failed to open sheet 0");
        }
    }
    else {
        string error_message = book->errorMessage();
        throw runtime_error("Failed to load book from file: " + filename + ". Error: " + error_message);
    }

    book->release();
}




void BarChart::GenerateVisualization() {
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to visualize");
    }

    vector<double> x, y;
    for (const auto& dataPoint : m_dataPoints) {
        x.push_back(dataPoint.GetX());
        y.push_back(dataPoint.GetValue());
    }

    // Calculate the average data point
    DataPoint averageDataPoint = CalculateAverageDataPoint();

    // Convert single values to vectors with one element
    vector<double> avgX = {averageDataPoint.GetX()};
    vector<double> avgY = {averageDataPoint.GetValue()};

    // Define color map
    map<string, string> color_map = {{"color", "blue"}};

    // Add the average data point to the plot
    plt::bar(avgX, avgY, "", "", static_cast<double>(m_barWidth), color_map);


    // Generate the bar chart
    plt::bar(x, y);
    plt::title(m_title);
    plt::xlabel("X");
    plt::ylabel("Value");
    plt::grid(true);
    plt::show();
}

DataPoint BarChart::CalculateAverageDataPoint() {
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to calculate average");
    }

    DataPoint averageDataPoint;
    for (const auto& dataPoint : m_dataPoints) {
        averageDataPoint = averageDataPoint + dataPoint;
    }
    
    double size = static_cast<double>(m_dataPoints.size());
    averageDataPoint = DataPoint(averageDataPoint.GetX() / size, averageDataPoint.GetY() / size, averageDataPoint.GetValue() / size);

    return averageDataPoint;
}




// Derived class for line plot visualization tool (Inheritance)
class LinePlot : public VisualizationTool {
public:
	DataPoint CalculateAverageDataPoint();
	// Function Overriding (Polymorphism)
    void LoadData() override; 
    void GenerateVisualization() override; 
    void LoadDataFromFile(const string& filename) override;

    // Setter and getter functions for the line width and color
    void SetLineWidth(int width) { m_lineWidth = width; }
    int GetLineWidth() const { return m_lineWidth; }
    void SetLineColor(const string& color) { m_lineColor = color; }
    string GetLineColor() const { return m_lineColor; }

private:
    int m_lineWidth = 1;
    string m_lineColor;
    string filename;
};

// File Handling
void LinePlot::LoadData() {
    // Check if the data points vector is already populated
    if (!m_dataPoints.empty()) {
        return;
    }

    // Prompt the user to select data source
    cout << "Enter 1 to load data from file, or 2 to generate random data: ";
    int choice;
    cin >> choice;

    // Load data from file
    if (choice == 1) {

            cout << "Enter filename: ";
            cin >> filename;
    
		try { LoadDataFromFile(filename); }
		catch (const runtime_error& e) { 
		cout << "Exception caught: " << e.what() << endl; }
    }
    
    // Generate random data
	else if (choice == 2) {
    int numDataPoints;
    cout << "Enter number of data points to generate: ";
    cin >> numDataPoints;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 20.0);

    // Generate x-values
    vector<double> x_values;
    for (int i = 0; i < numDataPoints; i++) {
        double x = dis(gen);
        x_values.push_back(x);
    }

    // Sort the x-values
    sort(x_values.begin(), x_values.end());

    // Generate corresponding y-values and create DataPoint objects
    for (const auto& x : x_values) {
        double y = dis(gen);
        double value = dis(gen);
        DataPoint dataPoint(x, y, value);
        m_dataPoints.push_back(dataPoint);
    }
}

    // Invalid choice
    else {
        throw runtime_error("Invalid choice");  //Exception Handling
    }
}


// Function for loading data from excel file
void LinePlot::LoadDataFromFile(const string& filename) {
libxl::Book* book = xlCreateXMLBook();
    cout << "Attempting to load file: " << filename << endl;
    if (book->load(filename.c_str())) {
    	cout << "File loaded successfully." << endl;
        libxl::Sheet* sheet = book->getSheet(0);
        if (sheet) {
            for (int row = sheet->firstRow(); row < sheet->lastRow(); ++row) {
                double x = 0.0, y = 0.0, value = 0.0;
                for (int col = sheet->firstCol(); col < sheet->lastCol(); ++col) {
                    if (sheet->cellType(row, col) == libxl::CELLTYPE_NUMBER) {
                        double cellValue = sheet->readNum(row, col);
                        if (col == sheet->firstCol()) x = cellValue;
                        else if (col == sheet->firstCol() + 1) y = cellValue;
                        else if (col == sheet->firstCol() + 2) value = cellValue;
                    }
                }
                DataPoint dataPoint(x, y, value);
                m_dataPoints.push_back(dataPoint);
            }
        }
        else {
            throw runtime_error("Failed to open sheet 0");
        }
    }
    else {
		string error_message = book->errorMessage();
		throw runtime_error("Failed to load book from file: " + filename + ". Error: " + error_message);
}

    book->release();
}

void LinePlot::GenerateVisualization() {
    // Implementation for generating line plot visualization using Matplotlibcpp
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to visualize");  //Exception Handling
    }

    vector<double> x, y;
    for (const auto& dataPoint : m_dataPoints) {
        x.push_back(dataPoint.GetX());
        y.push_back(dataPoint.GetY());
    }


    plt::plot(x, y, {{"color", m_lineColor}, {"linewidth", to_string(m_lineWidth)}});
	plt::title(m_title);
    plt::xlabel("X");
    plt::ylabel("Y");
    plt::grid(true);
    plt::show();
}

DataPoint LinePlot::CalculateAverageDataPoint() {
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to calculate average");
    }

    DataPoint averageDataPoint;
    for (const auto& dataPoint : m_dataPoints) {
        averageDataPoint = averageDataPoint + dataPoint;
    }
    
    double size = static_cast<double>(m_dataPoints.size());
    averageDataPoint = DataPoint(averageDataPoint.GetX() / size, averageDataPoint.GetY() / size, averageDataPoint.GetValue() / size);

    return averageDataPoint;
}




class Histogram : public VisualizationTool {
public:
	DataPoint CalculateAverageDataPoint();
	// Function Overriding (Polymorphism)
    void LoadData() override; 
    void GenerateVisualization() override; 
    void LoadDataFromFile(const string& filename) override;

    // Setter and getter functions for the bin number
    void SetBinNumber(int bins) { m_binNumber = bins; }
    int GetBinNumber() const { return m_binNumber; }

private:
    int m_binNumber = 7;
    string filename;
};

// File Handling
void Histogram::LoadData() {
    // Check if the data points vector is already populated
    if (!m_dataPoints.empty()) {
        return;
    }

    // Prompt the user to select data source
    cout << "Enter 1 to load data from file, or 2 to generate random data: ";
    int choice;
    cin >> choice;

    // Load data from file
    if (choice == 1) {

            cout << "Enter filename: ";
            cin >> filename;
        
		try { LoadDataFromFile(filename); }
		catch (const runtime_error& e) { 
		cout << "Exception caught: " << e.what() << endl; }
    }
 // Generate random data
	else if (choice == 2) {
		int numDataPoints;
		cout << "Enter number of data points to generate: ";
		cin >> numDataPoints;
  	  
  		random_device rd;
    	mt19937 gen(rd());
    	normal_distribution<> dis(15.0, 3.0); // mean 15, standard deviation 3

    	for (int i = 0; i < numDataPoints; i++) {
       		 double x = i;
      		 double y = dis(gen); // Assign random value to y-coordinate
       		 double value = dis(gen);
       		 
       		 DataPoint dataPoint(x, y, value);
       		 m_dataPoints.push_back(dataPoint);
    }
}
    // Invalid choice
    else {
        throw runtime_error("Invalid choice");
    }
}


// Function for loading data from excel file
void Histogram::LoadDataFromFile(const string& filename) {
libxl::Book* book = xlCreateXMLBook();
    cout << "Attempting to load file: " << filename << endl;
    if (book->load(filename.c_str())) {
    	cout << "File loaded successfully." << endl;
        libxl::Sheet* sheet = book->getSheet(0);
        if (sheet) {
            for (int row = sheet->firstRow(); row < sheet->lastRow(); ++row) {
                double x = 0.0, y = 0.0, value = 0.0;
                for (int col = sheet->firstCol(); col < sheet->lastCol(); ++col) {
                    if (sheet->cellType(row, col) == libxl::CELLTYPE_NUMBER) {
                        double cellValue = sheet->readNum(row, col);
                        if (col == sheet->firstCol()) x = cellValue;
                        else if (col == sheet->firstCol() + 1) y = cellValue;
                        else if (col == sheet->firstCol() + 2) value = cellValue;
                    }
                }
                DataPoint dataPoint(x, y, value);
                m_dataPoints.push_back(dataPoint);
            }
        }
        else {
            throw runtime_error("Failed to open sheet 0");
        }
    }
    else {
		string error_message = book->errorMessage();
		throw runtime_error("Failed to load book from file: " + filename + ". Error: " + error_message);
}

    book->release();
}

void Histogram::GenerateVisualization() {
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to visualize");
    }

    vector<double> values;
    for (const auto& dataPoint : m_dataPoints) {
        values.push_back(dataPoint.GetValue());
    }

	 // Calculate the average data point
    DataPoint averageDataPoint = CalculateAverageDataPoint();


    // Generate the histogram
    plt::hist(values, m_binNumber);
    plt::title(m_title);
    plt::xlabel("Value");
    plt::ylabel("Frequency");
    plt::grid(true);
    
    // Add the average data point to the plot
    plt::plot({averageDataPoint.GetX()}, {averageDataPoint.GetY()}, "ro"); // Plot average data point as a red circle

    
    plt::show();
}

DataPoint Histogram::CalculateAverageDataPoint() {
    if (m_dataPoints.empty()) {
        throw runtime_error("No data to calculate average");
    }

    DataPoint averageDataPoint;
    for (const auto& dataPoint : m_dataPoints) {
        averageDataPoint = averageDataPoint + dataPoint;
    }
    
    double size = static_cast<double>(m_dataPoints.size());
    averageDataPoint = DataPoint(averageDataPoint.GetX() / size, averageDataPoint.GetY() / size, averageDataPoint.GetValue() / size);

    return averageDataPoint;
}




int main() {

	int choice;

	cout << "Enter 1 for Scatter Plot, 2 for Bar Chart, 3 for Line Graph or 4 for Histogram: ";
    cin >> choice;
  
if (choice == 1) {
	    try {
			ScatterPlot scatterPlot;
    		scatterPlot.SetChartTitle("Scatter Plot");
    		scatterPlot.SetMarkerSize(10);
    		scatterPlot.SetMarkerColor("red");
    		scatterPlot.LoadData();
    		scatterPlot.GenerateVisualization();
   			}
    catch (const runtime_error& e) {
        cerr << "A runtime error occurred: " << e.what() << endl;
        return 1;
    }
    catch (...) {
       cerr << "An unknown error occurred." << endl;
        return 2;
    }
     return 0;
}


if (choice == 2) {
    	try {
        BarChart barChart;
        barChart.SetChartTitle("Bar Chart");
        barChart.SetBarWidth(5);
        barChart.SetBarColor("blue");
        barChart.LoadData();
        barChart.GenerateVisualization();
   		}
    catch (const runtime_error& e) {
        cerr << "A runtime error occurred: " << e.what() << endl;
        return 1;
    }
    catch (...) {
       cerr << "An unknown error occurred." << endl;
        return 2;
    }
     return 0;
}


if (choice == 3) {
    try {
        LinePlot linePlot;
		linePlot.SetChartTitle("Line Plot");
    	linePlot.SetLineWidth(2);
    	linePlot.SetLineColor("red");
    	linePlot.LoadData();
    	linePlot.GenerateVisualization();
   		 }
    catch (const runtime_error& e) {
        cerr << "A runtime error occurred: " << e.what() << endl;
        return 1;
    }
    catch (...) {
       cerr << "An unknown error occurred." << endl;
        return 2;
    }
     return 0;
}


if (choice == 4) {
        try {
            Histogram histogram;
            histogram.SetChartTitle("Histogram");
            histogram.SetBinNumber(7); 
            histogram.LoadData();
            histogram.GenerateVisualization();
        }
        catch (const runtime_error& e) {
            cerr << "A runtime error occurred: " << e.what() << endl;
            return 1;
        }
        catch (...) {
            cerr << "An unknown error occurred." << endl;
            return 2;
        }
        return 0;
    }
}


