#ifndef L1Trigger_Phase2L1ParticleFlow_CoeFile_h
#define L1Trigger_Phase2L1ParticleFlow_CoeFile_h

// user include files
#include "L1Trigger/Phase2L1ParticleFlow/interface/PatternFile.h"

namespace l1tpf_impl {

	class COEFile: public PatternFile {

		protected:
			// Constants
			static constexpr unsigned int header_size = 10;

		public:
			COEFile(const edm::ParameterSet& iConfig) : PatternFile(iConfig,l1tpf_impl::PatternFile::PatternFileType::COE) {}
			~COEFile() {}

			unsigned int	getHeaderSize() { return header_size; }
			bool			readFile();
			std::string		readHeader() { return l1tpf_impl::PatternFile::readHeader(header_size); }
			void			writeHeader();
			void			writeTracksToFile(const std::vector<Region>& regions, bool print = false);
	};
} // namespace

#endif