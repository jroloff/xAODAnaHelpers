// c++ include(s):
#include <iostream>
#include <typeinfo>
#include <map>

// EL include(s):
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

// EDM include(s):
//#include "xAODCore/ShallowCopy.h"
//#include "AthContainers/ConstDataVector.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODJet/JetContainer.h"

// package include(s):
#include "xAODAnaHelpers/TauJetMatching.h"
#include "xAODAnaHelpers/HelperClasses.h"
#include "xAODAnaHelpers/HelperFunctions.h"

// ROOT include(s):
#include "TLorentzVector.h"

// this is needed to distribute the algorithm to the workers
ClassImp(TauJetMatching)


TauJetMatching :: TauJetMatching () :
    Algorithm("TauJetMatching")
{
}

TauJetMatching::~TauJetMatching() {}

EL::StatusCode TauJetMatching :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

  ANA_MSG_INFO( "Calling setupJob");

  job.useXAOD ();
  xAOD::Init( "TauJetMatching" ).ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TauJetMatching :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  ANA_MSG_INFO( "Calling histInitialize");
  ANA_CHECK( xAH::Algorithm::algInitialize());

  //if ( this->numInstances() > 1 ) {
  //  m_isUsedBefore = true;
  //  ANA_MSG_INFO( "\t An algorithm of the same type has been already used " << numInstances() << " times" );
  //}

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TauJetMatching :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed

  ANA_MSG_INFO( "Calling fileExecute");

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TauJetMatching :: changeInput (bool /*firstFile*/)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.

  ANA_MSG_INFO( "Calling changeInput");

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TauJetMatching :: initialize ()
{
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  ANA_MSG_INFO( "Initializing TauJetMatching Interface... ");

  // Let's see if the algorithm has been already used before:
  // if yes, will write object cutflow in a different histogram!
  //
  // This is the case when the selector algorithm is used for
  // preselecting objects, and then again for the final selection
  //
  ANA_MSG_INFO( "Algorithm name: " << m_name << " - of type " << m_className );

  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();

  ANA_MSG_INFO( "Number of events in file: " << m_event->getEntries() );

  if ( m_inContainerName.empty() ){
    ANA_MSG_ERROR( "InputContainer is empty!");
    return EL::StatusCode::FAILURE;
  }

  if ( m_inJetContainerName.empty() ){
    ANA_MSG_ERROR( "InputJetContainer is empty!");
    return EL::StatusCode::FAILURE;
  }

  // ********************************
  //
  // Initialise TauJetMatchingTool
  //
  // ********************************

  // IMPORTANT: if no working point is specified the one in this configuration will be used

  ANA_MSG_INFO( "TauJetMatching Interface succesfully initialized!" );

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TauJetMatching :: execute ()
{
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.

  ANA_MSG_DEBUG( "Applying Tau Selection..." );

  const xAOD::EventInfo* eventInfo(nullptr);
  ANA_CHECK( HelperFunctions::retrieve(eventInfo, m_eventInfoContainerName, m_event, m_store, msg()) );

  const xAOD::TauJetContainer* inTaus(nullptr);

  const xAOD::JetContainer* inJets(nullptr);
  ANA_CHECK( HelperFunctions::retrieve(inJets, m_inJetContainerName, m_event, m_store, msg()) );


  // if input comes from xAOD, or just running one collection,
  // then get the one collection and be done with it
  //
  if ( m_inputAlgoSystNames.empty() ) {

    // this will be the collection processed - no matter what!!
    //
    ANA_CHECK( HelperFunctions::retrieve(inTaus, m_inContainerName, m_event, m_store, msg()) );

    // fill truth-matching map
    //
    std::unordered_map<int, std::pair<const xAOD::TauJet*, const xAOD::Jet* > > match_map;
    
    match_map = findBestMatchDR( inJets, inTaus, m_DeltaR );
    
    executeDecoration(match_map, inTaus);

  } else { // get the list of systematics to run over

    // get vector of string giving the syst names of the upstream algo from TStore (rememeber: 1st element is a blank string: nominal case!)
    //
    std::vector< std::string >* systNames(nullptr);
    ANA_CHECK( HelperFunctions::retrieve(systNames, m_inputAlgoSystNames, 0, m_store, msg()) );

    ANA_MSG_DEBUG( " input list of syst size: " << static_cast<int>(systNames->size()) );

    // loop over systematic sets
    //
    for ( auto systName : *systNames ) {

      ANA_MSG_DEBUG( " syst name: " << systName << "  input container name: " << m_inContainerName+systName );

      ANA_CHECK( HelperFunctions::retrieve(inTaus, m_inContainerName + systName, m_event, m_store, msg()) );
      ANA_CHECK( HelperFunctions::retrieve(inJets, m_inJetContainerName, m_event, m_store, msg()) );

      std::unordered_map<int, std::pair<const xAOD::TauJet*, const xAOD::Jet* > > match_map_sys;
      
      match_map_sys = findBestMatchDR( inJets, inTaus, m_DeltaR );
      
      executeDecoration(match_map_sys, inTaus);
    }
  }

  // look what we have in TStore
  //
  if(msgLvl(MSG::VERBOSE)) m_store->print();

  return EL::StatusCode::SUCCESS;

}

bool TauJetMatching :: executeDecoration ( std::unordered_map<int, std::pair<const xAOD::TauJet*, const xAOD::Jet* > > match_map, const xAOD::TauJetContainer* inTaus)
{
  static SG::AuxElement::Decorator< float > JetWidthDecor("JetWidth");
  static SG::AuxElement::ConstAccessor<float> jetWidthAcc("Width");

  static SG::AuxElement::Decorator< float > JetJvtDecor("JetJvt");
  static SG::AuxElement::ConstAccessor<float> jetJvtAcc("Jvt");

  ANA_MSG_DEBUG( "Initial Taus: " << static_cast<uint32_t>(inTaus->size()) );
  
  int iTau = -1;

  for ( auto tau_itr : *inTaus ) { // duplicated of basic loop
    iTau++;

    std::unordered_map<int, std::pair<const xAOD::TauJet*, const xAOD::Jet* > >::const_iterator it_map = match_map.find (iTau);

    if (it_map != match_map.end()) {
      
      // jet width      
      if (jetWidthAcc.isAvailable(*match_map[iTau].second)) {
        JetWidthDecor(*tau_itr) = static_cast<float>( jetWidthAcc(*match_map[iTau].second) );
      } else { 
        JetWidthDecor(*tau_itr) = -1.;
      } 
   
      // jet jvt    
      if (jetJvtAcc.isAvailable(*match_map[iTau].second)) {
        JetJvtDecor(*tau_itr) = static_cast<float>( jetJvtAcc(*match_map[iTau].second) );
      } else { 
        JetJvtDecor(*tau_itr) = -1.;
      } 


    } else {
      JetWidthDecor(*tau_itr) = -1.;
      JetJvtDecor(*tau_itr) = -1.;
    }
  }

  ANA_MSG_DEBUG( "Left  executeDecoration..." );

  return true;
}

EL::StatusCode TauJetMatching :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.

  ANA_MSG_DEBUG( "Calling postExecute");

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TauJetMatching :: finalize ()
{
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode TauJetMatching :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  ANA_MSG_INFO( "Calling histFinalize");
  ANA_CHECK( xAH::Algorithm::algFinalize());
  return EL::StatusCode::SUCCESS;
}

float TauJetMatching::getDR(float eta1, float eta2, float phi1, float phi2)
{
 float deta = std::abs(eta1-eta2);
 float dphi = std::abs(phi1-phi2);
 dphi = ( dphi <= TMath::Pi() ) ? dphi : ( 2 * TMath::Pi() - dphi );
 return sqrt(deta*deta + dphi*dphi);
}

std::unordered_map<int, std::pair<const xAOD::TauJet*, const xAOD::Jet* > > TauJetMatching::findBestMatchDR(const xAOD::JetContainer* jetCont,
                                                                                                            const xAOD::TauJetContainer* tauCont,
                                                                                                            float best_DR=1.0)
{ 
  // Find tau that best matches a jet using DR.
  // If matching is successful, returns a map where the key 
  // is the  container index of the matched tau and the value
  // is the pair of the matched tau and the corresponding jet
  
  float default_best_DR = best_DR;
  int ijet = -1;
  int best_ijet = -1;
  const xAOD::Jet* best_jet = nullptr;

  std::unordered_map<int, std::pair<const xAOD::TauJet*, const xAOD::Jet*>> match_map;
  
  for (const auto jet : *jetCont) {
      ++ijet;
      
      int itau = -1;
      int best_itau = -1;
      const xAOD::TauJet* best_tau = nullptr;
      float DR = 0;
      
      for (const auto tau : *tauCont) {
        ++itau;
        
        DR = this->getDR(tau->eta(),jet->eta(),tau->phi(),jet->phi());

        if (DR < best_DR) {
          best_DR = DR;
          best_tau = tau;
          best_jet = jet;
          best_itau = itau;
          best_ijet = ijet;
        }
      }
      
      std::unordered_map<int, std::pair<const xAOD::TauJet*, const xAOD::Jet* > >::const_iterator got = match_map.find (best_itau);
      
      // if a new match is found for a previous 
      // tau keep the new match if it is better 
      
      if (got == match_map.end() and best_itau != -1 and best_ijet != -1) {
        match_map[best_itau] = std::pair<const xAOD::TauJet*, const xAOD::Jet*>(best_tau, best_jet);
      }
      else if (got != match_map.end() and best_itau != -1 and best_ijet != -1) {
        float old_DR = this->getDR(match_map[best_itau].first->eta(), match_map[best_itau].second->eta(),match_map[best_itau].first->phi(), match_map[best_itau].second->phi());
        if (old_DR > best_DR) {
          match_map[best_itau] = std::pair<const xAOD::TauJet*, const xAOD::Jet*>(best_tau, best_jet);
        }
      }
      
      // reset the best_DR to the default value
      best_DR = default_best_DR;
  }

  return match_map;

}
