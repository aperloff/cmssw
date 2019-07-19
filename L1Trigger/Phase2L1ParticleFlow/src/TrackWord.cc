#include "L1Trigger/Phase2L1ParticleFlow/interface/TrackWord.h"

using namespace l1tpf_impl;

void TrackWordInt::clear() {
	tkPt			= 0;
	tkPtCharge		= 0;
	tkPhi			= 0;
	tkEta			= 0;
	tkZ0			= 0;
	tkD0			= 0;
	tkChi2			= 0;
	tkBendChi2		= 0;
	tkHitMask		= 0;
	tkQualityMVA	= 0;
	tkSpecMVASel	= 0;
	tkSpare			= 0;
}

std::string TrackWordInt::getWord() {
	return "";
}

void TrackWordInt::setWord(PropagatedTrack track) {
	tkPt			= track.hwPt;
	tkPtCharge		= track.hwCharge;
	tkPhi			= track.hwVtxPhi;
	tkEta			= track.hwVtxEta;
	tkZ0			= track.hwZ0;
	tkD0			= 0;
	tkChi2			= track.hwChi2;
	tkBendChi2		= 0;
	tkHitMask		= track.hwStubs;
	tkQualityMVA	= 0;
	tkSpecMVASel	= 0;
	tkSpare			= 0;

}

void TrackWordFixed::clear() {
	tkPt			= 0;
	tkPtCharge		= 0;
	tkPhi			= 0;
	tkEta			= 0;
	tkZ0			= 0;
	tkD0			= 0;
	tkChi2			= 0;
	tkBendChi2		= 0;
	tkHitMask		= 0;
	tkQualityMVA	= 0;
	tkSpecMVASel	= 0;
	tkSpare			= 0;
}

std::string TrackWordFixed::getWord() {
	return "";
}

void TrackWordFixed::setWord(PropagatedTrack track) {
	tkPt			= track.src->pt();
	tkPtCharge		= track.src->charge();
	tkPhi			= track.src->phi();
	tkEta			= track.src->eta();
	tkZ0			= track.src->vertex().Z();
	tkD0			= 0;
	tkChi2			= track.hwChi2;
	tkBendChi2		= 0;
	tkHitMask		= track.hwStubs;
	tkQualityMVA	= 0;
	tkSpecMVASel	= 0;
	tkSpare			= 0;
}