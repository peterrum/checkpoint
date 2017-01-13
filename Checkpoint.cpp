#include "Checkpoint.h"

Checkpoint::Checkpoint(Parameters & parameters, std::string folder,
		std::string chkp_name) :
		_parameters(parameters), header("HEADER"), ascii("ASCII"), binary("BIN") {
	std::stringstream ss("");
	ss << "mkdir -p " << folder;
	const int dir_err = system(ss.str().c_str());
	if (-1 == dir_err) {
		ss.str("");
		ss << "Cannot create " << folder << " folder!";
		perror(ss.str().c_str());
		exit(1);
	}
	_localsize = parameters.parallel.localSize;

	_chkp_name = folder.append(chkp_name);

}

/*
 *
 */
void Checkpoint::add(Field<FLOAT> & field, std::string name) {
	field_data fd = { name, field };
	_data_list.push_back(fd);
	chk_print("Added Field " + name);
}

std::string Checkpoint::write_ascii(FLOAT time, int iteration) {
	//Open file
	std::stringstream ss("");
	ss << _chkp_name << "_" << iteration << "_" << _parameters.parallel.rank;
	std::ofstream output(ss.str().c_str(), std::ios::out | std::ios::binary);
	if (!output.is_open()) {
		std::cerr << "Cannot open file " << _chkp_name.c_str() << " !"
				<< std::endl;
		return "ERROR";
	}
//	std::cout << "File " << ss.str() << " has been opened"
//			<< std::endl;

	//write header information to file
	output << header << std::endl;
	output << ascii << " " << time << " " << iteration << " "
			<< _parameters.parallel.localSize[0] << " "
			<< _parameters.parallel.localSize[1] << " "
			<< _parameters.parallel.localSize[2] << std::endl;

	//loop over each element in the list
	std::list<field_data>::const_iterator it;
	for (it = _data_list.begin(); it != _data_list.end(); ++it) {
		std::cout << (*it).name << std::endl;
		output << (*it).name << std::endl;

		//try cast it to scalarfield
		try {
			ScalarField & sf = dynamic_cast<ScalarField &>((*it).field);

			if (_parameters.geometry.dim == 2) {
				for (int i = 0; i < sf.getNx(); i++) {
					for (int j = 0; j < sf.getNy(); j++) {
						output << sf.getScalar(i, j) << " ";
					}
				}
			} else {
				for (int i = 0; i < sf.getNx(); i++) {
					for (int j = 0; j < sf.getNy(); j++) {
						for (int k = 0; k < sf.getNz(); k++) {
							output << sf.getScalar(i, j, k) << " ";
						}
					}
				}
			}

		} catch (const std::bad_cast& e) {
			try {
				VectorField & vf = dynamic_cast<VectorField &>((*it).field);

				if (_parameters.geometry.dim == 2) {
					for (int i = 0; i < vf.getNx(); i++) {
						for (int j = 0; j < vf.getNy(); j++) {
							output << vf.getVector(i, j)[0] << " "
									<< vf.getVector(i, j)[1] << " ";
						}
					}
				} else {
					for (int i = 0; i < vf.getNx(); i++) {
						for (int j = 0; j < vf.getNy(); j++) {
							for (int k = 0; k < vf.getNz(); k++) {
								output << vf.getVector(i, j, k)[0] << " "
										<< vf.getVector(i, j, k)[1] << " "
										<< vf.getVector(i, j, k)[1] << " ";
							}
						}
					}
				}

			} catch (const std::bad_cast& e) {
				std::cerr << "Field " << (*it).name
						<< " is not a Scalar or Vector field. Creating checkpoint failed!";
				output.close();
				return "ERROR";
			}
		}

		output << std::endl;
	}

	output.close();

	std::stringstream ss2("");
	ss2 << _chkp_name << "_" << iteration;
	return ss2.str().c_str();
}

