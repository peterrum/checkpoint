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
	Checkpoint(Parameters & parameters);
	virtual ~Checkpoint();

	void add(Field<FLOAT>& field, std::string name);
	std::string write_ascii(FLOAT time);
	std::string write(FLOAT sim_time, int iteration);
	int read(std::string filename, FLOAT &sim_time, int &iteration);

private:
	Parameters &_parameters;
	std::list<field_data> _data_list;
	std::string _chkp_name;
	int* _localsize;
	int _chkp_counter;
	int _rank;
	void chk_print(std::string msg);
	int readBinary(std::fstream &stream);
	int readASCII(std::fstream &stream);

	const std::string header;
	const std::string ascii;
	const std::string binary;
};



#endif /* STENCILS_CHECKPOINT_CHECKPOINT_H_ */
