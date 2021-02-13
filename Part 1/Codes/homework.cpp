#include <iostream>
#include <fstream>
#include <unordered_map>
#include <iterator>
#include "pin.H"
using std::cerr;
using std::endl;
using std::ios;
using std::ofstream;
using std::string;

ofstream OutFile;

std::ofstream *out = 0;

//static variables to initialize the number of instructions
static UINT64 totalCount = 0;
//for Task 2 
static UINT64 memoryCount = 0;
static UINT64 branchCount = 0;
static UINT64 aritmeticCount = 0;

//toString function for converting string
template <typename T>
std::string toString(T value)
{
    std::ostringstream word;
    word << value;
    return word.str();
}

// The function to count instructions, it increases variables according to type
// of instructions 
VOID docount(UINT32 i)
{
    // memory insturaction
    if (i == 0)
    {
        memoryCount++;
    }
    // branch insturaction
    else if (i == 1)
    {
        branchCount++;
    }
    // arithmetic insturaction
    else if (i == 2)
    {
        aritmeticCount++;
    }
    totalCount++;
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{

    // for memory insturaction; it sends 0 to docount function
  if (INS_IsMemoryWrite(ins) || INS_IsMemoryRead(ins))
    { 
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_UINT32, 0, IARG_END);
    }
    // for branch insturaction; it sends 1 to docount function
    else if (INS_IsBranch(ins))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_UINT32, 1, IARG_END);
    }
    // for arithmetic and logic insturaction; it sends 2 to docount function
    else
    {
        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_UINT32, 2, IARG_END);
    }
}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
                            "o", "outputFile.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    OutFile.setf(ios::showbase);
    //calculation of ratio of instruction types
    double rMemory = (memoryCount * 1.0 / totalCount * 1.0) * 100;
    double rBranch = (branchCount * 1.0 / totalCount * 1.0) * 100;
    double rAritmetic = (aritmeticCount * 1.0 / totalCount * 1.0) * 100;

    // output string to write output file
    string strResult = "Total Count of Macroinstructions and Microinstructions: " + toString(totalCount) +
    "\nCount of Memory Instructions: " + toString(memoryCount) +  "\nRatio of Memory Instructions: " + toString(rMemory) + "%" +
     "\nCount of Branch Instructions: " + toString(branchCount)+  "\nRatio of Branch Instructions: " + toString(rBranch) + "%" +
      "\nCount of Arithmetic and Logic Instructions: " + toString(aritmeticCount) +  "\nRatio of Arithmetic and Logic Instructions: " + toString(rAritmetic) + "%\n" ;
    
    OutFile << strResult;
    OutFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl
         << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv))
        return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);
    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
