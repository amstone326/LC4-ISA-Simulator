
/*                                                                                                   
 * You must implement the following functions in LC4.c. You can add additional ones if you like      
 */
#include "LC4.h"
#include <stdio.h>
#include <stdlib.h>

#define INSN_OPCODE(I) ((I) >> 12) //isolates bits 15-12, i.e. opcode
#define INSN_11_9(I) (((I) >> 9) & 0x7) //isolates bits 11-9 
#define INSN_8_6(I) (((I) >> 6) & 0x7) //isolates bits 8-6 (often Rs)
#define INSN_5_3(I) (((I) >> 3) & 0x7) //isolates bits 5-3 (sometimes subopcode)
#define INSN_2_0(I) ((I) & 0x7) //isolates bits 0-2 (often Rt)
#define INSN_5_0(I) ((I) & 0x3F) //isolates bits 0-5 (IMM6)
#define INSN_8_0(I) ((I) & 0x1FF) //isolates bits 0-8 (IMM9)
#define INSN_5(I) (((I) >> 5) & 0x1) //isolates bit 5
#define INSN_8_7(I) (((I) >> 7) & 0x3) //isolates bits 8-7
#define INSN_11(I) (((I) >> 11) & 0x1) //isolates bit 11
#define INSN_5_4(I) (((I) >> 4) & 0x3) //isolates bits 5-4
#define INSN_4_0(I) ((I) & 0x1F) //isolates bits 4-0
#define INSN_3_0(I) ((I) & 0xF) //isolates bits 3-0
#define INSN_7_0(I) ((I) & 0xFF) //isolates bits 7-0 (IMM8)
#define INSN_6_0(I) ((I) & 0x7F) //isolates bits 6-0 (IMM7)
#define INSN_10_0(I) ((I) & 0x7FF) //isolates bits 10-0 (IMM11)
#define INSN_15(I) ((I) >> 15) //isolates bit 15

