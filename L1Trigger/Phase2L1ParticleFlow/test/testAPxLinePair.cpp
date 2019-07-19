// STL includes
#include <iostream>
#include <string>

// ROOT includes
#include "TROOT.h"
#include "TSystem.h"

// CMSSW includes
#include "FWCore/FWLite/interface/FWLiteEnabler.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSetReader/interface/ParameterSetReader.h"
#include "FWCore/PluginManager/interface/PresenceFactory.h"
#include "FWCore/PluginManager/interface/ProblemTracker.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/Presence.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/APxLinePair.h"

// Constants
static constexpr unsigned int wordsize = 64;
static constexpr unsigned int objectsize = 96;
static const std::vector<std::vector<std::string>> referenceObjects = {
	{
		"000000000000000000110000011000000000000001111111001111011001111011110100011010011100000000010010",
		"000000000000000000110000101000000000000000000000000111110001111010000101000001110100000000010011",
		"000000000000000000101000011100000000000001111101110111100011100010100110000100001000000000001001",
		"000000000000000000101000001000000000000000000100011101100100101100010111001101011000000000001101",
		"000000000000000000110000011000000000000000000000001001101110100011101000001010001000000000010010",
		"000000000000000000110000010000000000000000000000000111111110001001010000101000100100000000011011",
		"000000000000000000110000010100000000000000000010110001100110111111010001101100100000000000001000",
		"000000000000000000110000001100000000000000000001100101111000000000111010011100100100000000001001",
		"000000000000000000100000111000000000000001111110011111110101000110001011001111111100000000001000",
		"000000000000000000110000101000000000000001111111111000010110000100001100001011010000000001000001",
		"000000000000000000110000101100000000000000000000110000000011001100011101011010100000000000010011",
		"000000000000000000101000111100000000000001111101010110001010111010001101111000101000000000001010",
		"000000000000000000101000010100000000000000000001100000001011110111001110110001111100000000001010",
		"000000000000000000100000011100000000000001111110011000100011011011010000000111000100000000001010",
		"000000000000000000101000011100000000000001111111111010000000101110110000011111011100000000001010",
		"000000000000000000100000011100000000000001111111111100011000001100010001011111111000000000001001",
		"000000000000000000110000000100000000000000000000000100001000111000101010100011111100000000011011",
		"000000000000000000110000100000000000000001111011101010101100000010011011010100001000000000001011"
	},
	{
		"000000000000000000101000111000000000000000000000010001100011001010101100100010011000000000001000",
		"000000000000000000100000110000000000000000000011110011110001111100110101101110010000000000001001",
		"000000000000000000100000101000000000000001111100010011111011111010000110011101000100000000001001",
		"000000000000000000101000011000000000000000000011011111100100111000111111000000001100000000001110",
		"000000000000000000101000110000000000000000000010110011110110001111110000000001000000000000001000",
		"000000000000000000110000011100000000000001111110000101010100110101101000111110000100000000001100",
		"000000000000000000101000000100000000000001111011111101110001011000100001100000010100000000001111",
		"000000000000000000101000001100000000000001111111101101011011001111111010100110111100000000001110",
		"000000000000000000110000011000000000000001111110011111011010011100100011001101001100000000001011",
		"000000000000000000100000110100000000000001111111000010000110000100101100011011100000000000001100",
		"000000000000000000100000011000000000000000000011011000000001101001100101101011010100000000001000",
		"000000000000000000110000101100000000000001111110110000100001011111000101111110101100000000001111",
		"000000000000000000100000000000000000000000000000000000000001110100101110110100111100000000001001",
		"000000000000000000110000010000000000000001111111010110011110100010111000011000001000000000001001",
		"000000000000000000101000110100000000000000000110100110010110010100000000100011101000000000001001",
		"000000000000000000101000101000000000000000000001111010101101011110011001111101100100000000001100",
		"000000000000000000111000110100000000000000000011011110011101010111110010010100110100000000001101",
		"000000000000000000110000100100000000000000000000001010100100010101000011100111001100000000010000"
	}
};