std::string Checkpoint::write(FLOAT sim_time, int iteration) {
	//Open file
	std::stringstream ss("");
	ss << _chkp_name << "_" << iteration << "_" << _parameters.parallel.rank;
	std::ofstream output(ss.str().c_str(), std::ios::out | std::ios::binary);
	if (!output.is_open()) {
		std::cerr << "Cannot open file " << ss.str().c_str() << " !"
				<< std::endl;
		return "ERROR";
	}
//	std::cout << "File " << ss.str() << " has been opened" << std::endl;

	//write header information to file
	output << header << std::endl;
	output << binary << " " << sim_time << " " << iteration << " "
			<< _parameters.parallel.localSize[0] << " "
			<< _parameters.parallel.localSize[1] << " "
			<< _parameters.parallel.localSize[2] << std::endl;

	//loop over each element in the list
	std::list<field_data>::const_iterator it;
	for (it = _data_list.begin(); it != _data_list.end(); ++it) {
		//std::cout << (*it).name << std::endl;
		output << (*it).name << std::endl;

		//try cast it to scalarfield
		try {
			ScalarField & sf = dynamic_cast<ScalarField &>((*it).field);
			if (_parameters.geometry.dim == 2) {
				for (int i = 0; i < sf.getNx(); i++) {
					for (int j = 0; j < sf.getNy(); j++) {

						output.write(
								reinterpret_cast<const char *>(&sf.getScalar(i,
										j)), sizeof(FLOAT));
					}
				}
				//std::cout << std::endl;

			} else {
				for (int i = 0; i < sf.getNx(); i++) {
					for (int j = 0; j < sf.getNy(); j++) {
						for (int k = 0; k < sf.getNz(); k++) {
							output.write(
									reinterpret_cast<const char *>(&sf.getScalar(
											i, j, k)), sizeof(FLOAT));
						}
					}
				}
			}

		} catch (const std::bad_cast& e) {
			try {
				VectorField & vf = dynamic_cast<VectorField &>((*it).field);
				if (_parameters.geometry.dim == 2) {
					for (int i = 0; i < vf.getNx(); i++) {
						for (int j = 0; j < vf.getNy(); j++) {
							output.write(
									reinterpret_cast<const char *>(&vf.getVector(
											i, j)[0]), sizeof(FLOAT));
							output.write(
									reinterpret_cast<const char *>(&vf.getVector(
											i, j)[1]), sizeof(FLOAT));
						}
					}
				} else {
					for (int i = 0; i < vf.getNx(); i++) {
						for (int j = 0; j < vf.getNy(); j++) {
							for (int k = 0; k < vf.getNz(); k++) {
								output.write(
										reinterpret_cast<const char *>(&vf.getVector(
												i, j, k)[0]), sizeof(FLOAT));
								output.write(
										reinterpret_cast<const char *>(&vf.getVector(
												i, j, k)[1]), sizeof(FLOAT));
								output.write(
										reinterpret_cast<const char *>(&vf.getVector(
												i, j, k)[2]), sizeof(FLOAT));
							}
						}
					}
				}

			} catch (const std::bad_cast& e) {
				std::cerr << "Field " << (*it).name
						<< " is not a Scalar or Vector field. Creating checkpoint failed!";
				output.close();
				return "ERROR";
			}
		}

		output << std::endl;
	}

	output.close();

	std::stringstream ss2("");
	ss2 << _chkp_name << "_" << iteration;
	return ss2.str().c_str();
}

int Checkpoint::read(std::string filename, FLOAT &sim_time, int &iteration) {
	std::stringstream ss("");
	ss << filename << "_" << _parameters.parallel.rank;

	std::cout << "read checkpoint File: " << ss.str() << std::endl;
	if (filename.compare("") == 0) {
		std::cerr << "Filepath was empty! Aborting..." << std::endl;
		return -1;
	}

	std::fstream input(ss.str().c_str(), std::ios::in | std::ios::binary);
	if (!input.good()) {
		std::cerr << "Could not find file " << ss.str() << std::endl;
		input.close();
		return -1;
	}

	std::string line;

	getline(input, line, '\n');
	if (line.compare(header) == 0) {
		getline(input, line, ' ');
		bool bin = true;
		if (line.compare(binary) == 0) {
			bin = true;
		} else if (line.compare(ascii) == 0) {
			bin = false;
		} else {
			std::cerr << "Wrong File format" << std::endl;
			input.close();
			return -1; //wrong file format
		}
		getline(input, line, ' ');
		sim_time = (FLOAT) atof(line.c_str());
		getline(input, line, ' ');
		iteration = atoi(line.c_str());

		int x, y, z;

		getline(input, line, ' ');
		x = atoi(line.c_str());

		getline(input, line, ' ');
		y = atoi(line.c_str());

		getline(input, line, ' ');
		z = atoi(line.c_str());

		std::cout << (bin?"Binary":"ASCII") << " Checkpoint sim_time: " << sim_time
				<< " iteration: " << iteration	<< std::endl;

		if(_parameters.parallel.localSize[0] != x || _parameters.parallel.localSize[1] != y ||
				_parameters.parallel.localSize[2] != z){
			std::cerr << " size: "
			<< _parameters.parallel.localSize[0] << " == " << x << " "
			<< _parameters.parallel.localSize[1] << " == " << y << " "
			<< _parameters.parallel.localSize[2] << " == " << z << std::endl;
			input.close();
			return -1;
		}
		if (bin) {
			if (readBinary(input) != 0){
				input.close();
				return -1;
			}
		} else if (readASCII(input) != 0){
			input.close();
			return -1;
		}

	} else {
		std::cerr << "ERROR: wrong file format" << std::endl;
		input.close();
		return -1;
	}

	input.close();
	return 1;
}
int Checkpoint::readBinary(std::fstream &input) {

	//std::cout << "read BINARY" << std::endl;
	std::string line;
	while (getline(input, line)) {
		//std::cout << " while: "<< line << std::endl;
		std::list<field_data>::const_iterator it;
		for (it = _data_list.begin(); it != _data_list.end(); ++it) {
			if (line.compare((*it).name) == 0) {
				//std::cout << "READ " << (*it).name << std::endl;
				try {
					ScalarField & sf = dynamic_cast<ScalarField &>((*it).field);
					if (_parameters.geometry.dim == 2) {
						for (int i = 0; i < sf.getNx(); i++) {
							for (int j = 0; j < sf.getNy(); j++) {
								input.read(
										reinterpret_cast<char *>(&sf.getScalar(
												i, j)), sizeof(FLOAT));
							}
						}
					} else {
						for (int i = 0; i < sf.getNx(); i++) {
							for (int j = 0; j < sf.getNy(); j++) {
								for (int k = 0; k < sf.getNz(); k++) {
									input.read(
											reinterpret_cast<char *>(&sf.getScalar(
													i, j, k)), sizeof(FLOAT));
								}
							}
						}
					}

				} catch (const std::bad_cast& e) {
					try {
						VectorField & vf =
								dynamic_cast<VectorField &>((*it).field);
						if (_parameters.geometry.dim == 2) {
							for (int i = 0; i < vf.getNx(); i++) {
								for (int j = 0; j < vf.getNy(); j++) {
									input.read(
											reinterpret_cast<char *>(&vf.getVector(
													i, j)[0]), sizeof(FLOAT));
									input.read(
											reinterpret_cast<char *>(&vf.getVector(
													i, j)[1]), sizeof(FLOAT));

								}
							}
						} else {
							for (int i = 0; i < vf.getNx(); i++) {
								for (int j = 0; j < vf.getNy(); j++) {
									for (int k = 0; k < vf.getNz(); k++) {
										input.read(
												reinterpret_cast<char *>(&vf.getVector(
														i, j, k)[0]),
												sizeof(FLOAT));
										input.read(
												reinterpret_cast<char *>(&vf.getVector(
														i, j, k)[1]),
												sizeof(FLOAT));
										input.read(
												reinterpret_cast<char *>(&vf.getVector(
														i, j, k)[2]),
												sizeof(FLOAT));

									}
								}
							}
						}

					} catch (const std::bad_cast& e) {
						std::cerr << "Field " << (*it).name
								<< " is not a Scalar or Vector field. Reading checkpoint failed!";
						input.close();
						return -1;
					}
				}
			} else {
				//				happens often, but no need to do anything
			}
		}
	}
	return 0;
}

