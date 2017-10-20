#include <vector>

#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/PseudoJet.hh"
#include "fastjet/Selector.hh"

#include "xAODCore/ShallowCopy.h"
#include "xAODCaloEvent/CaloCluster.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODBase/IParticleHelpers.h"
#include "xAODCore/ShallowAuxContainer.h"
#include "TH2.h"
#include "TFile.h"

#include "TRandom3.h"

#include "xAODAnaHelpers/CES.h"



using namespace std;


ClusterEnergyScale::ClusterEnergyScale(const std::string& name) : JetConstituentModifierBase(name)  {

#ifdef ASG_TOOL_ATHENA
  declareInterface<IJetConstituentModifier>(this);
#endif
  declareProperty("Method", m_method = "");
  myrand_global = new TRandom3(0); //for uncerts.
}


StatusCode ClusterEnergyScale::process(xAOD::CaloClusterContainer* cont) const {
	// Need to double check if this is done per-jet, per-event, etc.
  double CES_option2_factors[7]={myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1),myrand_global->Gaus(0,1)};

	vector<double> etabins =  {0,    1.1,  1.4,  1.5,  1.8,  1.9,  5.0 };
	vector<double> alpha =    {0.05, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
	vector<double> beta =     {500,  500,  500,  0,    500,  0,    500 };

  for(xAOD::CaloCluster* cl : *cont) {
    double m_alpha = 0.05;
    double m_beta = 500;
		double eta = cl->eta();
		double E = cl->e();
		double p = cl->pt();
		double abs_eta = fabs(eta);

		int etabin = -1;
		for(int i=0; i<etabins.size(); i++){
			if(fabs(eta) < etabins[i]){
				etabin = i;
			}
		}

    double CESfactor = alpha[etabin]*(1.+beta[etabin]/E);

    double e_ces =  E*(1.+CES_option2_factors[etabin]*CESfactor);
    double e_cesu = E*(1.+CESfactor);
    double e_cesd = E*(1.-CESfactor);

	  double center_lambda;
	 	double m_lambdaCalDivide = 317;
    bool ishad = true;
    (cl)->retrieveMoment(xAOD::CaloCluster::CENTER_LAMBDA,center_lambda);
    if( center_lambda > m_lambdaCalDivide) ishad = false;

    std::string filename = "cluster_uncert_map.root";
		TFile *file = new TFile(filename.c_str());
	  TH2D *cluster_means = (TH2D*) file->Get("Mean");
	  TH2D *cluster_rmss = (TH2D*) file->Get("RMS");




    int pbin = cluster_means->GetXaxis()->FindBin(p);
    int ebin = cluster_means->GetYaxis()->FindBin(abs_eta);
    if (pbin > cluster_means->GetNbinsX()) pbin = cluster_means->GetNbinsX();
    if (pbin < 1) pbin = 1;
    if (ebin > cluster_means->GetNbinsX()) ebin  = cluster_means->GetNbinsX();
    if (ebin < 1) ebin  = 1;

    double myCES = fabs(cluster_means->GetBinContent(pbin,ebin)-1.);
    double myCER = fabs(cluster_rmss->GetBinContent(pbin,ebin));

    if (p > 350) myCES = 0.1;
    if (p > 350) myCER = 0.1;

  
    if (!ishad){
        myCER = 0.004;
        if (p < 10) myCES = 0.01;
        else myCES = 0.005; //it is much better than this, just being conservative.                                                         
    }
    
    myCES =  E*(1+myCES);
    myCER =  E*(1+myCER);
		std::cout << E << "\t" << e_cesu << "\t" << myCES << std::endl;
    e_cesu = myCES;
    e_ces = myCER;

		file->Close();



		//std::cout << "Method: " << m_method << "\t" << CESfactor << "\t" << std::endl;
	  //if(strcmp("Up", m_method.c_str()) == 1) cl->setE( E * (1. + CESfactor) );
	  if(strcmp("Up", m_method.c_str()) == 0) {
	
			cl->setE( e_cesu );
		//	std::cout << m_method << "\t" << E << "\t" << cl->e() << "\t" << CESfactor << "\t" << E*(1+CESfactor) << std::endl;
		}
		//else if(strcmp("Down", m_method.c_str()) == 1) cl->setE( E * (1. - CESfactor) );
		else if(strcmp("Down", m_method.c_str()) == 0) {
			cl->setE( e_cesd );
		}
		//else cl->setE( E * (1. + CESfactor * CES_option2_factors[etabin]) );
		else {
			cl->setE( e_ces );
		}
  }
  return StatusCode::SUCCESS;
}

StatusCode ClusterEnergyScale::process(xAOD::IParticleContainer* cont) const {
  xAOD::CaloClusterContainer* clust = dynamic_cast<xAOD::CaloClusterContainer*> (cont); // Get CaloCluster container
  if(clust) return process(clust);
  return StatusCode::FAILURE;
}