int testAPxLinePair(int argc, char *argv[]) {
	int debug = (argc > 1) ? atoi(argv[1]) : 0;
	int mismatch(0);
	l1tpf_impl::APxLinePair<objectsize,wordsize> linePair;
	
	linePair.addLine("0x0000    0x007f3d9ef469c012    0x00001f1e85074013    0x007dde38a6108009    0x0004764b1735800d    "
					 "0x000026e8e8288012    0x00001fe250a2401b    0x0002c66fd1b20008    0x000197803a724009    0x007e7f518b3fc008    "
					 "0x007fe1610c2d0041    0x0000c0331d6a0013    0x007d58ae8de2800a    0x000180bdcec7c00a    0x007e6236d01c400a    "
					 "0x007fe80bb07dc00a    0x007ff183117f8009    0x0000108e2a8fc01b    0x007baac09b50800b\n");
	linePair.addLine("0x0001    0xac89800800003060    0x35b90009000030a0    0x8674400900002870    0x3f00c00e00002820    "
					 "0xf004000800003060    0x68f8400c00003040    0x2181400f00003050    0xfa9bc00e00003030    0x2334c00b000020e0    "
					 "0x2c6e000c000030a0    0x65ad4008000030b0    0xc5fac00f000028f0    0x2ed3c00900002850    0xb860800900002070    "
					 "0x008e800900002870    0x99f6400c00002070    0xf253400d00003010    0x439cc01000003080\n");
	l1tpf_impl::APxLinePair<objectsize,wordsize>::LineState ls = l1tpf_impl::APxLinePair<objectsize,wordsize>::ZeroOne;
	linePair.runAll(ls,debug);
	edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "The headers for the current lines are (" << linePair.headers[0] << "," << linePair.headers[1] << ")\n"
										<< "The DynamicBitsets retrieved are:"; 
	for (unsigned int i = 0; i < linePair.objects.size(); i++) {
		if (linePair.objects[i] != l1tpf_impl::APxLinePair<objectsize,wordsize>::DynamicBitset(referenceObjects[0][i])) mismatch++;
		edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "\t" << linePair.objects[i];
	}
	if (mismatch == 0) {
		edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "All of the objects in the first set match their references.\n";
	}
	else {
		edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "There are " << mismatch << " mismatching objects in the first set.";
		return mismatch;
	}

	mismatch = 0;
	linePair.reset();
	linePair.addLine("0x0001    0xac89800800003060    0x35b90009000030a0    0x8674400900002870    0x3f00c00e00002820    "
					 "0xf004000800003060    0x68f8400c00003040    0x2181400f00003050    0xfa9bc00e00003030    0x2334c00b000020e0    "
					 "0x2c6e000c000030a0    0x65ad4008000030b0    0xc5fac00f000028f0    0x2ed3c00900002850    0xb860800900002070    "
					 "0x008e800900002870    0x99f6400c00002070    0xf253400d00003010    0x439cc01000003080\n");
	linePair.addLine("0x0002    0x000028e000004632    0x000020c00003cf1f    0x000020a0007c4fbe    0x0000286000037e4e    "
					 "0x000028c00002cf63    0x00003070007e154d    0x00002810007bf716    0x00002830007fb5b3    0x00003060007e7da7    "
					 "0x000020d0007f0861    0x000020600003601a    0x000030b0007ec217    0x000020000000001d    0x00003040007f59e8    "
					 "0x000028d000069965    0x000028a00001ead7    0x000038d0000379d5    0x0000309000002a45\n");
	ls = l1tpf_impl::APxLinePair<objectsize,wordsize>::OneTwo;
	linePair.runAll(ls,debug);
	edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "The headers for the current lines are (" << linePair.headers[0] << "," << linePair.headers[1] << ")\n"
										<< "The DynamicBitsets retrieved are:"; 
	for (unsigned int i = 0; i < linePair.objects.size(); i++) {
		if (linePair.objects[i] != l1tpf_impl::APxLinePair<objectsize,wordsize>::DynamicBitset(referenceObjects[1][i])) mismatch++;
		edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "\t" << linePair.objects[i];
	}
	if (mismatch == 0) {
		edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "All of the objects in the second set match their references.";
	}
	else {
		edm::LogVerbatim("testAPxLinePair") << "@SUB=testAPxLinePair::testAPxLinePair()" << "There are " << mismatch << " mismatching objects in the second set.";
		return mismatch;
	}

	return mismatch;
}

int main(int argc, char *argv[]) {
	// store some programatic information
	std::stringstream usage;
	usage << "usage: " << argv[0] << " <debug>";

	// load framework libraries
	gSystem->Load("libFWCoreFWLite");
	FWLiteEnabler::enable();

	// needed to handle the MessageLogger used in the PatternFile classes
	int rc(0);
	std::string const kProgramName = argv[0];
	try {
		// A.  Instantiate a plug-in manager first.
		edm::AssertHandler ah;
	
		// B.  Load the message service plug-in.  Forget this and bad things happen!
		//     In particular, the job hangs as soon as the output buffer fills up.
		//     That's because, without the message service, there is no mechanism for
		//     emptying the buffers.
		std::shared_ptr<edm::Presence> theMessageServicePresence;
		theMessageServicePresence =
			std::shared_ptr<edm::Presence>(edm::PresenceFactory::get()->makePresence("MessageServicePresence").release());
	
		// C.  Manufacture a configuration and establish it.
#include <L1Trigger/Phase2L1ParticleFlow/test/messageLoggerConfiguration.h>
	
		// D.  Create the services.
		std::unique_ptr<edm::ParameterSet> params;
		edm::makeParameterSets(config, params);
		edm::ServiceToken tempToken(edm::ServiceRegistry::createServicesFromConfig(std::move(params)));
	
		// E.  Make the services available.
		edm::ServiceRegistry::Operate operate(tempToken);
	
		//  Generate a bunch of messages.
		rc = testAPxLinePair(argc,argv);
	}
	
	//  Deal with any exceptions that may have been thrown.
	catch (cms::Exception& e) {
		std::cout << "cms::Exception caught in " << kProgramName << "\n" << e.explainSelf();
		rc = 1;
	} catch (std::exception& e) {
		std::cout << "Standard library exception caught in " << kProgramName << "\n" << e.what();
		rc = 1;
	} catch (...) {
		std::cout << "Unknown exception caught in " << kProgramName;
		rc = 2;
	}

	return rc;
}