int Checkpoint::readASCII(std::fstream &input) {

	//std::cout << "read ASCII" << std::endl;
	std::string line;
	while (getline(input, line)) {
		//std::cout << " while: "<< line << std::endl;
		std::list<field_data>::const_iterator it;
		for (it = _data_list.begin(); it != _data_list.end(); ++it) {
			if (line.compare((*it).name) == 0) {
				//std::cout << "READ " << (*it).name << std::endl;
				try {
					ScalarField & sf = dynamic_cast<ScalarField &>((*it).field);
					if (_parameters.geometry.dim == 2) {
						for (int i = 0; i < sf.getNx(); i++) {
							for (int j = 0; j < sf.getNy(); j++) {
								getline(input, line, ' ');
								sf.getScalar(i, j) = atof(line.c_str());
							}
						}
					} else {
						for (int i = 0; i < sf.getNx(); i++) {
							for (int j = 0; j < sf.getNy(); j++) {
								for (int k = 0; k < sf.getNz(); k++) {
									getline(input, line, ' ');
									sf.getScalar(i, j, k) = atof(line.c_str());
								}
							}
						}
					}

				} catch (const std::bad_cast& e) {
					try {
						VectorField & vf =
								dynamic_cast<VectorField &>((*it).field);
						if (_parameters.geometry.dim == 2) {
							for (int i = 0; i < vf.getNx(); i++) {
								for (int j = 0; j < vf.getNy(); j++) {
									getline(input, line, ' ');
									vf.getVector(i, j)[0] = atof(line.c_str());
									getline(input, line, ' ');
									vf.getVector(i, j)[1] = atof(line.c_str());
								}
							}
						} else {
							for (int i = 0; i < vf.getNx(); i++) {
								for (int j = 0; j < vf.getNy(); j++) {
									for (int k = 0; k < vf.getNz(); k++) {
										getline(input, line, ' ');
										vf.getVector(i, j, k)[0] = atof(
												line.c_str());
										getline(input, line, ' ');
										vf.getVector(i, j, k)[1] = atof(
												line.c_str());
										getline(input, line, ' ');
										vf.getVector(i, j, k)[2] = atof(
												line.c_str());
									}
								}
							}
						}

					} catch (const std::bad_cast& e) {
						std::cerr << "Field " << (*it).name
								<< " is not a Scalar or Vector field. Reading checkpoint failed!";
						input.close();
						return -1;
					}
				}
			} else {
//				happens often, but no need to do anything
			}
		}
	}
	return 0;
}

void Checkpoint::chk_print(std::string msg) {
	std::cout << "Chkp[" << _parameters.parallel.rank << "] -> " << msg
			<< std::endl;
}

Checkpoint::~Checkpoint() {
}
