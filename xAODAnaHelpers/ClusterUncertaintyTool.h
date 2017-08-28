/********************************************************
 * ClusterUncertaintyTool:
 *
 * This class handles jet calibration and systematics
 *
 * Jeff Dandoy (jeff.dandoy@cern.ch)
 *
 ********************************************************/

#ifndef xAODAnaHelpers_ClusterUncertaintyTool_H
#define xAODAnaHelpers_ClusterUncertaintyTool_H

// external tools include(s):
#include "AsgTools/AnaToolHandle.h"

#include "JetRecTools/JetConstituentModSequence.h"

// algorithm wrapper
#include "xAODAnaHelpers/Algorithm.h"

/** @rst
  A wrapper to a few JetETMiss packages. By setting the configuration parameters detailed in the header documentation, one can:

    - calibrate a given jet collection
    - apply systematic variations for JES
    - apply systematic variations for JER
    - decorate the jet with the decision of the Jet Cleaning tool

  When considering systematics, a new ``xAOD::JetCollection`` is created for each systematic variation. The names are then saved in a vector for downstream algorithms to use.

@endrst */
class ClusterUncertaintyTool : public xAH::Algorithm
{
public:
  /// @brief The name of the input container for this algorithm to read from ``TEvent`` or ``TStore``
  std::string m_inContainerName = "";
  std::string m_outContainerName = "";

  /// @brief set to ``AntiKt4EMTopo`` for ``AntiKt4EMTopoJets``
  std::string m_jetAlgo = "";
  /// @brief name of vector holding names of jet systematics given by the JetEtmiss Tools
  std::string m_outputAlgo = "";

private:

  int m_numEvent;         //!
  int m_numObject;        //!

  bool m_isMC;            //!
  bool m_isFullSim;       //!


  ToolHandleArray<IJetConstituentModifier> hclusts_ces; //!
  ToolHandleArray<IJetConstituentModifier> hclusts_cesUp; //!
  ToolHandleArray<IJetConstituentModifier> hclusts_cesDown; //!
  ToolHandleArray<IJetConstituentModifier> hclusts_car; //!
  ToolHandleArray<IJetConstituentModifier> hclusts_ce; //!

  JetConstituentModSequence *cesModSeq; //!
  JetConstituentModSequence *cesUpModSeq; //!
  JetConstituentModSequence *cesDownModSeq; //!
  JetConstituentModSequence *carModSeq; //!
  JetConstituentModSequence *ceModSeq; //!


public:

  // this is a standard constructor
  ClusterUncertaintyTool ();

  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();

  /// @cond
  // this is needed to distribute the algorithm to the workers
  ClassDef(ClusterUncertaintyTool, 1);
  /// @endcond

};

#endif