// Fill in control signals based on the current instruction                                          
int DecodeCurrentInstruction (unsigned short int INSN, ControlSignals *theControls) {
  unsigned short int bit5;
  unsigned short int opcode;
  unsigned short int subOpcode;
  unsigned short int bits7and8;
  unsigned short int bit11;
  unsigned short int bits4and5;
  unsigned short int bit;
  unsigned short int shifted_INSN;
  int i;

  //check if INSN is valid input, i.e. contains only 0s and 1s
  shifted_INSN = INSN;
  for (i = 0; i < 16; i++) {
    bit = shifted_INSN & 0x1;
    if (bit != 0 && bit != 1) {
      printf("INSN input is invalid, contains a digit that is not 0 or 1\n");
      return 1;
    }
    shifted_INSN = shifted_INSN >> 1; 
  }

  opcode = INSN_OPCODE(INSN);
  printf("Value of OPCODE is %d\n", opcode);
  switch (opcode) {
  case 0: //0000 
    //Branch statements
    bit11 = INSN_11(INSN);
    theControls->PCMux_CTL = 0;
    theControls->regFile_WE = 0;
    theControls->NZP_WE = 0;
    theControls->DATA_WE = 0;
    theControls->rsMux_CTL = 0;
    theControls->rtMux_CTL = 0;
    theControls->rdMux_CTL = 0;
    theControls->regInputMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->ALUMux_CTL = 0;
    theControls->Privilege_CTL = 2; //leaves privilege unchanged 
    break;
  case 1: //0001 
    //Arithmetic statements
    theControls->PCMux_CTL = 1;
    theControls->rsMux_CTL = 0;
    theControls->rtMux_CTL = 0;
    theControls->rdMux_CTL = 0;
    theControls->regInputMux_CTL = 0;
    theControls->regFile_WE = 1;
    theControls->ALUMux_CTL = 0;
    theControls->NZP_WE = 1;
    theControls->DATA_WE = 0;
    bit5 = INSN_5(INSN);
    if (bit5) {
      theControls->ArithMux_CTL = 1;
    }
    else {
      theControls->ArithMux_CTL = 0;
    }
    subOpcode = INSN_5_3(INSN);
    switch (subOpcode) {
    case 0: //000
      //ADD
      theControls->Arith_CTL = 0;
      break;
    case 1: //001
      //MUL
      theControls->Arith_CTL = 1;
      break;
    case 2: //010
      //SUB
      theControls->Arith_CTL = 2;
      break;
    case 3: //011
      //DIV
      theControls->Arith_CTL = 3;
      break;
    default:
      //ADD IMM5
      theControls->Arith_CTL = 0;
      break;
    }
    //all else set to 0
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2; 
    break;
  case 2: //0010
    //Comparison Statement
    theControls->DATA_WE = 0;
    theControls->NZP_WE = 1;
    theControls->ALUMux_CTL = 4;
    theControls->regInputMux_CTL = 0;
    theControls->regFile_WE = 0;
    theControls->rsMux_CTL = 2;
    theControls->PCMux_CTL = 1;
    theControls->rtMux_CTL = 0;
    bits7and8 = INSN_8_7(INSN);
    switch (bits7and8) {
    case 0: //00
      //CMP
      theControls->CMP_CTL = 0;
      break;
    case 1: //01
      //CMPU
      theControls->CMP_CTL = 1;
      break;
    case 2: //10
      //CMPI
      theControls->CMP_CTL = 2;
      break;
    case 3: //11
      theControls->CMP_CTL = 3;
      break;
    }
    //all else set to 0
    theControls->rdMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->Privilege_CTL = 2; 
    break;
  case 4: //0100
    //JSR Statement
    theControls->DATA_WE = 0;
    theControls->NZP_WE = 1;
    theControls->regInputMux_CTL = 2;
    theControls->regFile_WE = 1;
    theControls->rdMux_CTL = 1;
    theControls->rsMux_CTL = 0;
    bit11 = INSN_11(INSN);
    if (bit11) {
      //JSR
      theControls->PCMux_CTL = 5;
    }
    else {
      //JSRR
      theControls->PCMux_CTL = 3;
    }
    //all else set to 0
    theControls->rtMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->ALUMux_CTL = 0;
    theControls->Privilege_CTL = 2; 
    break;
  case 5: //0101
    //Logical Statements
    theControls->DATA_WE = 0;
    theControls->NZP_WE = 1;
    theControls->ALUMux_CTL = 1;
    theControls->regInputMux_CTL = 0;
    theControls->regFile_WE = 1;
    theControls->rsMux_CTL = 0;
    theControls->PCMux_CTL = 1;
    theControls->rtMux_CTL = 0;
    theControls->rdMux_CTL = 0;
    bit5 = INSN_5(INSN);
    if (bit5) {
      theControls->LogicMux_CTL = 1;
    }
    else {
      theControls->LogicMux_CTL = 0;
    }
    subOpcode = INSN_5_3(INSN);
    switch (subOpcode) {
    case 0: //000
      //AND
      theControls->LOGIC_CTL = 0;
      break;
    case 1: //001
      //NOT
      theControls->LOGIC_CTL = 1;
      break;
    case 2: //010
      //OR
      theControls->LOGIC_CTL = 2;
      break;
    case 3: //011
      //XOR
      theControls->LOGIC_CTL = 3;
      break;
    default:
      //AND IMM
      theControls->LOGIC_CTL = 0; 
      break;
    }
    //all else set to 0                                                                
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2; 
    break;
  case 6: //0110
    //LDR
    theControls->PCMux_CTL = 1;
    theControls->rsMux_CTL = 0;
    theControls->rdMux_CTL = 0;
    theControls->rtMux_CTL = 0;
    theControls->regFile_WE = 1;
    theControls->regInputMux_CTL = 1;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 2;
    theControls->ALUMux_CTL = 0;
    theControls->NZP_WE = 1;
    theControls->DATA_WE = 0;
    //all else set to 0                                                                 
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2; 
    break;
  case 7: //0111
    //STR
    theControls->PCMux_CTL = 1;
    theControls->rsMux_CTL = 0;
    theControls->rtMux_CTL = 1;
    theControls->regFile_WE = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 2;
    theControls->ALUMux_CTL = 0;
    theControls->NZP_WE = 0;
    theControls->DATA_WE = 1;
    break;
    //all else set to 0
    theControls->rdMux_CTL = 0;
    theControls->regInputMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2; 
    break;
  case 8: //1000
    //RTI
    theControls->PCMux_CTL = 3;
    theControls->rsMux_CTL = 1;
    theControls->regFile_WE = 0;
    theControls->NZP_WE = 0;
    theControls->DATA_WE = 0;
    theControls->Privilege_CTL = 0; //sets privilege back to user
    //all else set to 0                                                             
    theControls->rtMux_CTL = 0;
    theControls->rdMux_CTL = 0;
    theControls->regInputMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->ALUMux_CTL = 0;
    break;
  case 9: //1001
    //CONST
    theControls->PCMux_CTL = 1;
    theControls->rdMux_CTL = 0;
    theControls->regFile_WE = 1;
    theControls->regInputMux_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->ALUMux_CTL = 3;
    theControls->NZP_WE = 1;
    theControls->DATA_WE = 0;
    //all else set to 0                                                             
    theControls->rsMux_CTL = 0;
    theControls->rtMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2;
    break;
  case 10: //1010
    //Shift Statements
    theControls->PCMux_CTL = 1;
    theControls->rsMux_CTL = 0;
    theControls->rdMux_CTL = 0;
    theControls->rtMux_CTL = 0;
    theControls->regFile_WE = 1;
    theControls->regInputMux_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->NZP_WE = 1;
    theControls->DATA_WE = 0;
    bits4and5 = INSN_5_4(INSN);
    if (bits4and5 == 3) { //11, MOD
      theControls->Arith_CTL = 4;
      theControls->ALUMux_CTL = 0;
    }
    else {
      theControls->Arith_CTL = 0;
      theControls->ALUMux_CTL = 2;
    }
    switch (bits4and5) {
    case 0: //00, SLL
      theControls->SHIFT_CTL = 0;
      break;
    case 1: //01, SRA
      theControls->SHIFT_CTL = 1;
      break;
    case 2: //10, SRL
      theControls->SHIFT_CTL = 2;;
      break;
    case 3: //11, MOD
      theControls->SHIFT_CTL = 0;
      break;
    }
    //all else set to 0
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2;
    break;
  case 12: //1100
    //JMP statements
    theControls->rsMux_CTL = 0;
    theControls->regFile_WE = 0;
    theControls->NZP_WE = 0;
    theControls->DATA_WE = 0;
    bit11 = INSN_11(INSN);
    if (bit11 == 1) {
      //JMP 
      theControls->PCMux_CTL = 2;
    }
    else {
      //JMPR
      theControls->PCMux_CTL = 3;
    }
    //all else set to 0                                                                 
    theControls->rtMux_CTL = 0;
    theControls->rdMux_CTL = 0;
    theControls->regInputMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->ALUMux_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2;
    break;
  case 13: //1101
    //HICONST
    theControls->PCMux_CTL = 1;
    theControls->rsMux_CTL = 2;
    theControls->rdMux_CTL = 0;
    theControls->regFile_WE = 1;
    theControls->regInputMux_CTL = 0;
    theControls->CONST_CTL = 1;
    theControls->ALUMux_CTL = 3;
    theControls->NZP_WE = 1;
    theControls->DATA_WE = 0;
    //all else set to 0                                                                 
    theControls->rtMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->Privilege_CTL = 2;
    break;
  case 15: //1111 
    //TRAP
    theControls->PCMux_CTL = 4;
    theControls->rdMux_CTL = 1;
    theControls->regFile_WE = 1;
    theControls->regInputMux_CTL = 2;
    theControls->NZP_WE = 1;
    theControls->DATA_WE = 0;
    theControls->Privilege_CTL = 1; //sets privilege to OS mode
    //all else set to 0                                                                 
    theControls->rsMux_CTL = 0;
    theControls->rtMux_CTL = 0;
    theControls->Arith_CTL = 0;
    theControls->ArithMux_CTL = 0;
    theControls->LOGIC_CTL = 0;
    theControls->LogicMux_CTL = 0;
    theControls->SHIFT_CTL = 0;
    theControls->CONST_CTL = 0;
    theControls->CMP_CTL = 0;
    theControls->ALUMux_CTL = 0;
    break;
  case 16:
    //No valid opcodes were entered, opcode must be invalid
    printf("Invalid INSN given, opcode is invalid.\n");
    return 1;
  }
  return 0;
}

