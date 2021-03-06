#ifndef xAODAnaHelpers_VtxHists_H
#define xAODAnaHelpers_VtxHists_H

#include "xAODAnaHelpers/HistogramManager.h"
#include <xAODTracking/TrackParticleContainer.h>
#include <xAODTracking/VertexContainer.h>
#include <xAODTracking/Vertex.h>
#include <xAODTracking/TrackParticle.h>

ANA_MSG_HEADER(msgVtxHists)

class VtxHists : public HistogramManager
{
  public:
    VtxHists(std::string name, std::string detailStr );
    ~VtxHists();

    StatusCode initialize();
    StatusCode execute( const xAOD::VertexContainer* vtxs,  float eventWeight );
    StatusCode execute( const xAOD::Vertex *vtx, float eventWeight );

    // Use tracks passed in to calculate isolated track quantities
    StatusCode execute( const xAOD::VertexContainer* vtxs,  const xAOD::TrackParticleContainer* trks, float eventWeight );
    StatusCode execute( const xAOD::Vertex *vtx,            const xAOD::TrackParticleContainer* trks, float eventWeight );


    using HistogramManager::book; // make other overloaded versions of book() to show up in subclass
    using HistogramManager::execute; // overload

  protected:
    // bools to control which histograms are filled
    bool m_fillTrkDetails;        //!
    bool m_fillIsoTrkDetails;        //!
    bool m_fillDebugging;        //!
    bool m_fillTrkPtDetails;     //!

  private:

    float getIso( const xAOD::TrackParticle *inTrack,            const xAOD::TrackParticleContainer* trks, float z0_cut = 2, float cone_size = 0.2);

    // Histograms
    TH1F* h_type              ; //!
    TH1F* h_nTrks              ; //!
    TH1F* h_nTrks_l              ; //!
    TH1F* h_trk_Pt              ; //!
    TH1F* h_trk_Pt_l              ; //!
    TH1F* h_nTrks1GeV      ; //!
    TH1F* h_nTrks2GeV      ; //!
    TH1F* h_nTrks5GeV      ; //!
    TH1F* h_nTrks10GeV     ; //!
    TH1F* h_nTrks15GeV     ; //!
    TH1F* h_nTrks20GeV     ; //!
    TH1F* h_nTrks25GeV     ; //!
    TH1F* h_nTrks30GeV     ; //!
    TH1F* h_pt_miss_x    ; //!
    TH1F* h_pt_miss_x_l  ; //!
    TH1F* h_pt_miss_y    ; //!
    TH1F* h_pt_miss_y_l  ; //!
    TH1F* h_pt_miss    ; //!
    TH1F* h_pt_miss_l  ; //!

    // Pt of the nth track
    unsigned int m_nLeadTrackPts ; //!
    std::vector<TH1F*> h_trk_max_Pt              ; //!
    std::vector<TH1F*> h_trk_max_Pt_l              ; //!


    TH1F* h_trkIsoAll              ; //!
    TH1F* h_trkIso             ; //!
    TH1F* h_nIsoTrks              ; //!
    TH1F* h_nIsoTrks_l              ; //!
    TH1F* h_IsoTrk_Pt              ; //!
    TH1F* h_IsoTrk_Pt_l              ; //!
    TH1F* h_nIsoTrks1GeV      ; //!
    TH1F* h_nIsoTrks2GeV      ; //!
    TH1F* h_nIsoTrks5GeV      ; //!
    TH1F* h_nIsoTrks10GeV     ; //!
    TH1F* h_nIsoTrks15GeV     ; //!
    TH1F* h_nIsoTrks20GeV     ; //!
    TH1F* h_nIsoTrks25GeV     ; //!
    TH1F* h_nIsoTrks30GeV     ; //!
    TH1F* h_dZ0Before         ; //!
    TH1F* h_pt_miss_iso_x    ; //!
    TH1F* h_pt_miss_iso_x_l  ; //!
    TH1F* h_pt_miss_iso_y    ; //!
    TH1F* h_pt_miss_iso_y_l  ; //!
    TH1F* h_pt_miss_iso    ; //!
    TH1F* h_pt_miss_iso_l  ; //!

    // Pt of the nth track
    unsigned int m_nLeadIsoTrackPts ; //!
    std::vector<TH1F*> h_IsoTrk_max_Pt              ; //!
    std::vector<TH1F*> h_IsoTrk_max_Pt_l              ; //!


};


#endif
