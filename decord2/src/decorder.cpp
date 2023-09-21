#include "decorder.h"
//-------------------------------------------------
using namespace std;
int debug = 2; // 1 = light, 2 = detail, 3 = verbose

bool t_format = false;

typedef struct stp_{
  unsigned int nevent;
  unsigned int cmd1bit;
  unsigned int ncycle;
  unsigned int ntdc;
} stp;

enum sm_ {first,second,third,other};

int change_endian(int val_big_endian)
{
  int val = 0;
  val = (val_big_endian & 0xff000000)>>24;
  val |= (val_big_endian & 0x00ff0000)>>8;
  val |= (val_big_endian & 0x0000ff00)<<8;
  val |= (val_big_endian & 0x000000ff)<<24;
  return val;
}

void c_format()
{
  return;
}

bool isAdcHg(unsigned int data)
{
  if (t_format) {
    return (data & 0xffc00000) == 0xc0000000;
  }
  else return (data & 0x00680000) == 0x00000000;
}

bool isAdcLg(unsigned int data)
{
  if (t_format)
    return (data & 0xffc00000) == 0xc0400000;

  else
    return (data & 0x00680000) == 0x00080000;
}

bool isTdcLeading(unsigned int data)
{
  if (t_format)
    return (data & 0xff000000) == 0xc1000000;
  else
    return (data & 0x00601000) == 0x00201000;
}

bool isTdcTrailing(unsigned int data)
{
  if (t_format)
    return (data & 0xff000000) == 0xc1000000;
  else
    return (data & 0x00601000) == 0x00200000;
}

bool isScaler(unsigned int data)
{
  if (t_format) 
    return (data & 0xff000000) == 0xc2000000;
  else
    return (data & 0x00600000) == 0x00400000;
}

bool isStp(unsigned int data)
{
  if (t_format)
    return (data & 0xff000000) == 0xc3000000;
  else
    return (data & 0x00600000) == 0x00600000;
}

bool optIsCN(string opt,int* cn,bool* isRO)
{
  int num;
  string optstr1 = opt.substr(0,2);
  string optstr2 = opt.substr(2);
  if (debug>1)
    cout << "optIsCN():optstr1=" << optstr1 << " optstr2=" << optstr2 << endl;

  istringstream ss(optstr2);
  ss >> num;

  if (optstr1=="CN") {
    if      (num == 11) num = 1;
    else if (num == 12) num = 2;
    else if (num == 13) num = 3;
    else if (num == 14) num = 4;
    else if (num == 21) num = 5;
    else if (num == 22) num = 6;
    else if (num == 23) num = 7;
    else if (num == 24) num = 8;

    *isRO = false;
    if (1<=num && num<=8){
      if (debug>1)cout<<"optIsCN():CN"<<num<<endl;
      *cn = num;
      return true;
    }
    cout<<"optIsCN(): wrong argument:CN"<<num<<endl;
    return false;
  }
  else if (optstr1=="RO") {
    *isRO = true;
    if (1<=num && num<=8) {
      if (debug>1)
	cout << "optIsCN():RO" << num << endl;
      *cn = num;
      return true;
    }
    cout << "optIsCN(): wrong argument:RO" << num << endl;
    return false;
  }
  else {
    return false;
  }
}

bool optIsCH(string opt,int* ch) {
  int num;
  string optstr1 = opt.substr(0,2);
  string optstr2 = opt.substr(2);
  if (debug>1)
    cout << "optIsCH():optstr1=" << optstr1 << " optstr2=" << optstr2 << endl;

  istringstream ss(optstr2);
  ss >> num;
  if (optstr1=="CH") {
    if (0<=num && num<=63) {
      if (debug>1)
	cout << "optIsCN():CH" << num << endl;
      *ch = num;
      return true;
    }
    cout << "optIsCH(): wrong argument:CH" << num << endl;
    return false;
  }
  return false;
}

bool optIsSN(string opt,int* cn,bool* isRO) {
  int num;
  string optstr1 = opt.substr(0,2);
  string optstr2 = opt.substr(2);
  if (debug>1)
     cout << "optIsCN():optstr1=" << optstr1 << " optstr2=" << optstr2 << endl;

  istringstream ss(optstr2);
  ss >> num;

  if (optstr1=="SN") {
    if      (num == 11) num = 1;
    else if (num == 12) num = 2;
    else if (num == 13) num = 3;
    else if (num == 14) num = 4;
    else if (num == 21) num = 5;
    else if (num == 22) num = 6;
    else if (num == 23) num = 7;
    else if (num == 24) num = 8;

    *isRO = false;
    if (1<=num && num<=8) {
      if (debug>1)
	cout << "optIsSN():SN" << num << endl;
      *cn = num;
      return true;
    }
    cout << "optIsSN(): wrong argument:SN" << num << endl;
    return false;
  } else if (optstr1=="RO") {
    *isRO = true;
    if (1<=num && num<=8) {
      if (debug>1)
	cout << "optIsSN():RO" << num << endl;
      *cn = num;
      return true;
    }
    cout << "optIsSN(): wrong argument:RO" << num << endl;
    return false;
  } else {
    return false;
  }
}

