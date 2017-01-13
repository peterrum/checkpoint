#ifndef STENCILS_CHECKPOINT_CHECKPOINT_H_
#define STENCILS_CHECKPOINT_CHECKPOINT_H_

#include "../Parameters.h"
#include "../DataStructures.h"
#include <list>
#include <iterator>
#include <iostream>
#include <fstream>
#include <ostream>
#include <sstream>
#include <typeinfo>
#include <string>

struct field_data{
	std::string name;
	Field<FLOAT>& field;
};

typedef unsigned char byte;

class Checkpoint {
public:

	/**
	 * folder : will be created if it is not already present, can be used to write to $SCRATCH or in some subfolder
	 * chkpName : filename of the checkpoint files, it will be extended by <iteration>_<rank>
	 */
	Checkpoint(Parameters & parameters, std::string folder, std::string chkpName);
	virtual ~Checkpoint();

	/**
	 * field : pointer to the data field that should occure in the checkpoint
	 * name : some unique identifier for the field, this must stay unchanged between the write and read of checkpoints
	 */
	void add(Field<FLOAT>& field, std::string name);

	/**
	 * writes the fields to the file <folder><filename>_iteration_rank
	 * time : needs to be set to restart the simulation at the correct time
	 */
	std::string write_ascii(FLOAT time, int iteration);

	/**
	 * writes the fields to the file <folder><filename>_iteration_rank
	 * The data itself is written in binary
	 * time : needs to be set to restart the simulation at the correct time
	 */
	std::string write(FLOAT sim_time, int iteration);

	/**
	 * reads the file specified by filename, it can be either a pure ASCII-File or a binary file
	 */
	int read(std::string filename, FLOAT &sim_time, int &iteration);

private:
	Parameters &_parameters;
	std::list<field_data> _data_list;
	std::string _chkp_name;
	int* _localsize;
	void chk_print(std::string msg);
	int readBinary(std::fstream &stream);
	int readASCII(std::fstream &stream);

	const std::string header;
	const std::string ascii;
	const std::string binary;
};



#endif /* STENCILS_CHECKPOINT_CHECKPOINT_H_ */
