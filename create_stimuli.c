/*!
 *  \file    create_stimuli.c
 *  \author  Tycho Jobsis
 *  \date    14-09-2021
 *  \version 1
 *
 *  \brief   A program to create stimuli for a VHDL testbench  
 *
 *  \details This program is used to create stimuli for the VHDL testbench generator from <a href="https://dolman-wim.nl/di/tools/mtb/index.php">Wim Dolman</a>. 
 * 
 * 	\note call the program as follows: ./create_stimuli <output file name> <delay time> <stimuli 1> <stimuli ...>
 */

/*!	\copyright
 *
 *	MIT License
 *
 *	Copyright (c) 2021 TychoJ
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *
 *	The above copyright notice and this permission notice shall be included in all
 *	copies or substantial portions of the Software.
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *	SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define EXTENSION ".vhdl"

// function prototypes 
int isFinished(uint64_t binary, int nbrStimuli);
int changedState(uint64_t binary, int bit);

int main(int argc, char const *argv[]) {

	if(argc < 3) {
		printf("%s <File name> <delay> <stimuli 1>", argv[0]);
		return EXIT_FAILURE;
	}
	int nbrStimuli = argc - 3;	//<! number of stimuli
	char *fileName;
	char *delay;
	fileName	= (char *)malloc( (strlen(argv[1]) + strlen(EXTENSION)) * sizeof(char));
	delay 		= (char *)malloc( (strlen(argv[2]) + 1) * sizeof(char));
	if(fileName == NULL || delay == NULL) {
		printf("Not enough memory\n");
		return EXIT_FAILURE;
	}
	strcpy(delay, argv[2]);
	strcpy(fileName, argv[1]);
	strcat(fileName, EXTENSION);
	strcat(delay, "\0");
	strcat(fileName, "\0");

	remove(fileName);

	FILE *output;
	output = fopen(fileName, "r");
	fclose(output);

	uint64_t binary = 0ULL;

	output = fopen(fileName, "a");
	fprintf(output, "--\n--  gebruik dit als eigen declaratie\n--  constant %s : time := tijd(standaard 50 ns);\n--\n", delay);
	fprintf(output, "\n\n");
	fprintf(output, "signal_generator: process is\nbegin\n");
	fclose(output);

	while(isFinished(binary, nbrStimuli)) {
		output = fopen(fileName, "a");
		if(output == NULL) {
			printf("probleem met file\n");
			exit(EXIT_FAILURE);
		}
		for(int i = 0; i < nbrStimuli; i++) {
			if(changedState(binary, i)) {
				fprintf(output, "  %s <= '%d';\n", argv[i + 3], !!(binary & (1ULL << i)));
			}
		}
		fprintf(output, "  wait for %s;\n", delay);
		fclose(output);
		printf("aantal mogelijkheden: %lld\n", binary);
		binary++;
	}
	output = fopen(fileName, "a");
	fprintf(output, "  wait;\nend process signal_generator;");
	return 0;
}

/*! \brief  return if program has added all binary options
 *
 *  \param  binary		binary options
 *	\param	nbrStimuli	the number of stimuli
 *
 *  \return 0 if all binary options are 1 else 0
 */
int isFinished(uint64_t binary, int nbrStimuli) {
	if(binary & (1ULL << nbrStimuli)) return 0;
	return 1;
}

/*! \brief  return if the signal will change state
 *
 *  \param  binary		binary options
 *	\param	bit			the bit that will be checked
 *
 *  \return 0 if the signal stay's the same otherwise 1
 */
int changedState(uint64_t binary, int bit) {
	if( (binary & (1ULL << bit)) == ((binary - 1) & (1ULL << bit)) ) return 0;
	return 1;
}