bool optIsSH(string opt,int* ch) {
  int num;
  string optstr1 = opt.substr(0,2);
  string optstr2 = opt.substr(2);
  if (debug>1)
    cout << "optIsCH():optstr1=" << optstr1 << " optstr2=" << optstr2 << endl;

  istringstream ss(optstr2);
  ss >> num;
  if (optstr1=="SH") {
    if (0<=num && num<=66) {
      if (debug>1)
	cout << "optIsSN():SH" << num << endl;
      *ch = num;
      return true;
    }
    cout << "optIsSH(): wrong argument:SH" << num << endl;
    return false;
  }
  return false;
}


int getAdcCh(unsigned int data) {
  if (t_format) {
      return (data >> 16) & 0x3f;
  } else {
    return (data >> 13) & 0x3f;
  }
}

bool getAdcOtr(unsigned int data) {
  return ((data >> 12) & 0x01) != 0;
}

int getAdcData(unsigned int data) {
  return data & 0x0fff;
}

int getTdcCh(unsigned int data) {
  if (t_format) {
      return (data >> 16) & 0x3f;
  } else {
    return (data >> 13) & 0x3f;
  }
}

int getTdcData(unsigned int data) {
  return data & 0x0fff;
}

int getScalerCh(unsigned int data) {
  if (t_format) {
      return (data >> 16) & 0x7f;
  } else {
    return (data >> 14) & 0x7f;
  }
}

int getScalerData(unsigned int data) {
  return data & 0x3fff;
}

