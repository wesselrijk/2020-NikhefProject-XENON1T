# NP-G4
Module containing the Simulation part of the Geant 4 assignment from the Nikhef project. 

Our simulation consists of a cylinder filled with Geant 4 and two PMT arrays with PMT's in the right positions. There are two options to use the program,
one is by running opt_phot.mac, the other is by running neutron.mac. If the main function called ourmain is ran, it will automatically rund opt_phot.mac. This will simulate a 2 eV photon, which is meant to show the detector. The command for the terminal is ./build/ourmain (you will have to make a build folder first and run in this folder cmake .. followed by cmake --build .)

To run neutron.mac, which shoots a 2.2 MeV neutron into the detector we suggest to run the program in batch mode. To do this run the command ./build/ourmain neutron.mac . The program will write a data file as output, called data.dat. The data is stored in the following way: #hit; #pmt; energy; time; position(x, y, z). This might take a few seconds.

