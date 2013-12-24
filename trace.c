#include <stdlib.h>
#include <stdlib.h>
#include "ObjectFiles.h" 

int main(int argc, char** argv) {
  //initialize 3 basic fields
  MachineState *theMachineState;
  ControlSignals *theControlSignals;
  DatapathSignals *theDatapath;
  //other variables
  FILE *output_file;
  char *obj_file_name;
  int i, num_obj_files;
  unsigned short instruction;
  unsigned int exception_check;
  unsigned short curr_PC;
  unsigned int valid_instruction_check;

  theMachineState = malloc(sizeof(MachineState));
  theControlSignals = malloc(sizeof(ControlSignals));
  theDatapath = malloc(sizeof(DatapathSignals));

  //initialize state (reset)
  Reset(theMachineState);

  //read object files and load into memory
  output_file = fopen(argv[1], "w");
  num_obj_files = argc - 2;
  for (i = 0; i < num_obj_files; i++) {
    obj_file_name = argv[i+2];
    ReadObjectFile(obj_file_name, theMachineState);
  }

  //simulate one instruction at a time and print instruction / PC
  while (theMachineState->PC != 0x80FF) {
    instruction = theMachineState->memory[theMachineState->PC];
    curr_PC = theMachineState->PC;

    valid_instruction_check = DecodeCurrentInstruction(instruction, theControlSignals);
    if (valid_instruction_check == 1) {
      break;
    }
    SimulateDatapath(theControlSignals, theMachineState, theDatapath);
    exception_check = UpdateMachineState(theControlSignals, theMachineState, theDatapath);
    if (exception_check > 0) {
      break;
    }
    //print to output
    fwrite(&curr_PC, sizeof(unsigned short), 1, output_file);
    fwrite(&instruction, sizeof(unsigned short), 1, output_file);
    printf("-----------------------------------------------\n");
  }
  //write last PC and instruction values if the loop did NOT end with an exception or error
  if (exception_check == 0 && valid_instruction_check == 0) {
    fwrite(&theMachineState->PC, sizeof(unsigned short), 1, output_file);
    instruction = theMachineState->memory[theMachineState->PC];
    fwrite(&instruction, sizeof(unsigned short), 1, output_file);
  }
  
  free(theMachineState);
  free(theControlSignals);
  free(theDatapath);

  return 0;
}