int main(int argc, char** argv) {

  if (argc<1) {
    cerr << "Usage: ./decorder dat_filename" << endl;
    return -1;
  }

  string dat_filename = string(argv[1]);

  double mean = 0;
  double RMS = 0;
  int cn; // ?
  int cn_; // ?
  int ch; // ?
  bool isRO;

  ifstream f_dat(dat_filename.c_str(),ios::binary);
  if (!f_dat.is_open()) {
    cerr << "no file: " << dat_filename << endl;
    return -1;
  }

  string root_filename = dat_filename.replace(dat_filename.rfind(".dat"), std::string(".dat").length(), ".root");

  TFile *file = new TFile(root_filename.c_str(),"RECREATE");
  TTree *tree = new TTree("eventTree", "Event Tree");

  // ADC class
  ADC hgadc;
  ADC lgadc;
  long eventID;
  
  tree->Branch("eventID", &eventID,    "event/L");
  tree->Branch("hgadc",   &hgadc,  "value[64]/I");
  tree->Branch("lgadc",   &lgadc,  "value[64]/I");
  
  UInt_t data_big_endian;
  UInt_t data;
  UInt_t data_length;
  UInt_t scalerValues[69];
  UInt_t scalerValuesArray[10][69];
  UInt_t events = 0;
  UInt_t counter = 0;

  sm_ sm = first;
  vector<stp> stp_vec;
  stp this_stp;

  while (!f_dat.eof()) {
    f_dat.read((char*)&data_big_endian, sizeof(int));
    data = change_endian(data_big_endian);
    if (t_format) // Please make a comment for this line
      c_format();

    if (debug>3)
      cout << "TString::Format(\"%08x\",data):" << TString::Format("%08x",data) << endl; 

    if ((data & 0xfffff000) == 0x0ffff000) {
      data_length = data & 0x00000fff;
      if (debug>2)
	cout << TString::Format("Regacy Format Header:0x0ffff0.., data_length= %dwords, event%d\n",data_length,events+1);
      eventID = events++;
      sm = first;
      if (events%1000==0)
	cout << "reading events#:" << events << endl;
      continue;
    } else if ((data & 0xffff0000) == 0xff7c0000) {
      data_length = data & 0x00000fff;
      if (debug>2)
	cout << TString::Format("Header:0xff7c...., data_length= %dwords, event%d\n",data_length,events+1);
      t_format = true;
      eventID = events++;
      sm = first;
      if (events%1000==0)
	cout << "reading events#:" << events << endl;
      continue;
    } else if (isAdcHg(data)) { // ADC HG
      int ch = getAdcCh(data);
      bool otr = getAdcOtr(data);
      int value = getAdcData(data);

      hgadc.value[ch] = value;
      
      if (debug>2)
	cout << TString::Format("ADC_HG, ch=%d, otr=%x, value=%d\n", ch, otr, value);
      if (!otr) {
	if (debug>3)
	  cout << otr << endl;
      }
      continue;
    } else if (isAdcLg(data)) {// ADC LG
      int ch = getAdcCh(data);
      bool otr = getAdcOtr(data);
      int value = getAdcData(data);

      lgadc.value[ch] = value;

      if (debug>2)
	cout << TString::Format("ADC_LG, ch=%d, otr=%x, value=%d\n", ch, otr, value);
      if (!otr) {
	if (debug>3)
	  cout << otr << endl;
      }
      continue;
    } else if (isTdcLeading(data)) { // TDC LeadingEdge
      int ch = getTdcCh(data);
      int value = getTdcData(data);
      if (debug>3)
	cout << TString::Format("TDC_L, ch=%d, value=%d\n", ch, value);
      continue;
    } else if (isTdcTrailing(data)) { // TDC TrailingEdge
      int ch = getTdcCh(data);
      int value = getTdcData(data);
      if (debug>3)
	cout << TString::Format("TDC_T, ch=%d, value=%d\n", ch, value);
      continue;
    } else if (isScaler(data)) { // SCALER
      int ch = getScalerCh(data);
      int value = getScalerData(data);
      if (debug>3)
	cout << TString::Format("SCALER, ch=%d, value=%d\n", ch, value);
      scalerValues[ch] = value;

      if (ch == 68) { // ch 0-68, ch 68 is a 1kHz clock
	int scalerValuesArrayIndex = (events -1) % 10;
	memcpy(scalerValuesArray[scalerValuesArrayIndex], scalerValues, sizeof(scalerValues));

	if (events % 10 == 0) {
	  unsigned int scalerValuesSum[69] = {0};
	  //for (int i = 0; i < 69; ++i) {
	  //  scalerValuesSum[i] = 0; // Initialize
	  //}
	  for (int i = 0; i < 10; ++i) { // K.Kawade what is this idenx?
	    for (int j = 0; j < 69; ++j) {
	      scalerValuesSum[j] += scalerValuesArray[i][j];
	    }
	  }

	  int counterCount1MHz = scalerValuesSum[67];
	  int counterCount1KHz = scalerValuesSum[68];
	  if (debug>2) {
	    cout << "counterCount1MHz: " << counterCount1MHz << endl;
	    cout << "counterCount1KHz: " << counterCount1KHz << endl;
	  }
	  // 1count = 1.0ms
	  double counterCount = (double)counterCount1KHz + counterCount1MHz / 1000.0;
	  if (debug>2)
	    cout << "counterCount: " << counterCount << endl;
	  // TODO it will be fixed once firmware will be updated
	  counterCount /= 2.0;
	  if (debug>2)
	    cout << "counterCount: " << counterCount << endl;
	  for(size_t j = 0; j < 67; ++j) {
	    if (debug>2)
	      cout << " scalerValuesSum[" << j << "]:" << scalerValuesSum[j] << endl;
	    bool ovf = (((scalerValuesSum[j] >> 13) & 0x01) != 0);

	    double scalerCount = scalerValuesSum[j] & 0xffff;
	    if (debug>2)
	      cout << " scalerCount: " << scalerCount <<", ovf: " << ovf << endl;
	    if (!ovf && scalerCount != 0) {
	      double rate = scalerCount / counterCount;
	      if (debug>2)
		cout <<" ch: " << j <<" scalerCount: " << scalerCount <<", ovf: " << ovf << " rate: " << rate << endl;
	    }
	  }
	  if (debug>2)
	    cout << endl;
	}
      }
      continue;
    } else if (isStp(data)) { //STP
      if (debug>2)
	cout << TString::Format("STP, data=%08x\n",data);
      if (sm == first) {
	this_stp.nevent = data & 0x00000fff;
	this_stp.cmd1bit = (data & 0x00010000)>>16;
	sm = second;
      } else if (sm == second) {
	this_stp.ncycle = data & 0x00ffffff;
	sm = third;
      } else if (sm == third) {
	this_stp.ntdc = data & 0x00ffffff;
	sm = other;
	stp_vec.push_back(this_stp);
      } else {
	cerr << "Wrong STP data: "<<data<<endl;
      }
      // Fill tree at the end of data format
      tree->Fill();
      continue;
    } else {
      cout << "data:" << data << endl; 
      cerr << "Unknown data type" << endl;
    }
    cout << "Fill Tree" << endl;
    tree->Fill();
  }

  file->Write();
  file->Close();
    
  return 1;
}
