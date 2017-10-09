#include <vector>

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"

#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"

#include "TRandom3.h"

#include "xAODAnaHelpers/CE.h"



using namespace std;


ClusterEfficiency::ClusterEfficiency(const std::string& name) : JetConstituentModifierBase(name)
                                                                      
{

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif

  myrand_global = new TRandom3(0); //for uncerts.
}


StatusCode ClusterEfficiency::process(xAOD::CaloClusterContainer* cont) const {
  for(xAOD::CaloCluster* cl : *cont) {
    double phi_smear = myrand_global->Gaus(cl->phi(),0.005);
    //double eta_smear = myrand_global->Gaus(cl->eta(),0.005);
		double E = cl->e();
		double pt = cl->pt();
		double eta = cl->eta();
    //double E_smear = E*cosh(eta)/cosh(eta_smear);



      //double r = 0.25*exp(-2.*E); //8 TeV.
      //double flip = myrand_global->Uniform(0.,1.);

		  vector<double> etabins = { 0.00,  0.60,  1.10,  1.40,  1.50,   1.80,  1.90,  5.00};
		  vector<double> alpha1 =  { 0.25,  0.12,  0.17,  0.17,  0.15,   0.16,  0.16,  0.16};
  		vector<double> beta1 =   {-2.00, -0.51, -1.31, -0.95, -1.14,  -2.77, -1.47, -1.61};
		  vector<double> alpha2 =  { 0.00,  4.76,  4.33,  1.14, 2768.98, 0.67,  0.86,  4.99};
  		vector<double> beta2 =   { 0.00, -0.29, -0.23, -0.04, -4.20,  -0.11, -0.12, -0.52};

    int etabin = -1;
    for(int i=0; i<etabins.size(); i++){
      if(fabs(eta) < etabins[i]){
        etabin = i;
      }
    }


    double p = pt*cosh(eta)/1000.; //GeV
    double r = (alpha1[etabin]*exp(beta1[etabin]*p) + alpha2[etabin]*exp(beta2[etabin]*p*p)) / 100.;
    double flip = myrand_global->Uniform(0.,1.);
    if (((flip < r) && (E/1000. < 2.5))) {
			cl->setE(0.0);
			//std::cout << E << "\t" << "cluster eff" << std::endl;
		}
  }

  return StatusCode::SUCCESS;
}

StatusCode ClusterEfficiency::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);
  return StatusCode::FAILURE;
}






