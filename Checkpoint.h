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

struct field_data{
	std::string name;
	Field<FLOAT>& field;
};

class Checkpoint {
public:
	Checkpoint(Parameters & parameters);
	virtual ~Checkpoint();

	void add(Field<FLOAT>& field, std::string name);
	int write_ascii();
	int read();

private:
	Parameters &_parameters;
	std::list<field_data> _data_list;
	std::string _chkp_name;
	int* _localsize;
	int _chkp_counter;
	int _rank;
	void chk_print(std::string msg);
};



#endif /* STENCILS_CHECKPOINT_CHECKPOINT_H_ */