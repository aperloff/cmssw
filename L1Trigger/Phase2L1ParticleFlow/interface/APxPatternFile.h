#ifndef L1Trigger_Phase2L1ParticleFlow_APxPatternFile_h
#define L1Trigger_Phase2L1ParticleFlow_APxPatternFile_h

// user include files
#include "L1Trigger/Phase2L1ParticleFlow/interface/PatternFile.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/APxLinePair.h"

namespace l1tpf_impl {

	class APxPatternFile: public PatternFile {

		protected:
			// Constants
			static constexpr l1tpf_impl::PatternFile::PatternFileType classType = l1tpf_impl::PatternFile::PatternFileType::APx;
			static constexpr unsigned int wordsize = 64;
			static constexpr unsigned int bitsToNibbles = 4;
			static constexpr unsigned int wordchars = wordsize/bitsToNibbles;
			static constexpr unsigned int nState = 3;
			static constexpr unsigned int tracksPerState = 2;
			static constexpr unsigned int headerSize = 3;

		public:
			APxPatternFile(const edm::ParameterSet& iConfig, std::ios_base::openmode openMode) :
				PatternFile(iConfig,openMode,classType), state(0) {}
			~APxPatternFile() {}

			PatternFileType getClassType() { return classType; }
			unsigned int	getHeaderLines() { return headerSize; }
			void			readFile();
			void			writeHeader();
			void			writeObjectsToFile();
			void			writeObjectsToFiles();

		private:
			unsigned int state;
	};
} // namespace

#endif