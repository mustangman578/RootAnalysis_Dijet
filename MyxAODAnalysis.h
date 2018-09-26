#ifndef MyAnalysis_MyxAODAnalysis_H
#define MyAnalysis_MyxAODAnalysis_H

#include <AnaAlgorithm/AnaAlgorithm.h>
// GRL
#include <AsgAnalysisInterfaces/IGoodRunsListSelectionTool.h>
#include <AsgTools/AnaToolHandle.h>

// Histograms
#include <TH1.h>
#include <TH2.h>

// Jet Cleaning
#include <JetInterface/IJetSelector.h>

// JER
#include <JetResolution/IJERTool.h>

// Jet Calibration Tool
#include <JetCalibTools/IJetCalibrationTool.h>
#include "JetCalibTools/JetCalibrationTool.h"

// Jet Vertexing Tool
#include <JetMomentTools/JetVertexTaggerTool.h> //Working on this 

// Ntuple
#include <TTree.h>
#include <vector>


class MyxAODAnalysis : public EL::AnaAlgorithm
{
public:
  // this is a standard algorithm constructor
  MyxAODAnalysis (const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor for Ntuples
  ~MyxAODAnalysis();

  // GRL
  asg::AnaToolHandle<IGoodRunsListSelectionTool> m_grl; //!
  // Jet Cleaning
  asg::AnaToolHandle<IJetSelector> m_jetCleaning; //!
  // JER
  //asg::AnaToolHandle<IJERTool> m_JERTool; //!

  // Jet Calibration 
  asg::AnaToolHandle<IJetCalibrationTool> m_JetCalibrationTool_handle; //!
  

  // Jet Vertex Tool 
  //asg::AnaToolHandle<IJetUpdateJvt> m_jvt; //!  //Working on this 

  //Dr. White Code JVT
  //JetVertexTaggerTool* pjvtag; //!
  //ToolHandle<IJetUpdateJvt> hjvtagup; //!


  // Creating trees/Ntuples
  unsigned int m_runNumber = 0; ///< Run number for the current event
  unsigned long long m_eventNumber = 0; ///< Event number
   /// Jet 4-momentum variables 
   std::vector<float> *m_jetEta = nullptr;
   std::vector<float> *m_jetPhi = nullptr;
   std::vector<float> *m_jetPt = nullptr;
   //std::vector<float> *m_jetE = nullptr;

   //calibratetd jets Total
   std::vector<float> *m_jetEta_calib = nullptr;
   std::vector<float> *m_jetPt_calib = nullptr;
   std::vector<float> *m_jetPhi_calib = nullptr;
   //calibrated jets individual
   std::vector<float> *m_jetPhi_calib_1 = nullptr;
   std::vector<float> *m_jetEta_calib_1 = nullptr;
   std::vector<float> *m_jetPt_calib_1 = nullptr;
   std::vector<float> *m_jetPhi_calib_2 = nullptr;
   std::vector<float> *m_jetEta_calib_2 = nullptr;
   std::vector<float> *m_jetPt_calib_2 = nullptr;
   std::vector<float> *m_jetPhi_calib_3 = nullptr;
   std::vector<float> *m_jetEta_calib_3 = nullptr;
   std::vector<float> *m_jetPt_calib_3 = nullptr;

  // these are the functions inherited from Algorithm
  virtual StatusCode initialize () override;
  virtual StatusCode execute () override;
  virtual StatusCode finalize () override;

private:
  // Configuration, and any other types of variables go here.
  //float m_cutValue;
  //TTree *m_myTree;
  //TH1 *m_myHist;
  
  int total_grl; //!
  int event_number; //!
  int index = 0; //!
  unsigned numGoodJets; //!
  unsigned numGoodJets_calib; //!
  unsigned jet_count; //!
  
  //int Njets; //!
  
};

#endif
