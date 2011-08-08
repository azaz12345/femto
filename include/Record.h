#ifndef RECORD_H
#define RECORD_H
#include <vector>
#include <string>

typedef struct {

	int index;
	int Count;
}DataNum;

struct findIndex{
	int index_;
	findIndex(int index):index_(index){}

	bool operator () (const DataNum& b) const{

		return b.index == index_;
	}

};

struct sortByIndex{

	bool operator () (const DataNum& d1, const DataNum& d2) const{

		return d1.index < d2.index;
	}

};


class Record
{
    public:
        Record(std::string filename);
        virtual ~Record();

        void InsertData(double Data);

        void OutputPDF();
        void OutputCDF();
        double Average();
        double Variance();
		double Total();

    protected:
    private:

    std::string RealTimeInTheWorld;

	std::vector<double> allValue;
    std::vector<DataNum> PDF;
    std::string filename_;

};

#endif // RECORD_H
