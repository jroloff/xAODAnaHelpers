// this file is -*- C++ -*- 
// SoftKillerWeightTool.h

#ifndef xAODAnaHelpers_CES_H
#define xAODAnaHelpers_CES_H



// \class SoftKillerWeightTool
// \author Jennifer Roloff
// \date November 2015
//
// This is the tool to reweight clusters according to the SoftKiller
// algorithm
// It creates a new cluster collection based on the original clusters
// and the grid spacing for SK
//
//
// Properties are as follows:
//   OutputContainer: Name of the output cluster collection
//   InputCollection: Name of the input cluster collection. 
//   GridSize: The grid size that should be applied for the SK 
//   	 algorithm. Suggested values between 0.3 and 0.6
//   SKRapMin: The minimum (absolute) rapidity over which to calculate SK
//   SKRapMax: The maximum (absolute) rapidity over which to calculate SK
//   SKRapMinApplied: The minimum (absolute) rapidity over which to apply SK
//   SKRapMaxApplied: The maximum (absolute) rapidity over which to apply SK
//   isCaloSplit: If false, SK is run the same on all clusters. If
//   	 true, SK is run separately for clusters in the ECal and the
//   	 HCal.
//   ECalGridSize: Only necessary if isCaloSplit == true. The SK grid spacing
//   	 used for the ECal.
//   HCalGridSize: Only necessary if isCaloSplit == true. The SK grid spacing
//     used for the HCal.
//
//
// This tool may be used on its own or in sequence with other pileup
// suppression methods. For central jets, it is suggested that the
// input collection be clusters with area subtraction (such as Voronoi)
// applied.
//




#include <string>
#include "JetRecTools/JetConstituentModifierBase.h"
#include "xAODBase/IParticleContainer.h"

#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"

#include "TRandom3.h"



class ClusterEnergyScale : public JetConstituentModifierBase{
  ASG_TOOL_CLASS(ClusterEnergyScale, IJetConstituentModifier)

  public:
  
  ClusterEnergyScale(const std::string& name);
  StatusCode process(xAOD::IParticleContainer* cont) const; 
  StatusCode process(xAOD::CaloClusterContainer* cont) const; // MEN: Might need to rename this process

	std::string m_method;

private:

 TRandom3* myrand_global; //!

		
};


#endif
