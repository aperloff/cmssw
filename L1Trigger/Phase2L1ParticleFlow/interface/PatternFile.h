#ifndef L1Trigger_Phase2L1ParticleFlow_PatternFile_h
#define L1Trigger_Phase2L1ParticleFlow_PatternFile_h

// STL include files
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

// external package include files
#include <boost/dynamic_bitset.hpp>
#include <boost/multiprecision/cpp_int.hpp>

// user include files
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/Phase2L1ParticleFlow/interface/PFCandidate.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/DiscretePFInputs.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/Region.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/TrackWord.h"

namespace l1tpf_impl {

	class PatternFile {

		protected:
			// Enums and Typedefs
			enum PatternFileType {None=0,COE,APx,Other};
			enum TrackRepresentationMode { integer, fixedPoint } trackRepresentationMode_;
			typedef boost::dynamic_bitset<> DynamicBitset;
			typedef std::vector<DynamicBitset> BitsetCollection;
			typedef std::vector<BitsetCollection> BitsetTable;
			typedef std::vector<BitsetTable> BitsetTableCollection;

			// Constants
			static constexpr unsigned int tracksize = 96;
			const std::vector<uint32_t> track_word_block_sizes = {14,1,12,16,12,13,4,3,7,14};
			const std::vector<string> typeStrings = {"None", "COE", "APx", "Other"};

		public:
			// Constructor
			PatternFile(const edm::ParameterSet&, const PatternFileType);
			PatternFile(string fileName_, unsigned int nTracksMax_, unsigned int phiSlices,
						unsigned int etaRegions, int nEventsMax, int nEventsPerFile, bool debug = false);

			// Destructor
			virtual ~PatternFile() { delete file; }

			// Methods
			void							assignTrackToBitset(const PropagatedTrack& track);
			void							close() { fclose(file); }
			template<typename T>
			bool							getBit(T value, unsigned bit) { return (value >> bit) & 1; }
			DynamicBitset					getBits(const boost::dynamic_bitset<>& track, unsigned int start_bit = 0, unsigned int end_bit = 63);
			DynamicBitset					getTrackLSB(const boost::dynamic_bitset<>& track) { return getBits(track,0,63); }
			DynamicBitset					getTrackMSB(const boost::dynamic_bitset<>& track) { return getBits(track,32,95); }
			DynamicBitset					getTrackLSBPlusMSB(const boost::dynamic_bitset<>& track1, const boost::dynamic_bitset<>& track2);
			std::string						getFileTypeString(PatternFileType type) { return typeStrings[type]; }
			bool							is_full() { return full; }
			bool							is_open() { return (file != nullptr); }
			void							open() { file = fopen(fileName.c_str(), "w"); }
			void							printDebugInfo(const Region& region, const PropagatedTrack& track);
			std::string						readHeader(unsigned int nlines);
			virtual void					writeHeader() = 0;

			virtual bool					readFile() = 0;
			//virtual std::string				readLine(bool store = true) = 0;
			//virtual int						splitLine() = 0;
			//virtual int						nWords() = 0;
			//virtual DynamicBitset			getBitsetFromWord(int i) = 0;
			//virtual BitsetCollection		getBitsetsFromLine() = 0;
			//virtual bool					getFloatFromBitset() = 0;

		protected:
			// Data Members
			FILE *file;
			std::string fileName, bset_string_, fileNameBase, fileExtension, header;
			unsigned int nTracksMax, phiSlices, etaRegions, nFiles;
			int nEventsMax, nEventsPerFile, nEventsProcessed, debug_;
			DynamicBitset bset_;
			BitsetTable bset_table_;
			BitsetTableCollection bset_table_collection_;
			bool full;
			std::stringstream message;
	};
} // namespace

#endif