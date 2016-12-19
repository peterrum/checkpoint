#include "Checkpoint.h"


Checkpoint::Checkpoint(Parameters & parameters):
_parameters(parameters),
_chkp_counter(0){
	MPI_Comm_rank(MPI_COMM_WORLD, &_rank);
	const int dir_err = system("mkdir -p checkpoint/data");
	if (-1 == dir_err) {
			perror("Cannot create checkpoint/data folder!");
			exit(1);
	}
	_localsize = parameters.parallel.localSize;

	std::stringstream tmp_chk_name("");
	tmp_chk_name<<"checkpoint/data/chkp_"
				<<parameters.simulation.scenario
				<<"_"<<_localsize[0]<<"x"<<_localsize[1]<<"x"<<_localsize[2];
	_chkp_name = tmp_chk_name.str();

}

/*
 *
 */
void Checkpoint::add(Field<FLOAT> & field, std::string name){
	struct field_data fd = {name, field};
	_data_list.push_back(fd);
	chk_print("Added Field "+name);
}

int Checkpoint::write_ascii(){
	chk_print("Write out checkpoint #"+_chkp_counter);
	//Open file
	std::ofstream output (_chkp_name.c_str(), std::ios::out | std::ios::binary);
	if (!output.is_open()){
		std::cerr<<"Cannot open file "<<_chkp_name.c_str()<<" !"<<std::endl;
		return 1;
	}
	std::cout<<"File "<<_chkp_name.c_str()<<" has been opened"<<std::endl;


	//write header information to file
	output<< "Header"<<std::endl;
	output<< _parameters.parallel.localSize[0]<<" "<<_parameters.parallel.localSize[1]<<" "<<_parameters.parallel.localSize[2]<<std::endl;

	//loop over each element in the list
	std::list<field_data>::const_iterator it;
	for (it = _data_list.begin(); it != _data_list.end(); ++it) {
	    std::cout << (*it).name<<std::endl;
	    output<<(*it).name<<std::endl;

	    //try cast it to scalarfield
	    try{
	    	ScalarField & sf= dynamic_cast<ScalarField &>((*it).field);

	    	if(_parameters.geometry.dim==2){
	    		 for(int i=0;i<sf.getNx();i++){
					for(int j=0;j<sf.getNy();j++){
							output<<sf.getScalar(i,j)<<" ";
					}
				}
	    	}else{
				for(int i=0;i<sf.getNx();i++){
					for(int j=0;j<sf.getNy();j++){
						for(int k=0;k<sf.getNz();k++){
							output<<sf.getScalar(i,j,k)<<" ";
						}
					}
				}
	    	}

	    }catch(const std::bad_cast& e) {
	    	try{
				VectorField & vf= dynamic_cast<VectorField &>((*it).field);

				if(_parameters.geometry.dim==2){
						 for(int i=0;i<vf.getNx();i++){
							for(int j=0;j<vf.getNy();j++){
									output<<vf.getVector(i,j)[0]<<" "<<vf.getVector(i,j)[1]<<" ";
							}
						}
					}else{
						for(int i=0;i<vf.getNx();i++){
							for(int j=0;j<vf.getNy();j++){
								for(int k=0;k<vf.getNz();k++){
									output<<vf.getVector(i,j,k)[0]
								     <<" "<<vf.getVector(i,j,k)[1]
									 <<" "<<vf.getVector(i,j,k)[1]<<" ";
								}
							}
						}
					}

			}catch(const std::bad_cast& e) {
				std::cerr<<"Field "<<(*it).name<<" is not a Scalar or Vector field. Creating checkpoint failed!";
				output.close();
				return 1;
			}
	    }

	    output<<std::endl;
	}

	output.close();

	_chkp_counter++;
	return 0;
}

int Checkpoint::read(){
	chk_print("Read in checkpoint");

	return 0;
}
void Checkpoint::chk_print(std::string msg){
	std::cout<<"Chkp["<<_rank<<"] -> "<<msg<<std::endl;
}

Checkpoint::~Checkpoint() {}
