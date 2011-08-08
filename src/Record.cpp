#include "Record.h"
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>


Record::Record(std::string  filename)
:filename_(filename)
{

    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    RealTimeInTheWorld += asctime (timeinfo);
    //ctor
}

Record::~Record()
{
    //dtor
}

void Record::Scatter(double x , double y ,int CSGID)
{

    std::vector<CSGData>::iterator itCSGData;

    CSGData CSG_Data;

    CSG_Data.x = x ;
    CSG_Data.y = y ;
    CSG_Data.CSGID = CSGID ;

    CSGvec.push_back(CSG_Data);
}




void Record::InsertData(double Data)
{

    std::vector<DataNum>::iterator itPDF;
    if( ( itPDF = find_if( PDF.begin(), PDF.end(), findIndex((int)Data))) == PDF.end() )
    {
        DataNum PDFindex ;
        PDFindex.index = (int) Data ;
        PDFindex.Count = 1;
        PDF.push_back(PDFindex);

        return;
    }

    itPDF->Count++;

}

void Record::OutputScatter()
{

    std::string fileName_Scatter = filename_+"position.xls";

    std::ofstream Scatter_file;
    Scatter_file.open( fileName_Scatter.c_str(), std::ios::out);

    for(std::vector<CSGData>::iterator itCSGData=CSGvec.begin(); itCSGData!=CSGvec.end(); itCSGData++)
    {
        Scatter_file<<itCSGData->x<<'\t'<<itCSGData->y<<'\t'<<itCSGData->CSGID<<'\t'<<std::endl;
    }

    Scatter_file.close();
}



void Record::OutputPDF()
{
    sortByIndex sortOBJ;
    sort(PDF.begin(),PDF.end(),sortOBJ);


    std::string fileName_PDF = filename_+"_PDF.xls";

    std::ofstream pdf_file;
    pdf_file.open( fileName_PDF.c_str(), std::ios::out);

    int totalCout=0;
    for(std::vector<DataNum>::iterator itPDF=PDF.begin(); itPDF!=PDF.end(); itPDF++)
    {
        totalCout+= itPDF->Count;
    }


    for(std::vector<DataNum>::iterator itPDF=PDF.begin(); itPDF!=PDF.end(); itPDF++)
    {
        pdf_file<<itPDF->index<<'\t'<<((double)(itPDF->Count)/(double)(totalCout))<<std::endl;

    }

    pdf_file.close();
}

void Record::OutputCDF()
{
    sortByIndex sortOBJ;
    sort(PDF.begin(),PDF.end(),sortOBJ);


    std::string fileName_CDF = filename_+"_CDF.xls";

    std::ofstream cdf_file;
    cdf_file.open( fileName_CDF.c_str(), std::ios::out);

    int totalCout=0;
    for(std::vector<DataNum>::iterator itPDF=PDF.begin(); itPDF!=PDF.end(); itPDF++)
    {
        totalCout+= itPDF->Count;
    }

    int cumulativeCount=0;

    for(std::vector<DataNum>::iterator itPDF=PDF.begin(); itPDF!=PDF.end(); itPDF++)
    {
        cumulativeCount+=itPDF->Count;
        cdf_file<<itPDF->index<<'\t'<<(double)(cumulativeCount)/(double)(totalCout)<<std::endl;

    }

    cdf_file.close();

}

double Record::Average()
{

}

double Record::variance()
{}

