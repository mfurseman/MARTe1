/*
 * Copyright 2011 EFDA | European Fusion Development Agreement
 *
 * Licensed under the EUPL, Version 1.1 or - as soon they 
   will be approved by the European Commission - subsequent  
   versions of the EUPL (the "Licence"); 
 * You may not use this work except in compliance with the 
   Licence. 
 * You may obtain a copy of the Licence at: 
 *  
 * http://ec.europa.eu/idabc/eupl
 *
 * Unless required by applicable law or agreed to in 
   writing, software distributed under the Licence is 
   distributed on an "AS IS" basis, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
   express or implied. 
 * See the Licence for the specific language governing 
   permissions and limitations under the Licence. 
 *
 * $Id$
 *
**/

#include "MathGAM.h"
#include "CDBExtended.h"
#include "DDBInputInterface.h"
#include "DDBOutputInterface.h"

/// This method runs at GAM initialisation
bool MathGAM::Initialise(ConfigurationDataBase &cdbData) {

    CDBExtended cdb(cdbData);
    ////////////////////////////////////////////////////
    //                Add interfaces to DDB           //
    ////////////////////////////////////////////////////
    if(!AddInputInterface(input, "InputInterface")) {
        AssertErrorCondition(
            InitialisationError, 
            "Math::Initialise: %s failed to add input interface",
            Name());
        return False;
    }
 
    if(!AddOutputInterface(output, "OutputInterface")) {
        AssertErrorCondition(
            InitialisationError,
            "Math::Initialise: %s failed to add output interface",
            Name());
        return False;
    }   

    ////////////////////////////////////////////////////
    //                Add input signals               //
    ////////////////////////////////////////////////////
    if(!cdb->Move("InputSignals")) {
        AssertErrorCondition(
            InitialisationError, 
            "Math::Initialise: %s did not specify InputSignals entry",
            Name());
        return False;
    }

    int32 numberOfSignals = cdb->NumberOfChildren();
    if(numberOfSignals != 1) {
        AssertErrorCondition(
            InitialisationError,
            "Math::Initialise: %s ObjectLoadSetup. Math expects a single signal input, not %ld",
            Name(),
            numberOfSignals);
        return False;
    }

    if(!input->ObjectLoadSetup(cdb, NULL)) {
        AssertErrorCondition(
            InitialisationError,
            "Math::Initialise: %s ObjectLoadSetup Failed DDBInterface %s ",
            Name(),
            input->InterfaceName());
        return False;
    }
    cdb->MoveToFather();

    ////////////////////////////////////////////////////
    //                Add output signals              //
    ////////////////////////////////////////////////////
    if(!cdb->Move("OutputSignals")) {
        AssertErrorCondition(
            InitialisationError,
            "Math::Initialise: %s did not specify OutputSignals entry", 
            Name());
        return False;
    }

    if(!output->ObjectLoadSetup(cdb, NULL)) {
        AssertErrorCondition(
            InitialisationError,
            "Math::Initialise: %s ObjectLoadSetup Failed DDBInterface %s ",
            Name(),
            output->InterfaceName());
        return False;
    }

    numberOfSignals = cdb->NumberOfChildren();
    if(numberOfSignals != 1) {
        AssertErrorCondition(
            Warning,
            "Math::Initialise: %s ObjectLoadSetup. Only the adjusted signal will be output from this GAM",
            Name());
    }
    cdb->MoveToFather();
 
    ////////////////////////////////////////////////////
    //            Configuration parameters            //
    ////////////////////////////////////////////////////
	if(!cdb.ReadFloat(scaler, "scaler", 0)) {
	    AssertErrorCondition(Information, "Math %s::Initialise: Scaler not specified. Using default %f", Name(), scaler);
	}
 
	if(!cdb.ReadFloat(offset, "offset", 0)) {
	    AssertErrorCondition(Information, "Math %s::Initialise: Offset not specified. Using default %f", Name(), offset);
	}

    return True;
}

/// Called in every control loop
bool MathGAM::Execute(GAM_FunctionNumbers execFlag) {

    // Get input and output data pointers
    input->Read();
    float* inputBuffer = (float*)input->Buffer();
    float* outputBuffer = (float*)output->Buffer();

    switch(execFlag) {
        case GAMOnline: {
            *outputBuffer = *inputBuffer * scaler + offset;
            output->Write();  // Update output to match buffer
        }
        break;
    }

    return True;
}
OBJECTLOADREGISTER(MathGAM, "$Id$")
