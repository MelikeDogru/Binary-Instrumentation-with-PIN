#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include "pin.H"
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;

ofstream OutFile;
std::ofstream *out = 0;

//Static variables for Task1 to keep numbers of branches and correct and incorrect predictions 
static UINT64 totalCount = 0;
static UINT64 predictedcorrectly = 0;
static UINT64 predictedincorrectly = 0;

//Static variables for Task2 to keep size and  numbers of memory instructions
static UINT64 sizeofmem = 0;
static UINT64 readcounts = 0;
static UINT64 writecounts = 0;

//Static values for Task3 to keep frequences of usage of registers
static UINT64 rax = 0;
static UINT64 rbx = 0;
static UINT64 rcx = 0;
static UINT64 rdx = 0;
static UINT64 rsi = 0;
static UINT64 rdi=0;
static UINT64 rsp=0;
static UINT64 rbp=0;
static UINT64 r8=0;
static UINT64 r9=0;
static UINT64 r10=0;
static UINT64 r11=0;
static UINT64 r12=0;
static UINT64 r13=0;
static UINT64 r14=0;
static UINT64 r15=0;

//names of registers
std::string raxname ("rax");
std::string rbxname ("rbx");
std::string rcxname ("rcx");
std::string rdxname ("rdx");
std::string rsiname ("rsi");
std::string rdiname ("rdi");
std::string rspname ("rsp");
std::string rbpname ("rbp");
std::string r8name ("r8");
std::string r9name ("r9");
std::string r10name ("r10");
std::string r11name ("r11");
std::string r12name ("r12");
std::string r13name ("r13");
std::string r14name ("r14");
std::string r15name ("r15");

//toString function
template <typename T>
std::string toString(T value)
{
    std::ostringstream resultStr;
    resultStr << value;
    return resultStr.str();
}

//Task1; Function to calculete count of correct and incorrect predictions
VOID calculatePredictions(INT32 key, INT32 taken)
{
    //total count of branches
    totalCount++;

    //Check predictions and real conditions to increase counts
    if(taken && key == 1)
    {
      predictedcorrectly++;
    }   
    else if(!taken && key == 0)
    {
      predictedcorrectly++;
    } 
    else
    {
      predictedincorrectly++;
    }
}
    
// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
  //Task 1
    //Works only for branches 
    if(INS_IsBranch(ins))
    {
        //Check if branch is taken or not taken
        if(INS_HasFallThrough(ins))
        {
            //set taken as 0; not taken
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)calculatePredictions, 
                       IARG_ADDRINT, 0,IARG_BRANCH_TAKEN, IARG_END);  
        }
        else
        {
            //set taken as 1; taken
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)calculatePredictions, 
                       IARG_ADDRINT, 1, IARG_BRANCH_TAKEN, IARG_END);  
        }
    }
    
   //Task 2
    //Check the type of instruction: write memory or read memory
    if(INS_IsMemoryWrite(ins))
    {
      sizeofmem += INS_MemoryWriteSize(ins);
      writecounts++;
    }
    if(INS_IsMemoryRead(ins))
    {
      sizeofmem += INS_MemoryReadSize(ins);
      readcounts++;
    }
   //Task 3
    //Find destination register
    REG destination = INS_RegW(ins, 0);
    if(REG_valid(destination))
    {
      //Find name of register
      std::string name = REG_StringShort(destination);
      //Increase frequences according to name of register
      if(name.compare(raxname) == 0){
	rax++;
      }
      if(name.compare(rbxname) == 0){
	rbx++;
      }
      if(name.compare(rcxname) == 0){
	rcx++;
      }
      if(name.compare(rdxname) == 0){
	rdx++;
      }
      if(name.compare(rsiname) == 0){
	rsi++;
      }
      if(name.compare(rdiname) == 0){
	rdi++;
      }
      if(name.compare(rspname) == 0){
	rsp++;
      }
      if(name.compare(rbpname) == 0){
	rbp++;
      }
      if(name.compare(r8name) == 0){
	r8++;
      }
      if(name.compare(r9name) == 0){
	r9++;
      }
      if(name.compare(r10name) == 0){
	r10++;
      }
      if(name.compare(r11name) == 0){
	r11++;
      }
      if(name.compare(r12name) == 0){
	r12++;
      }
      if(name.compare(r13name) == 0){
	r13++;
      }
      if(name.compare(r14name) == 0){
	r14++;
      }
      if(name.compare(r15name) == 0){
	r15++;
      }

    }
    
}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "bonusHWoutput.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    OutFile.setf(ios::showbase);
    
    //Task 1: Calculate accuracy of branch predictions
    double accuracyofpredictions = (predictedcorrectly * 1.0 / totalCount * 1.0) * 100.0;

    //Task 2: Calculate ratio
    double ratio = (readcounts * 1.0 / (writecounts + readcounts) * 1.0) * 100;
    
    //Output string 
    string resultString = "Task 1: \nTotal count of branches: " + toString(totalCount)
      + "\nCount of branches predicted correctly: " + toString(predictedcorrectly)
      + "\nCount of branches predicted incorrectly: " + toString(predictedincorrectly)
      + "\nAccuracy of branch prediction: " + toString(accuracyofpredictions) + "%\n"
      + "\nTask 2: \nTotal size of memory operations: " + toString(sizeofmem) + "B \nRatio of read operations: " + toString(ratio) + "%\n"
      +"\nTask 3: \nRAX: " + toString(rax) + "\nRBX: " + toString(rbx)
      + "\nRCX: " + toString(rcx) + "\nRDX: " + toString(rdx) + "\nRSI: " + toString(rsi) + "\nRDI: " + toString(rdi) + "\nRSP: " + toString(rsp) + "\nRBP: " + toString(rbp) + "\nR8: " + toString(r8) + "\nR9: " + toString(r9) + "\nR10: " + toString(r10) + "\nR11: " + toString(r11) + "\nR12: " + toString(r12) + "\nR13: " + toString(r13) + "\nR14: " + toString(r14) + "\nR15: " + toString(r15);
    
    OutFile << resultString;
    OutFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