// Simulate the action of the datapath                                                   
int SimulateDatapath (ControlSignals *theControls, MachineState *theMachineState, DatapathSignals *theDatapath) {
  unsigned short int INSN;
  unsigned short int operand_1;
  unsigned short int operand_2;
  unsigned short int mem_address;
  unsigned short int shift_amount;
  unsigned short int curr_PC; 
  unsigned char register_number;
  unsigned char nzp_bits;
  unsigned char bits_11_9;
  signed short int difference;
  signed short int signed_operand_1;
  signed short int signed_operand_2;

  INSN = theMachineState->memory[theMachineState->PC];

  //set RS
  switch (theControls->rsMux_CTL) {
  case 0:
    register_number = INSN_8_6(INSN);
    theDatapath->RS = theMachineState->R[register_number];
    break;
  case 1:
    register_number = 0x7;
    theDatapath->RS = theMachineState->R[register_number];
    break;
  case 2:
    register_number = INSN_11_9(INSN);
    theDatapath->RS = theMachineState->R[register_number];
    break;
  default:
    printf("Error in setting RS: rsMux_CTL was set to invalid value.\n"); 
  }

  printf("RS SET TO: %d\n", theDatapath->RS);

  //set RT
  switch (theControls->rtMux_CTL) {
  case 0: 
    register_number = INSN_2_0(INSN);
    theDatapath->RT = theMachineState->R[register_number];
    break;
  case 1: 
    register_number = INSN_11_9(INSN);
    theDatapath->RT = theMachineState->R[register_number];
    break;
  default:
    printf("Error in setting RT: rtMux_CTL was set to invalid value.\n"); 
  }

  printf("RT SET TO: %d\n", theDatapath->RT);

  //set Arithmetic Ops
  operand_1 = theDatapath->RS;
  signed_operand_1 = (signed short int) theDatapath->RS;
  switch (theControls->ArithMux_CTL) {
  case 0: 
    operand_2 = theDatapath->RT;
    signed_operand_2 = (signed short int) theDatapath->RT;
    break;
  case 1:
    signed_operand_2 = SEXT5.x = (signed short int) INSN_4_0(INSN);
    break;
  case 2: 
    signed_operand_2 = SEXT6.x = (signed short int) INSN_5_0(INSN);
    break;
  }

  switch (theControls->Arith_CTL) {
  case 0:
    //ADD
    printf("operand 1 is %d\n", signed_operand_1);
    printf("operand 2 is %d\n", signed_operand_2);
    theDatapath->ArithmeticOps = signed_operand_1 + signed_operand_2;
    printf("OUTPUT FROM ADDITION: %d\n", (signed short) theDatapath->ArithmeticOps);
    break;
  case 1:
    //MUL
    printf("operand 1 is %d\n", signed_operand_1);
    printf("operand 2 is %d\n", signed_operand_2);
    theDatapath->ArithmeticOps = signed_operand_1 * signed_operand_2;
    printf("OUTPUT FROM MULTIPLICATION: %d\n",(signed short) theDatapath->ArithmeticOps);
    break;
  case 2: 
    //SUB
    theDatapath->ArithmeticOps = signed_operand_1 - signed_operand_2;
    printf("OUTPUT FROM SUBTRACTION: %d\n", theDatapath->ArithmeticOps);
    break;
  case 3:
    //DIV
    if (operand_2 == 0) {
      theDatapath->ArithmeticOps = 0;
    }
    else {
      theDatapath->ArithmeticOps = operand_1 / operand_2;
    }
    printf("OUTPUT FROM DIVIDE: %d\n", theDatapath->ArithmeticOps);
    break;
  case 4:
    //MOD
    theDatapath->ArithmeticOps = operand_1 % operand_2;
    break;
  }

  //set LogicalOps
  operand_1 = theDatapath->RS;
  switch(theControls->LogicMux_CTL) {
  case 0:
    operand_2 = theDatapath->RT;
    signed_operand_2 = (signed short int) theDatapath->RT;
    break;
  case 1: 
    signed_operand_2 = SEXT5.x = (signed short int) INSN_4_0(INSN);
    break;
  }
  switch (theControls->LOGIC_CTL) {
  case 0:
    //AND
    theDatapath->LogicalOps = operand_1 & signed_operand_2;
    break;
  case 1: 
    //NOT
    theDatapath->LogicalOps = ~operand_1;
    break;
  case 2: 
    //OR
    theDatapath->LogicalOps = operand_1 | operand_2;
    break;
  case 3:
    //XOR
    theDatapath->LogicalOps = operand_1 ^ operand_2;
    break;
  }

  //set Shifter
  operand_1 = theDatapath->RS;
  signed_operand_1 = (signed short int) theDatapath->RS;
  shift_amount = INSN_3_0(INSN);
  switch(theControls->SHIFT_CTL) {
  case 0:
    //SLL
    theDatapath->Shifter = operand_1 << shift_amount;
    break;
  case 1: 
    //SRA
    theDatapath->Shifter = signed_operand_1 >> shift_amount;
    break;
  case 2: 
    //SRL
    theDatapath->Shifter = operand_1 >> shift_amount;
    break;
  }

  //set Constants
  switch(theControls->CONST_CTL) {
  case 0:
    //CONST
    theDatapath->Constants = SEXT9.x = (signed short int) INSN_8_0(INSN);
    printf("Value in datapath->Constants is %d\n", theDatapath->Constants);
    break;
  case 1: 
    //HICONST
    operand_1 = theDatapath->RS & 0xFF;
    operand_2 = (INSN_7_0(INSN)) << 8;
    theDatapath->Constants = operand_1 | operand_2;
    break;
  }

  //set Comparator
  switch (theControls->CMP_CTL) {
  case 0:
    //signed-CC(RS-RT)
    signed_operand_1 = theDatapath->RS;
    signed_operand_2 = theDatapath->RT;
    difference = signed_operand_1 - signed_operand_2;
    if (difference < 0) {
      theDatapath->Comparator = -1;
    }
    else if (difference == 0) {
      theDatapath->Comparator = 0;
    }
    else {
      theDatapath->Comparator = 1;
    }
    break;
  case 1:
    //unsigned--CC(RS-RT)
    difference = theDatapath->RS - theDatapath->RT;    
    if (difference < 0) {
      theDatapath->Comparator = -1;
    }
    else if (difference == 0) {
      theDatapath->Comparator = 0;
    }
    else {
      theDatapath->Comparator = 1;
    }
    break;
  case 2: 
    //signed-CC(RS-SEXT(IMM7))
    signed_operand_1 = theDatapath->RS;
    signed_operand_2 = SEXT7.x = INSN_6_0(INSN);
    difference = signed_operand_1 - signed_operand_2;
    if (difference < 0) {
      theDatapath->Comparator = -1;
    }
    else if (difference == 0) {
      theDatapath->Comparator = 0;
    }
    else {
      theDatapath->Comparator = 1;
    }
    break;
  case 3: 
    //unsigned-CC(RS-UIMM7)
    operand_2 = INSN_6_0(INSN);
    difference = theDatapath->RS - operand_2;
    if (difference < 0) {
      theDatapath->Comparator = -1;
    }
    else if (difference == 0) {
      theDatapath->Comparator = 0;
    }
    else {
      theDatapath->Comparator = 1;
    }
    break;
  }
 
  //set ALUMux
  switch (theControls->ALUMux_CTL) {
  case 0: 
    theDatapath->ALUMux = theDatapath->ArithmeticOps;
    printf("ALUMux set to ARITHMETIC OPS\n");
    break;
  case 1: 
    theDatapath->ALUMux = theDatapath->LogicalOps;
    printf("ALUMux set to LOGICAL OPS\n");
    break;
  case 2: 
    theDatapath->ALUMux = theDatapath->Shifter;
    printf("ALUMux set to SHIFT OPS\n");
    break;
  case 3:
    theDatapath->ALUMux = theDatapath->Constants;
    printf("ALUMux set to CONST OPS\n");
    break;
  case 4: 
    theDatapath->ALUMux = theDatapath->Comparator;
    printf("ALUMux set to COMP OPS\n");
    break;
  default: 
    printf("ALUMux was set to an invalid number");
  }

  printf("VALUE OF ALUMUX set to: %d\n", theDatapath->ALUMux);
  
  //set regInputMux
  printf("RegInputControl is %d\n", theControls->regInputMux_CTL);
  switch (theControls->regInputMux_CTL) {
  case 0:
    //regInput is output of ALUMux
    theDatapath->regInputMux = theDatapath->ALUMux;
    break;
  case 1:
    //regInput is output of Datamem
    signed_operand_1 = SEXT6.x = INSN_5_0(INSN);
    mem_address = signed_operand_1 + theDatapath->RS;
    theDatapath->regInputMux = theMachineState->memory[mem_address];
    break;
  case 2: 
    theDatapath->regInputMux = theMachineState->PC + 1;
    break;
  }

  printf("REGINPUTMUX set to %d\n", theDatapath->regInputMux);

  //set PCMux
  curr_PC = theMachineState->PC;
  printf("value of PCMUX_CTL is %d\n", theControls->PCMux_CTL);
  switch (theControls->PCMux_CTL) {
  case 0:
    //compare to NZP
    nzp_bits = INSN_2_0(theMachineState->PSR);
    printf("PSR curr NZP bits are %d\n", nzp_bits);
    bits_11_9 = INSN_11_9(INSN);
    if (nzp_bits & bits_11_9) {
      printf("NZP TEST PASSES\n");
      signed_operand_1 = SEXT9.x = (signed short int) INSN_8_0(INSN);
      theDatapath->PCMux = curr_PC + 1 + signed_operand_1;
      printf("Next PC was set to %d\n", theDatapath->PCMux);
    }
    else {
      theDatapath->PCMux = curr_PC + 1;
    }
    break;
  case 1:
    theDatapath->PCMux = curr_PC + 1;
    break;
  case 2:
    signed_operand_1 = SEXT11.x = (signed short int) INSN_10_0(INSN);
    theDatapath->PCMux = curr_PC + 1 + signed_operand_1;
    break;
  case 3: 
    theDatapath->PCMux = theDatapath->RS;
    break;
  case 4:
    theDatapath->PCMux = (0x8000 | INSN_7_0(INSN));
    break;
  case 5:
    signed_operand_1 = (INSN_10_0(INSN) << 4);
    theDatapath->PCMux = ((theMachineState->PC) & 0x8000) | signed_operand_1;
    break;
  }

  return 0;
}

