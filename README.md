Repository for the creation of checkpoints

In order to use this, you have do make some changes:
clone https://github.com/zellnerm/checkpoint.git into your TurbulentFlow folder.

Makefile:
add 
checkpoint/Checkpoint.o
to NSOBJ

main.cpp:
add 
#include "checkpoint/Checkpoint.h"

in main function simply add:

//Setup
std::stringstream chkp_name("");
chkp_name <<"chkp_" << parameters.simulation.scenario
		<< "_" << parameters.geometry.sizeX << "x" << parameters.geometry.sizeY << "x"
		<< parameters.geometry.sizeZ;
std::stringstream folder_name("");
folder_name << "checkpoint/data/";

Checkpoint * c = new Checkpoint(parameters, folder_name.str(), chkp_name.str());

//and in order to register fields for the checkpointing you need to call the add function like this:

c->add(flowField->getPressure(), "Pressure");
c->add(flowField->getVelocity(), "Velocity");
if (parameters.simulation.type == "turbulence") {
	TurbFlowField * turbField = dynamic_cast<TurbFlowField *>(flowField);
	c->add(turbField->getMixingLength(), "Mixing_Length");
	c->add(turbField->getTurbViscosity(), "Viscosity");
}

//Just register all the fields that need to be checkpointed.

//Creating a Checkpoint is quite easy 
std::string name=c->write(time, timeSteps);
if(parameters.parallel.rank==0)
		std::cout<< "Checkpoint filename: " << name << std::endl;


// Reading it in is also quite easy, just pass the name of the file, as it is printed above to the method 
// In order to set the simulation time and timestep correct these need to be passed, too.
c->read(name, time, timeSteps);











