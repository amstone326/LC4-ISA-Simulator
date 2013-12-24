/*
 *LC4.h
 */

struct {signed int x:5;} SEXT5;
struct {signed int x:6;} SEXT6;
struct {signed int x:7;} SEXT7;
struct {signed int x:9;} SEXT9;
struct {signed int x:11;} SEXT11;

typedef struct {

  // PC the current value of the Program Counter register
  unsigned short int PC;

  // PSR : Processor Status Register, bit[0] = P, bit[1] = Z, bit[2] = N, bit[15] = privilege bit
  unsigned short int PSR;

  // Machine registers - all 8
  unsigned short int R[8];

  // Machine memory - all of it
  unsigned short int memory[65536];

} MachineState;


typedef struct {
  // Note that all of the control signals are represented as unsigned 8 bit values although none of them use more than 3 bits
  // You should use the lower bits of the fields to store the mandated control bits. Please refer to the LC4 information 

  unsigned char PCMux_CTL; 
  unsigned char rsMux_CTL;
  unsigned char rtMux_CTL;
  unsigned char rdMux_CTL;
  unsigned char regFile_WE;
  unsigned char regInputMux_CTL;
  unsigned char Arith_CTL;
  unsigned char ArithMux_CTL;
  unsigned char LOGIC_CTL;
  unsigned char LogicMux_CTL;
  unsigned char SHIFT_CTL;
  unsigned char CONST_CTL;
  unsigned char CMP_CTL;
  unsigned char ALUMux_CTL;
  unsigned char NZP_WE;
  unsigned char DATA_WE;
  unsigned char Privilege_CTL;

} ControlSignals;

/*
 * This structure models most of the major signals in the Single Cycle LC4 Datapath
 */
typedef struct {

  unsigned short int RS;             // RS output of register file
  unsigned short int RT;             // RT output of register file
  unsigned short int ArithmeticOps;  // Output of Arithmetic Ops module
  unsigned short int LogicalOps;     // Output of the LogicalOps module
  unsigned short int Shifter;        // Output of Shifter module
  unsigned short int Constants;      // Output of the Constants module
  unsigned short int Comparator;     // Output of the Comparator module
  unsigned short int ALUMux;         // Output of the ALU Mux
  unsigned short int regInputMux;    // Output of regInputMux
  unsigned short int PCMux;          // Output of the PCMux

} DatapathSignals;

/*
 * You must implement the following functions in LC4.c. You can add additional ones if you like
 */


// Fill in control signals based on the current instruction
int DecodeCurrentInstruction (unsigned short int INSN, ControlSignals *theControls);

// Simulate the action of the datapath
int SimulateDatapath (ControlSignals *theControls, MachineState *theMachineState, DatapathSignals *theDatapath);

// Update Machine State based on the prevailing control and Datapath Signals
int UpdateMachineState (ControlSignals *theControls, MachineState *theMachineState, DatapathSignals *theDatapath);

// Reset the machine state as Pennsim would do
void Reset (MachineState *theMachineState);
