#ifndef L1Trigger_Phase2L1ParticleFlow_TrackWord_h
#define L1Trigger_Phase2L1ParticleFlow_TrackWord_h

// STL include files
#include <string>

// external package include files
#include <ap_int.h>
#include <ap_fixed.h>

// user include files
#include "DataFormats/Phase2L1ParticleFlow/interface/PFTrack.h"
#include "L1Trigger/Phase2L1ParticleFlow/interface/DiscretePFInputs.h"

namespace l1tpf_impl {

	enum TrackBitWidths {
		kSpareSize          = 5,                                    // Width of bits reserved for future usage
	
		kMVASpareSize       = 6,                                    // Space for two specialized MVA selections
		kTrackQMVASize      = 3,                                    // Width of track quality MVA
		kHitMaskSize        = 7,                                    // Width of the hit mask
		kBendChi2Size       = 3,                                    // Width of the Bend-Chi2
		kChi2Size           = 4,                                    // Width of Chi2
		kTrackQualitySize   = kMVASpareSize + kTrackQMVASize \
		                    + kHitMaskSize + kBendChi2Size \
		                    + kChi2Size,                            // Width of track quality
	
		kD0Size             = 13,                                   // Width of D0
		kD0MagSize          = 5,                                    // Width of D0 magnitude (signed)
		kZ0Size             = 12,                                   // Width of z-position
		kZ0MagSize          = 5,                                    // Width of z-position magnitude (signed)
		kEtaSize            = 16,                                   // Width of eta
		kEtaMagSize         = 3,                                    // Width of eta magnitude (signed)
		kPhiSize            = 12,                                   // Width of phi
		kPhiMagSize         = 0,                                    // Width of phi magnitude (signed)
		kChargeSize         = 1,                                    // Charge of pT
		kChargeMagSize      = 1,                                    // Width of charge magnitude (signed)
		kPtSize             = 14,                                   // Width of pT
		kPtMagSize          = 9,                                    // Width of pT magnitude (unsigned)
		kTrackParamsSize    = kD0Size + kZ0Size + kEtaSize \
		                    + kPhiSize + kChargeSize + kPtSize,     // Width of track parameters
	
		kTrackWordSize      = kSpareSize \
		                    + kTrackQualitySize \
		                    + kTrackParamsSize,                     // Width of the Track Finding Word in bits
	};

	// spare
	typedef ap_uint<kSpareSize>                                      spare_t;        // spare bits

	// track quality types
	typedef ap_uint<kTrackQualitySize>                               trackQuality_t; // All track quality bits
	typedef ap_uint<kMVASpareSize>                                   specMVA_t;      // Specialized MVA selection
	typedef ap_uint<kTrackQMVASize>                                  trackQMVA_t;    // Track quality MVA
	typedef ap_uint<kHitMaskSize>                                    hitMask_t;      // Hit mask bits
	typedef ap_uint<kBendChi2Size>                                   bendChi2_t;     // Bend-Chi2
	typedef ap_uint<kChi2Size>                                       chi2_t;         // Chi2

	class TrackWordInt {

		// track parameters types
		typedef ap_uint<kD0Size>  d0_t;  // D0
		typedef ap_int<kZ0Size>   z0_t;  // Track z
		typedef ap_uint<kEtaSize> eta_t; // Track eta
		typedef ap_int<kPhiSize>  phi_t; // Track phi
		typedef bool              ptq_t; // Charge of track PT
		typedef ap_uint<kPtSize>  pt_t;  // Track PT

		public:
			TrackWordInt() { clear(); }
			TrackWordInt(PropagatedTrack track) { setWord(track); }

			void clear();
			std::string getWord();
			void setWord(PropagatedTrack track);

		protected:
			pt_t         tkPt;
			ptq_t        tkPtCharge;
			phi_t        tkPhi; // relative to the sector center
			eta_t        tkEta;
			z0_t         tkZ0;
			d0_t         tkD0;

			// Track quality
			chi2_t       tkChi2;
			bendChi2_t   tkBendChi2;
			hitMask_t    tkHitMask;
			trackQMVA_t  tkQualityMVA;
			specMVA_t    tkSpecMVASel;

			// Track spare
			spare_t      tkSpare;
	};

	class TrackWordFixed {

		// track parameters types
		typedef ap_fixed<kD0Size, kD0MagSize, AP_RND_CONV, AP_SAT>   d0_t;  // D0
		typedef ap_fixed<kZ0Size, kZ0MagSize, AP_RND_CONV, AP_SAT>   z0_t;  // Track z
		typedef ap_fixed<kEtaSize, kEtaMagSize, AP_RND_CONV, AP_SAT> eta_t; // Track eta
		typedef ap_fixed<kPhiSize, kPhiMagSize, AP_RND_CONV, AP_SAT> phi_t; // Track phi
		typedef bool                                                 ptq_t; // Charge of track PT
		typedef ap_ufixed<kPtSize, kPtMagSize, AP_RND_CONV, AP_SAT>  pt_t;  // Track PT

		public:
			TrackWordFixed() { clear(); }
			TrackWordFixed(PropagatedTrack track) { setWord(track); }

			void clear();
			std::string getWord();
			void setWord(PropagatedTrack track);

		protected:
			pt_t         tkPt;
			ptq_t        tkPtCharge;
			phi_t        tkPhi; // relative to the sector center
			eta_t        tkEta;
			z0_t         tkZ0;
			d0_t         tkD0;

			// Track quality
			chi2_t       tkChi2;
			bendChi2_t   tkBendChi2;
			hitMask_t    tkHitMask;
			trackQMVA_t  tkQualityMVA;
			specMVA_t    tkSpecMVASel;

			// Track spare
			spare_t      tkSpare;
	};
} // namespace

#endif