// Update Machine State based on the prevailing control and Datapath Signals                        
int UpdateMachineState (ControlSignals *theControls, MachineState *theMachineState, DatapathSignals *theDatapath) {
  unsigned int curr_PC;
  unsigned int curr_privilege;
  unsigned int updated_PC;
  unsigned int updated_privilege_bit;
  unsigned int register_d_number;
  signed short int mem_offset;
  signed short int signed_regInputMux;
  unsigned int mem_address;
  int i;
  
  unsigned short INSN = theMachineState->memory[theMachineState->PC];

  //check for exception type 1
  curr_PC = theMachineState->PC;
  if ((curr_PC > 0x3FFF && curr_PC < 0x8000) || (curr_PC > 0x9FFF)) {
    printf("Exception type 1: Attempting to execute a data section as code");
    return 1;
  }

  curr_privilege = INSN_15(theMachineState->PSR);
  printf("CURRENT PRIVILEGE: %d\n", curr_privilege);

  //update memory and check for exception type 2 
  if (theControls->DATA_WE) {
    mem_offset = SEXT6.x = (signed short int) INSN_5_0(INSN);
    mem_address = mem_offset + theDatapath->RS;
    //check for exception type 2
    if (curr_privilege == 1) { //if in OS mode, should be in user data or OS data
      if (!(mem_address > 0x3FFF && mem_address < 0x8000) && !(mem_address > 0x9FFF)) {
	printf("Exception type 2: Attempting to read or write a code section as data\n");
	return 2;
      }
    }
    else { //if in user mode, must be in user data
      printf("\n\n\n\n");
      printf("mem address when error checking is %04x\n", mem_address);
      if (!(mem_address > 0x3FFF && mem_address < 0x8000)) {
	printf("Exception type 2: Attempting to read or write a code section as data\n");
	return 2;
      }
    }
    theMachineState->memory[mem_address] = theDatapath->RT;
  }

  //check for exception type 3
  if (curr_privilege == 0) { //if in user mode
    if (curr_PC > 0x7FFF) {
      printf("Exception type 3: Attempting to OS section of memory in user mode.\n");
      return 3;
    }
  }    

 /********************************* updates */
   
  //update NZP
  if (theControls->NZP_WE) {
    signed_regInputMux = (signed short int) theDatapath->regInputMux;
    if (signed_regInputMux < 0) { //set to N, i.e. 100
      printf("NZP bits set to negative\n");
      theMachineState->PSR = (theMachineState->PSR & 0xFFF8); //clears lower 3 bits
      theMachineState->PSR = (theMachineState->PSR | 0x0004); //FFF + 1100
    }
    else if (signed_regInputMux == 0) { //set to Z, i.e. 010
      theMachineState->PSR = (theMachineState->PSR & 0xFFF8); //clears lower 3 bits
      theMachineState->PSR = (theMachineState->PSR | 0x0002);
      printf("NZP bits set to zero\n");
    } 
    else { //set to P, i.e. 001
      theMachineState->PSR = (theMachineState->PSR & 0xFFF8); //clears lower 3 bits
      theMachineState->PSR = (theMachineState->PSR | 0x0001); //FFF + 1001
      printf("NZP bits set to positive\n");
    }
  }
      
  //update privilege bit
  switch (theControls->Privilege_CTL) {
  case 0: //set to 0, i.e. user mode
    theMachineState->PSR = (theMachineState->PSR & 0x7FFF); //and uppermost bit with 0, and all else w/ 1 to maintain
    break;
  case 1: //set to 1, i.e. OS mode
    theMachineState->PSR = (theMachineState->PSR | 0x8000); //or uppermost with 1, or all else w/ 0 to maintain
    break;
  case 2: //do nothing
   break;
  }

  //update registers
  printf("Val of regFile_WE is %d\n", theControls->regFile_WE);
  if (theControls->regFile_WE) {
    switch (theControls->rdMux_CTL) {
    case 0:
      register_d_number = INSN_11_9(INSN);
      break;
    case 1:
      register_d_number = 0x7;
      break;
    }
    theMachineState->R[register_d_number] = theDatapath->regInputMux;
    for (i = 0; i < 8; i++) {
      printf("The value in register %d is %d\n", i, theMachineState->R[i]);
    }
  }

  //update PC
  theMachineState->PC = theDatapath->PCMux;
 
  return 0;
}

// Reset the machine state as Pennsim would do                                                      
void Reset (MachineState *theMachineState) {
  int i;

  //reset PC
  theMachineState->PC = 0x8200;

  //reset Registers
  for (i = 0; i < 8; i++) {
    theMachineState->R[i] = 0;
  }

  //reset PSR
  theMachineState->PSR = 0x8002;

  //reset memory
  for (i = 0; i < 0xFFFF; i++) {
    theMachineState->memory[i] = 0;
  }
}
