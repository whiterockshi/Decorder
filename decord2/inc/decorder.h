#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TMultiGraph.h>

using namespace std;

// ADC class
class ADC {
public:
    int value[64];
};
