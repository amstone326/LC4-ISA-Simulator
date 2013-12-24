#include "ObjectFiles.h"
#define COMBINE_BYTES(I1, I2) (((I1) << 8) | (I2))

int ReadObjectFile(char *filename, MachineState *theMachineState) {
  unsigned short int byte1;
  unsigned short int byte2;
  unsigned short int section_header;
  unsigned short int n;
  FILE *the_file;
  int unsuccessful_read;

  the_file = fopen (filename, "r");
  if (the_file == NULL) {
    perror("Error opening file.\n");
    return 1;
  }

  while (1) {
    byte1 = fgetc(the_file);
    byte2 = fgetc(the_file);
    //reached end of info 
    if (byte1 == 0xFFFF || byte2 == 0xFFFF) {
      break;
    }
    if (feof(the_file)) {
      break;
    }
    section_header = COMBINE_BYTES(byte1, byte2);
    printf("NEXT SECTION HEADER: %04x\n", section_header);

    //DATA OR CODE SECTIONS
    if (section_header == 0xCADE || section_header == 0xDADA) {
      unsuccessful_read = parseCodeOrData(the_file, theMachineState);
      if (unsuccessful_read) {
	printf("Error reading file in data or code section- more information expected.\n");
	fclose(the_file);
	return 1;
      }
    }
 
    //SYMBOL SECTION
    else if (section_header == 0xC3B7) {
      //read address
      byte1 = fgetc(the_file);
      byte2 = fgetc(the_file);
      if (byte1 == 0xFFFF || byte2 == 0xFFFF) {
	printf("Error reading file in symbol section- more information expected.\n");
	fclose(the_file);
	return 1;
      }
      //read n
      byte1 = fgetc(the_file);
      byte2 = fgetc(the_file);
      if (byte1 == 0xFFFF || byte2 == 0xFFFF) {
	printf("Error reading file in symbol section- more information expected.\n");
	fclose(the_file);
	return 1;
      }
      n = COMBINE_BYTES(byte1, byte2);
      unsuccessful_read = readNChars(n, the_file);
      if (unsuccessful_read) {
	printf("Error reading file in symbol section- more information expected.\n");
	fclose(the_file);
	return 1;
      }
    }

    //FILE NAME SECTION
    else if (section_header == 0xF17E) {
      //read n;
      byte1 = fgetc(the_file);
      byte2 = fgetc(the_file);
      if (byte1 == 0xFFFF || byte2 == 0xFFFF) {
	printf("Error reading file in filename section- more information expected.\n");
	fclose(the_file);
	return 1;
      }
      n = COMBINE_BYTES(byte1, byte2);
      unsuccessful_read = readNChars(n, the_file);
      if (unsuccessful_read) {
	printf("Error reading file in filename section- more information expected.\n");
	fclose(the_file);
	return 1;
      }
    }

    //LINE NUMBER SECTION
    else if (section_header == 0x715E) {
      unsuccessful_read = readNChars(6, the_file); //read 6 chars, i.e. 3 words
      if (unsuccessful_read) {
	fclose(the_file);
	printf("Error reading file in line number section- more information expected.\n");
	return 1;
      }
    }

    //ERROR
    else {
      printf("Error encountered reading file, invalid section header: %04x\n", section_header);
      fclose(the_file);
      return 1;
    }
  }

  fclose (the_file);  
  return 0;
}

//returns 0 if successful, 1 otherwise
int readNChars(int n, FILE *the_file) {
  unsigned short int byte;
  int i;
  for (i = 0; i < n; i++) {
    byte = fgetc(the_file);
    if (byte == 0xFFFF) {
      return 1;
    }
  }
  return 0;
}

//returns 0 if sucessful, 1 otherwise
int parseCodeOrData(FILE *the_file, MachineState *theMachineState) {
  unsigned short int byte1;
  unsigned short int byte2;
  unsigned short int mem_addr;
  unsigned short int instruction;
  unsigned short n;
  int i;

  //read address
  byte1 = fgetc(the_file);
  byte2 = fgetc(the_file);
  
  if (byte1 == 0xFFFF || byte2 == 0xFFFF) {
    return 1;
  }
  mem_addr = COMBINE_BYTES(byte1, byte2);
  //read n
  byte1 = fgetc(the_file);
  byte2 = fgetc(the_file);
  if (byte1 == 0xFFFF || byte2 == 0xFFFF) {
    return 1;
  }
  n = COMBINE_BYTES(byte1, byte2);

  //read n words into memory
  for (i = 0; i < n; i++) {
    byte1 = fgetc(the_file);
    byte2 = fgetc(the_file);
    if (byte1 == 0xFFFF || byte2 == 0xFFFF) {
      return 1;
    }
    instruction = COMBINE_BYTES(byte1, byte2);
    theMachineState->memory[mem_addr] = instruction;
    mem_addr++;
  }
  return 0;
}



