#ifndef L1Trigger_Phase2L1ParticleFlow_PatternFile_h
#define L1Trigger_Phase2L1ParticleFlow_PatternFile_h

// STL include files
#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

// ROOT include files
#include "TROOT.h"
#include "TSystem.h"
#include "TMath.h"
#include "Math/GenVector/LorentzVector.h"
#include "Math/Vector4D.h"

// external package include files
#include <boost/dynamic_bitset.hpp>
#include <boost/multiprecision/cpp_int.hpp>

// user include files
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "DataFormats/L1TParticleFlow/interface/PFCandidate.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/DiscretePFInputs.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/Region.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/TrackWord.h"

namespace l1tpf_impl {

	class PatternFile {

		protected:
			// Enums and Typedefs
			enum PatternFileType {None=0,COE,APx,Other};
			enum ObjectRepresentationMode { integer, fixedPoint } objectRepresentationMode_;
			typedef boost::dynamic_bitset<> DynamicBitset;
			typedef std::vector<DynamicBitset> BitsetCollection;
			typedef std::vector<BitsetCollection> BitsetTable;
			typedef std::vector<BitsetTable> BitsetTableCollection;
			typedef std::pair<int,int> LinkObjectIndex;
			typedef std::vector<LinkObjectIndex> LinkObjectIndexCollection;
			typedef std::map<LinkObjectIndex,ROOT::Math::PtEtaPhiEVector> CommonFormat;
			typedef std::vector<std::string> StringCollection;

			// Constants
			static constexpr unsigned int tracksize = 96;
			const std::vector<uint32_t> track_word_block_sizes = {14,1,12,16,12,13,4,3,7,14};
			const std::vector<string> typeStrings = {"None", "COE", "APx", "Other"};

		public:
			// Constructor
			PatternFile(const edm::ParameterSet&, std::ios_base::openmode fileOpenMode, const PatternFileType);

			// Destructor
			virtual ~PatternFile() {}

			// Methods
			void							assignTrackToBitset(const PropagatedTrack& track);
			template <size_t N, class = std::enable_if_t<(N > 0 && N < 64)>>
			int64_t							bitsetToInt64(const DynamicBitset& b) { int const shift = 64 - N; return (((int64_t)b.to_ulong() << shift) >> shift); }
			ROOT::Math::PtEtaPhiEVector		bitsetToLorentzVector(const DynamicBitset& b);
			void							close() { file.close(); file.clear(); }
			CommonFormat					convertBitsetTableToCommonFormat();
			bool							eof();
			bool							emptyTable();
			void							filterEmptyObjects();
			template<typename K, typename V, typename T>
			bool							findAllFromLink(std::vector<K> & vec, std::map<K, V> mapOfElemen, T value);
			template<typename T>
			bool							getBit(T value, unsigned bit) { return (value >> bit) & 1; }
			DynamicBitset					getBits(const boost::dynamic_bitset<>& bset, unsigned int start_bit = 0, unsigned int end_bit = 63);
			virtual PatternFileType			getClassType() = 0;
			virtual unsigned int			getHeaderLines() = 0;
			unsigned int					getNFilesLoaded() { return bset_table_collection_.size(); }
			unsigned int					getNEventsInFile(unsigned int ifile_ = 0);
			DynamicBitset					getTrackLSB(const boost::dynamic_bitset<>& bset) { return getBits(bset,0,63); }
			DynamicBitset					getTrackMSB(const boost::dynamic_bitset<>& bset) { return getBits(bset,32,95); }
			DynamicBitset					getTrackLSBPlusMSB(const boost::dynamic_bitset<>& bset1, const boost::dynamic_bitset<>& bset2);
			std::string						getFileTypeString(PatternFileType type) { return typeStrings[type]; }
			bool							is_full() { return full; }
			bool							is_open() { return file.is_open(); }
			bool							loadEvent(unsigned int ifile_ = 0, unsigned int ievent_ = 0);
			bool							loadNextEvent();
			ROOT::Math::PtEtaPhiEVector		makeLorentzVector(float pt, float eta, float phi, float e) { return ROOT::Math::PtEtaPhiEVector(pt,eta,phi,e); }
			bool							nextFile();
			bool							open() { file.open(fileName, openMode); return file.good(); }
			void							printCommonFormat(CommonFormat objects, const std::vector<Region>& regions = {});
			void							printEventDebugInfo(unsigned int row = 0, bool verbatim = false);
			void							printEventDebugInfo(std::vector<unsigned int> rows, bool verbatim = false) { for (auto row : rows) printEventDebugInfo(row,verbatim); }
			void							printTableSizes(std::string tableHeader = "");
			void							printTrackDebugInfo(const Region& region, const PropagatedTrack& track);
			virtual void					readFile() = 0; // Reads an entire files worth of events
			std::string						readHeader();
			void							resetBitsetTable() { bset_table_.clear(); bset_table_.resize(nLinks); }
			void							storeTracks(const std::vector<Region>& regions);
			virtual void					writeHeader() = 0;
			virtual void					writeObjectsToFile() = 0;

		protected:
			// Data Members
			std::fstream file;
			std::ios_base::openmode openMode;
			std::stringstream message;
			std::string fileName, bset_string_, fileNameBase, fileExtension, header;
			bool full;
			unsigned int nTracksMax, phiSlices, etaRegions, nLinks, nFiles, ifile, ievent;
			int nEventsMax, nEventsPerFile, nEventsProcessed, debug_;
			DynamicBitset bset_;
			BitsetTable bset_table_;
			BitsetTableCollection bset_table_collection_;
			std::vector<unsigned int> events_per_file_;
	};
} // namespace

#endif
