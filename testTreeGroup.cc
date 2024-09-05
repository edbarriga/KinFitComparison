#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <chrono>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TBranch.h"

using namespace chrono;

vector<const char*> getBranches(TTree *tree)
{
  vector<const char*> locVector;

  TObjArray *arr = tree->GetListOfBranches();

  for (int i = 0; i < arr->GetEntries(); ++i)
    {
      TBranch *b = (TBranch*)arr->At(i);
      const char* b_name = b->GetName();

      // Get branches that vary between TTrees
      if ( strstr( b_name, "__ChargedIndex" ) != NULL ||
           strstr( b_name, "__NeutralIndex" ) != NULL )
	{
	  locVector.push_back( b_name );
	}
      else
	continue; // branch didn't match with anything that should be kept
    }

  return locVector;
}

bool weigtedData(TTree *tree)
{
  bool hasWeight = false;

  TObjArray *arr = tree->GetListOfBranches();

  for (int i = 0; i < arr->GetEntries(); ++i)
    {
      TBranch *b = (TBranch*)arr->At(i);
      const char* b_name = b->GetName();
      // Get branches that vary between TTrees
      if ( strstr( b_name, "weight" ) != NULL )
        hasWeight = true;
      else
	continue; // branch didn't match with anything that should be kept
    }

  return hasWeight;
}


void testTreeGroup(const char *primaryFile, const char *primaryTree, const char *secondaryFile, const char *secondaryTree)
//void testTreeGroup(const char *secondaryFile, const char *secondaryTree,const char *primaryFile, const char *primaryTree )
{
  // For measuring performance
  high_resolution_clock::time_point t1 = high_resolution_clock::now();

  // This number does not have to be arbitrary.
  // The size of arrays may be equal to the total
  // number of combos. Unsure how to extract it though
  // A common error is not having the size of this number
  // to be large enough. At the same time I cannot use
  // the total number of combos ~320k
  int numberForArray = 100000;
  int numberForArray_ID = 100;

  // Here is an example of the structure of the branches
  // needed for the matching
  /*
    RunNumber : RunNumber/i
    EventNumber : EventNumber/l
    NumCombos : NumCombos/i
    IsComboCut : IsComboCut[NumCombos]/O
    ChiSq_KinFit : ChiSq_KinFit[NumCombos]/F
    NDF_KinFit : NDF_KinFit[NumCombos]/i
    ComboBeam__BeamIndex : ComboBeam__BeamIndex[NumCombos]/I
    PiPlus__ChargedIndex : PiPlus__ChargedIndex[NumCombos]/I
    Proton__ChargedIndex : Proton__ChargedIndex[NumCombos]/I
    PiMinus__ChargedIndex : PiMinus__ChargedIndex[NumCombos]/I
    Photon1__NeutralIndex : Photon1__NeutralIndex[NumCombos]/I
    Photon2__NeutralIndex : Photon2__NeutralIndex[NumCombos]/I
    Photon3__NeutralIndex : Photon3__NeutralIndex[NumCombos]/I
    Photon4__NeutralIndex : Photon4__NeutralIndex[NumCombos]/I
    NumBeam   : NumBeam/i
    Beam__PID : Beam__PID[NumBeam]/I 
    NumChargedHypos : NumChargedHypos/i 
    ChargedHypo__TrackID : ChargedHypo__TrackID[NumChargedHypos]/I
    NumNeutralHypos : NumNeutralHypos/i
    NeutralHypo__NeutralID : NeutralHypo__NeutralID[NumNeutralHypos]/I
  */

  TFile *f_primary = TFile::Open(primaryFile, "READ");
  TTree *t_primary = (TTree *)f_primary->Get(primaryTree);
  ULong64_t nEntries = t_primary->GetEntries();
  // We can select what branches we want to write in the new
  // tree by turning them on here. The minimum branches
  // that need to be always on are the ones used to
  // for matching. For example:
  // t_primary->SetBranchStatus("*",0);
  // t_primary->SetBranchStatus("RunNumber",1);
  // t_primary->SetBranchStatus("EventNumber",1);
  // t_primary->SetBranchStatus("NumCombos",1);
  // t_primary->SetBranchStatus("IsComboCut",1);
  // t_primary->SetBranchStatus("Beam__PID",1);
  // t_primary->SetBranchStatus("ChargedHypo__TrackID",1);
  // t_primary->SetBranchStatus("NeutralHypo__NeutralID",1);
  // t_primary->SetBranchStatus("ComboBeam__BeamIndex",1);
  // t_primary->SetBranchStatus("Proton__ChargedIndex",1);
  // t_primary->SetBranchStatus("PiPlus__ChargedIndex",1);
  // t_primary->SetBranchStatus("PiMinus__ChargedIndex",1);
  // t_primary->SetBranchStatus("Photon1__NeutralIndex",1);
  // t_primary->SetBranchStatus("Photon2__NeutralIndex",1);
  // t_primary->SetBranchStatus("Photon3__NeutralIndex",1);
  // t_primary->SetBranchStatus("Photon4__NeutralIndex",1);
  UInt_t run, numCombos;
  ULong64_t event;
  t_primary->SetBranchAddress("RunNumber", &run);
  t_primary->SetBranchAddress("EventNumber", &event);
  t_primary->SetBranchAddress("NumCombos", &numCombos);
  bool isComboCut[numberForArray];
  t_primary->SetBranchAddress("IsComboCut", isComboCut);
  //We can only match the beam through its energy and timing
  TClonesArray *beamP4_Measured = new TClonesArray("TLorentzVector");
  t_primary->SetBranchAddress("Beam__P4_Measured", &beamP4_Measured);
  TClonesArray *beamX4_Measured = new TClonesArray("TLorentzVector");
  t_primary->SetBranchAddress("Beam__X4_Measured", &beamX4_Measured);
  //These are the unique particles ID and what should be compared
  Int_t track_uniqueID[numberForArray_ID];
  t_primary->SetBranchAddress("ChargedHypo__TrackID",track_uniqueID);
  Int_t neutral_uniqueID[numberForArray_ID];
  t_primary->SetBranchAddress("NeutralHypo__NeutralID",neutral_uniqueID);
  //These are the index to the arrays in the arrays above
  //change the name of these variable to index
  Int_t beam_ID[numberForArray]; 
  t_primary->SetBranchAddress("ComboBeam__BeamIndex", beam_ID);
  // Pre-define 7 particles, use only the ones we need
  Int_t particle1_ID[numberForArray],particle2_ID[numberForArray],particle3_ID[numberForArray],particle4_ID[numberForArray],particle5_ID[numberForArray],particle6_ID[numberForArray],particle7_ID[numberForArray];
  vector<const char*> particleNames = getBranches(t_primary); 
  for(int i=0; i < particleNames.size(); i++){
    //  cout << particleNames.at(i) << "\n";
    if( i == 0)
      t_primary->SetBranchAddress( particleNames[0], particle1_ID);
    if( i == 1)
      t_primary->SetBranchAddress( particleNames[1], particle2_ID);
    if( i == 2)
      t_primary->SetBranchAddress( particleNames[2], particle3_ID);
    if( i == 3)
      t_primary->SetBranchAddress( particleNames[3], particle4_ID);
    if( i == 4)
      t_primary->SetBranchAddress( particleNames[4], particle5_ID);
    if( i == 5)
      t_primary->SetBranchAddress( particleNames[5], particle6_ID);
    if( i == 6)
      t_primary->SetBranchAddress( particleNames[6], particle7_ID);
    if( i == 7){
      cout << "Modify Script to add more particles \n";
      return;
    }
  }

    Float_t comboWeight[numberForArray];
    bool check4Weight = weigtedData(t_primary);
   if( check4Weight ){
     // t_primary->SetBranchAddress( "weight", comboWeight);
     t_primary->SetBranchStatus("weight",0);
   }



  TFile *f_secondary = TFile::Open(secondaryFile, "READ");
  TTree *t_secondary = (TTree *)f_secondary->Get(secondaryTree);
  ULong64_t secondary_nEntries = t_secondary->GetEntries();
  // t_secondary->SetBranchStatus("*",0);
  // t_secondary->SetBranchStatus("RunNumber",1);
  // t_secondary->SetBranchStatus("EventNumber",1);
  // t_secondary->SetBranchStatus("NumCombos",1);
  // t_secondary->SetBranchStatus("IsComboCut",1);
  // t_secondary->SetBranchStatus("Beam__PID",1);
  // t_secondary->SetBranchStatus("ChargedHypo__TrackID",1);
  // t_secondary->SetBranchStatus("NeutralHypo__NeutralID",1);
  // t_secondary->SetBranchStatus("Proton__ChargedIndex",1);
  // t_secondary->SetBranchStatus("PiPlus__ChargedIndex",1);
  // t_secondary->SetBranchStatus("PiMinus__ChargedIndex",1);
  // t_secondary->SetBranchStatus("Photon1__NeutralIndex",1);
  // t_secondary->SetBranchStatus("Photon2__NeutralIndex",1);
  // t_secondary->SetBranchStatus("Photon3__NeutralIndex",1);
  // t_secondary->SetBranchStatus("Photon4__NeutralIndex",1);
  // t_secondary->SetBranchStatus("ComboBeam__BeamIndex",1);
  UInt_t secondary_run, secondary_numCombos;
  ULong64_t secondary_event;
  t_secondary->SetBranchAddress("RunNumber", &secondary_run);
  t_secondary->SetBranchAddress("EventNumber", &secondary_event);
  t_secondary->SetBranchAddress("NumCombos", &secondary_numCombos);
  bool secondary_isComboCut[numberForArray];
  t_secondary->SetBranchAddress("IsComboCut", secondary_isComboCut);
  //We can only match the beam through its energy and timing
  TClonesArray *secondary_beamP4_Measured = new TClonesArray("TLorentzVector");
  t_secondary->SetBranchAddress("Beam__P4_Measured", &secondary_beamP4_Measured);
  TClonesArray *secondary_beamX4_Measured = new TClonesArray("TLorentzVector");
  t_secondary->SetBranchAddress("Beam__X4_Measured", &secondary_beamX4_Measured);
  //These are the unique particles ID and what should be compared
  Int_t secondary_track_uniqueID[numberForArray_ID];
  t_secondary->SetBranchAddress("ChargedHypo__TrackID",secondary_track_uniqueID);
  Int_t secondary_neutral_uniqueID[numberForArray_ID];
  t_secondary->SetBranchAddress("NeutralHypo__NeutralID",secondary_neutral_uniqueID);
  //These are the index to the arrays in the arrays above
  //change the name of these variable to index

  Int_t secondary_beam_ID[numberForArray]; 
  t_secondary->SetBranchAddress("ComboBeam__BeamIndex", secondary_beam_ID);
  
  //This needs to be generalized and propagated in the loop
  //Pre-define 7 particles, use only the ones we need
  Int_t secondary_particle1_ID[numberForArray],secondary_particle2_ID[numberForArray],secondary_particle3_ID[numberForArray],secondary_particle4_ID[numberForArray],secondary_particle5_ID[numberForArray],secondary_particle6_ID[numberForArray],secondary_particle7_ID[numberForArray];
  vector<const char*> secondary_particleNames = getBranches(t_secondary); 
  if( secondary_particleNames.size() != particleNames.size() ){
    cout << "Different number of particles on tree\n";
    cout << "we are not ready for that yet\n";
    return;
  }

  for(int i=0; i < secondary_particleNames.size(); i++){
    //cout <<  secondary_particleNames.at(i) << "\n";
    if( i == 0)
      t_secondary->SetBranchAddress( secondary_particleNames[0], secondary_particle1_ID);
    if( i == 1)
      t_secondary->SetBranchAddress( secondary_particleNames[1], secondary_particle2_ID);
    if( i == 2)
      t_secondary->SetBranchAddress( secondary_particleNames[2], secondary_particle3_ID);
    if( i == 3)
      t_secondary->SetBranchAddress( secondary_particleNames[3], secondary_particle4_ID);
    if( i == 4)
      t_secondary->SetBranchAddress( secondary_particleNames[4], secondary_particle5_ID);
    if( i == 5)
      t_secondary->SetBranchAddress( secondary_particleNames[5], secondary_particle6_ID);
    if( i == 6)
      t_secondary->SetBranchAddress( secondary_particleNames[6], secondary_particle7_ID);
    if( i == 7){
      cout << "Modify Script to add more particles \n";
      return;
    }
  }

    Float_t secondary_comboWeight[numberForArray];
    bool secondary_check4Weight = weigtedData(t_secondary);
   if( secondary_check4Weight ){
    t_secondary->SetBranchAddress( "weight", secondary_comboWeight);
   }

  // Create a new file with a clone of one of the trees.
  // I am still unsure if the one that gets clone matters
  // right now cloning the secondary since we want to
  // compare those to the primary
  TFile *f_new = TFile::Open("matchedTree.root", "recreate");
  TTree *t_new = (TTree *)t_primary->CloneTree(0);
  //This is how you could add costume branches:
  //I should add Alex Barnes old branches too
  Int_t new_numCombos;
  TBranch *b_numCombos = t_new->Branch("new_numCombos",&new_numCombos,"new_numCombos/I");
  //  t_new->SetBranchAddress("NumCombos", &secondary_numCombos);
  Float_t newWeight[numberForArray]; 
  TBranch *b_weight;
  //if(!check4Weight){
  b_weight = t_new->Branch("weight",newWeight,"weight[new_numCombos]/F");
     // }
  //Int_t myTest2; TBranch *b_test2 = t_new->Branch("myTest2",&myTest2,"myTest2/I");

  // Unsure if this step is necessary or if it is redundant
  t_primary->BuildIndex("RunNumber", "EventNumber");
  t_secondary->BuildIndex("RunNumber", "EventNumber");
  t_primary->AddFriend(t_secondary,"TF");
  // t_primary->AddFriend(t_new,"TN");

  //It should be a good idea to add a diagnostic set of plots
  // TH1D* diagnostic = new TH1D("plot", "", 100, -20, 20 );
  // TH1D* diagnostic2 = new TH1D("plot2", "", 100, -20, 20 );

  // Good to print structure of tree
  // t_primary->Draw(">>RunNumberList", "RunNumber!=0", "entrylist");
  // t_primary->Print("all");
  // t_secondary->Draw(">>RunNumberList", "RunNumber!=0", "entrylist");
  // t_secondary->Print("all");


  int eventsRead = 0;
  int runEventMatched = 0;
  int combinationMatched = 0;
  int eventsWritten = 0;

  map<Long64_t, vector<Int_t> > combinationsInEvent;
  map<Long64_t, vector<Int_t> > secondary_combinationsInEvent;

  map<Long64_t, double> beamEnergyMap;
  map<Long64_t, double> secondary_beamEnergyMap;
  map<Long64_t, double> beamTimingMap;
  map<Long64_t, double> secondary_beamTimingMap;
  double epsilon = 1e-10; //epsilon used to compare doubles

  //int loopLimit = (secondary_nEntries > nEntries) ? secondary_nEntries : nEntries;
  //cout<< nEntries << "\t" << secondary_nEntries << "\t"<< loopLimit <<"\n";

  for (Long64_t i = 0; i < nEntries; i++){
  // for (Long64_t i = 0; i < 10; i++){
    bool writeEventToFile = false;
    t_primary->GetEntry(i);
     new_numCombos =    numCombos;
    eventsRead++;
    // printf("i=%lld, Run=%d, Event=%d  : fRun=%d, fEvent=%d\n",i,int(run),int(event),int(secondary_run),int(secondary_event));
    // cout<< "primary entry: " << t_primary->GetEntryNumberWithIndex(run,event) << "\tsecondary entry: " << t_secondary->GetEntryNumberWithIndex(run,event) <<"\n";
    if (t_secondary->GetEntryWithIndex(run,event) > 0) {
      // The beauty of the tree friend (and indexing?) is that
      // the events are organized by run and then event number
      //if (event == secondary_event && run == secondary_run){
      //printf("i=%lld, Run=%d, Event=%d  : fRun=%d, fEvent=%d\n",i,int(run),int(event),int(secondary_run),int(secondary_event));
      runEventMatched++;

      //cout<< "primary entry: " << i << "\tsecondary entry: " << t_secondary->GetEntryNumberWithIndex(run,event) <<"\n";

      // Reset Maps
      combinationsInEvent.clear();
      beamTimingMap.clear();
      beamEnergyMap.clear();
      secondary_combinationsInEvent.clear();
      secondary_beamTimingMap.clear();
      secondary_beamEnergyMap.clear();

      // We want to loop over all combinations
      // sometimes one of the two trees have more
      // than the other so we have a loop for each
      for (int comboIndex = 0; comboIndex < numCombos; comboIndex++)
	{
	  // if (i <10)
	  //printf("i=%lld, Run=%d, Event=%d  : fRun=%d, fEvent=%d\n",i,int(run),int(event),int(secondary_run),int(secondary_event));
	  // if (i < 10)
	  //printf("i=%lld, comboIndex=%d, beamID=%d, particle1=%d, particle2=%d, particle3=%d, particle4=%d, particle5=%d, particle6=%d, particle7=%d : fbeamID=%d, fparticle1=%d, fparticle2=%d, fparticle3=%d, fparticle4=%d, fparticle5=%d, fparticle6=%d, fparticle7=%d\n", i, comboIndex, beam_ID[comboIndex], particle1_ID[comboIndex], particle2_ID[comboIndex], particle3_ID[comboIndex], particle4_ID[comboIndex], particle5_ID[comboIndex], particle6_ID[comboIndex], particle7_ID[comboIndex], secondary_beam_ID[comboIndex], secondary_particle1_ID[comboIndex], secondary_particle2_ID[comboIndex], secondary_particle3_ID[comboIndex], secondary_particle4_ID[comboIndex], secondary_particle5_ID[comboIndex], secondary_particle6_ID[comboIndex], secondary_particle7_ID[comboIndex]);
	  //cout <<"\n";

	  // set all comboCuts to true. We will
	  // turn off only the combinations that
	  // have a matching combo
	  // We may want to change this if we want something more general
	  //secondary_isComboCut[comboIndex] = true;
	  isComboCut[comboIndex] = true;
	  // cout << "\nArrayNumber: " << comboIndex<< " bool Primary " <<  isComboCut[comboIndex] <<"\n";
        
	  // We could save time (?) if we were to
	  // skip the combinations that have been Cut
	  // in the primary tree when filling the vector
	  // It caused problems for me, so I am not using
	  // this right now 
	  // if (!isComboCut[comboIndex])

	  vector<Int_t> currentCombination;
	  // Add combo particles from primary
	  currentCombination.push_back(event);
	  // This needs to be generalized
	  currentCombination.push_back(track_uniqueID[particle1_ID[comboIndex]]);
	  currentCombination.push_back(track_uniqueID[particle2_ID[comboIndex]]);
	  currentCombination.push_back(track_uniqueID[particle3_ID[comboIndex]]);
	  currentCombination.push_back(neutral_uniqueID[particle4_ID[comboIndex]]);
	  currentCombination.push_back(neutral_uniqueID[particle5_ID[comboIndex]]);
	  currentCombination.push_back(neutral_uniqueID[particle6_ID[comboIndex]]);
	  currentCombination.push_back(neutral_uniqueID[particle7_ID[comboIndex]]);
	  combinationsInEvent.insert({comboIndex, currentCombination});

	  TLorentzVector *beamP4 = (TLorentzVector*)beamP4_Measured->At(beam_ID[comboIndex]);
	  beamEnergyMap.insert({comboIndex, beamP4->E() });
	  //cout<< beamP4->E() << "\n";
	  TLorentzVector *beamX4 = (TLorentzVector*)beamX4_Measured->At(beam_ID[comboIndex]);
	  beamTimingMap.insert({comboIndex, beamX4->T() });        

	}

      // Running over the the second tree
      for (int comboIndex = 0; comboIndex < secondary_numCombos; comboIndex++)
	{
	  // if (i <10) printf("i=%lld, Run=%d, Event=%d  : fRun=%d, fEvent=%d\n",i,int(run),int(event),int(secondary_run),int(secondary_event));
	  // if (numCombos > secondary_numCombos)
	  //   {
	  //     // cout << "\nArrayNumber: " << comboIndex<< " bool Primary " <<  isComboCut[comboIndex] <<"\n";
	  //     // if (i < 10)
	  //     //  printf("i=%lld,  comboIndex=%d, beamID=%d, particle1=%d, particle2=%d, particle3=%d, particle4=%d, particle5=%d, particle6=%d, particle7=%d : \n", i, comboIndex, beam_ID[comboIndex], particle1_ID[comboIndex], particle2_ID[comboIndex], particle3_ID[comboIndex], particle4_ID[comboIndex], particle5_ID[comboIndex], particle6_ID[comboIndex], particle7_ID[comboIndex]);
	  //     //cout <<"\n";

	  //     isComboCut[comboIndex] = true;
	  //     //  Skip the combinations that have been Cut
	  //     //  in the primary tree. 
	  //     // if (!isComboCut[comboIndex])
	  //     //  continue;

	  
	  // if (i < 10)
	  // printf("i=%lld, comboIndex=%d, \t\t\t\t\t\t\t\t: fbeamID=%d, fparticle1=%d, fparticle2=%d, fparticle3=%d, fparticle4=%d, fparticle5=%d, fparticle6=%d, fparticle7=%d\n", i, comboIndex, secondary_beam_ID[comboIndex], secondary_particle1_ID[comboIndex], secondary_particle2_ID[comboIndex], secondary_particle3_ID[comboIndex], secondary_particle4_ID[comboIndex], secondary_particle5_ID[comboIndex], secondary_particle6_ID[comboIndex], secondary_particle7_ID[comboIndex]);
          
	  // set all comboCuts to true. We will
	  // turn off only the combinations that
	  // have a matching combo
	  //secondary_isComboCut[comboIndex] = true;
	  // isComboCut[comboIndex] = true;
	  
	  vector<Int_t> secondary_currentCombination;
	  // Add combo particles from secondary
	  secondary_currentCombination.push_back(secondary_event);
	  // This needs to be generalized
	  secondary_currentCombination.push_back(secondary_track_uniqueID[secondary_particle1_ID[comboIndex]]);
	  secondary_currentCombination.push_back(secondary_track_uniqueID[secondary_particle2_ID[comboIndex]]);
	  secondary_currentCombination.push_back(secondary_track_uniqueID[secondary_particle3_ID[comboIndex]]);
	  secondary_currentCombination.push_back(secondary_neutral_uniqueID[secondary_particle4_ID[comboIndex]]);
	  secondary_currentCombination.push_back(secondary_neutral_uniqueID[secondary_particle5_ID[comboIndex]]);
	  secondary_currentCombination.push_back(secondary_neutral_uniqueID[secondary_particle6_ID[comboIndex]]);
	  secondary_currentCombination.push_back(secondary_neutral_uniqueID[secondary_particle7_ID[comboIndex]]);
	  secondary_combinationsInEvent.insert({comboIndex, secondary_currentCombination});

	  TLorentzVector *secondary_beamP4 = (TLorentzVector*)secondary_beamP4_Measured->At(secondary_beam_ID[comboIndex]);
	  secondary_beamEnergyMap.insert({comboIndex, secondary_beamP4->E() });
	  //cout<< secondary_beamP4->E() << "\n";
	  TLorentzVector *secondary_beamX4 = (TLorentzVector*)secondary_beamX4_Measured->At(secondary_beam_ID[comboIndex]);
	  secondary_beamTimingMap.insert({comboIndex, secondary_beamX4->T() });

	  // if(!check4Weight)
	  newWeight[comboIndex] = secondary_comboWeight[comboIndex];

	}

      // Loop over the maps to see if there are matching combinations
      // do the order matters for the search?
      // Maybe use Alex code here
      for (auto itr = combinationsInEvent.begin(); itr != combinationsInEvent.end(); itr++)
	{
	  for (auto itr2 = secondary_combinationsInEvent.begin(); itr2 != secondary_combinationsInEvent.end(); itr2++)
	    {
	      // cout << "primary: " <<  itr->first  <<"\t" << itr->second[0] << "\t"<<itr->second[1] << "\t"<<itr->second[2] << "\t"<<itr->second[3] << "\t"<<itr->second[4] <<"\t"<<itr->second[5] << "\t"<<itr->second[6] << "\t"<<itr->second[7]<< "\t" <<itr->second[8] <<"\n";
	      // cout << "secondary: " <<  itr2->first  <<"\t"<<itr2->second[0]<< "\t"<<itr2->second[1] << "\t"<<itr2->second[2] << "\t"<<itr2->second[3] << "\t"<<itr2->second[4] <<"\t"<<itr2->second[5] << "\t"<<itr2->second[6] << "\t"<<itr2->second[7]<< "\t" <<itr2->second[8]<<"\n";
	      //cout << "secondaryT: " <<secondary_event<< "\t"<< secondary_beam_ID[itr2->first]<<"\t"<<secondary_particle1_ID[itr2->first]<< "\t"<<secondary_particle2_ID[itr2->first] << "\t"<<secondary_particle3_ID[itr2->first] << "\t"<<secondary_particle4_ID[itr2->first] << "\t"<<secondary_particle5_ID[itr2->first] <<"\t"<<secondary_particle6_ID[itr2->first] << "\t"<<secondary_particle7_ID[itr2->first] <<"\n\n";
	      // cout << "Entry " << i << "\tfirst\t" << itr->first << "\tsecond\t" << itr2->first << "\n";
	      // cout<< "beam 1: "<< setprecision(11) << beamEnergyMap.at(itr->first)<< "\tbeam 2: "<< setprecision(11) << secondary_beamEnergyMap.at(itr2->first)  << "\n";
	      // cout<< "beam 1: "<< setprecision(11) << beamTimingMap.at(itr->first)<< "\tbeam 2: "<< setprecision(11) << secondary_beamTimingMap.at(itr2->first)  << "\n";
	      if (itr->second == itr2->second)
		{
		  bool beamEnergyMatched = abs(secondary_beamEnergyMap.at(itr2->first) - beamEnergyMap.at(itr->first) ) < epsilon;
		  bool beamTimeMatched = abs(secondary_beamTimingMap.at(itr2->first) - beamTimingMap.at(itr->first) ) < epsilon;
		  bool beamMatched = beamEnergyMatched && beamTimeMatched;
		  if( beamMatched ){
		    // cout << "Entry " << i << "\tfirst\t" << itr->first << "\tsecond\t" << itr2->first << "\n";
		    //cout<< "beam 1: "<< setprecision(11) << beamEnergyMap.at(itr->first)<< "\tbeam 2: "<< setprecision(11) << secondary_beamEnergyMap.at(itr2->first)  << "\n";
		    //cout<< "beam 1: "<< setprecision(11) << beamTimingMap.at(itr->first)<< "\tbeam 2: "<< setprecision(11) << secondary_beamTimingMap.at(itr2->first)  << "\n";
		    //cout << "before bool1 " <<  isComboCut[itr->first] << " bool 2: " <<secondary_isComboCut[itr2->first]<<"\n";
		    // The current scheme loops over all combinations.
		    // for this reason we make the matched combination
		    // to have the same "cut" status as the primary
		    // this could be generalized
		    isComboCut[itr->first] = secondary_isComboCut[itr2->first];
		    // cout << "after  bool1 " <<  isComboCut[itr->first] << " bool 2: " <<secondary_isComboCut[itr2->first]<<"\n";
		    // cout << "primary: " <<  itr->first  <<"\t" << itr->second[0] << "\t"<<itr->second[1] << "\t"<<itr->second[2] << "\t"<<itr->second[3] << "\t"<<itr->second[4] <<"\t"<<itr->second[5] << "\t"<<itr->second[6] << "\t"<<itr->second[7]<<"\n";
		    // cout << "secondary: " <<  itr2->first  <<"\t"<<itr2->second[0]<< "\t"<<itr2->second[1] << "\t"<<itr2->second[2] << "\t"<<itr2->second[3] << "\t"<<itr2->second[4] <<"\t"<<itr2->second[5] << "\t"<<itr2->second[6] << "\t"<<itr2->second[7] <<"\n";
		    // cout << "secondaryT: " <<secondary_event<< "\t"<< secondary_beam_ID[itr2->first]<<"\t"<<secondary_particle1_ID[itr2->first]<< "\t"<<secondary_particle2_ID[itr2->first] << "\t"<<secondary_particle3_ID[itr2->first] << "\t"<<secondary_particle4_ID[itr2->first] << "\t"<<secondary_particle5_ID[itr2->first] <<"\t"<<secondary_particle6_ID[itr2->first] << "\t"<<secondary_particle7_ID[itr2->first] <<"\n\n";


		    //if (secondary_isComboCut[itr2->first] == false){
		    if (isComboCut[itr->first] == false){
		      // As long as a single combo matches, the whole
		      // event has to be written to file
		      // Maybe this needs to be moved in the if statement above
		      // that will prevent us from writting events
		      // that may have match but have all the combos marked 
		      // as cut
		      combinationMatched++;
		      // cout << "after  bool1 " <<  isComboCut[itr->first] << " bool 2: " <<secondary_isComboCut[itr2->first]<<"\n";
		      // cout << "primary: " <<  itr->first  <<"\t" << itr->second[0] << "\t"<<itr->second[1] << "\t"<<itr->second[2] << "\t"<<itr->second[3] << "\t"<<itr->second[4] <<"\t"<<itr->second[5] << "\t"<<itr->second[6] << "\t"<<itr->second[7]<< "\t" <<itr->second[8] <<"\n";
		      // cout << "secondary: " <<  itr2->first  <<"\t"<<itr2->second[0]<< "\t"<<itr2->second[1] << "\t"<<itr2->second[2] << "\t"<<itr2->second[3] << "\t"<<itr2->second[4] <<"\t"<<itr2->second[5] << "\t"<<itr2->second[6] << "\t"<<itr2->second[7]<< "\t" <<itr2->second[8]<<"\n";
		      // cout << "secondaryT: " <<secondary_event<< "\t"<< secondary_beam_ID[itr2->first]<<"\t"<<secondary_particle1_ID[itr2->first]<< "\t"<<secondary_particle2_ID[itr2->first] << "\t"<<secondary_particle3_ID[itr2->first] << "\t"<<secondary_particle4_ID[itr2->first] << "\t"<<secondary_particle5_ID[itr2->first] <<"\t"<<secondary_particle6_ID[itr2->first] << "\t"<<secondary_particle7_ID[itr2->first] <<"\n\n";
		      writeEventToFile = true;
		    }

		  }

		}
	    }
	}

    }

    if (writeEventToFile){
      //cout<<"\t"<<event<<"\t"<<secondary_event<<"\n";
      //cout << "\t combos: \t"<< new_numCombos << "\n";
      eventsWritten++;
      //if(!check4Weight){
      //	new_numCombos = secondary_numCombos;
	t_new->Fill();
	//b_weight->Fill();
	// }
    }
    
  }

  t_new->Write("", TObject::kOverwrite);                 // save the new version of the tree

  cout << "\nTotal events read: " << eventsRead << "\n"; // events = entries here
  cout << "Number of entries with the same run and event number: " << runEventMatched << "\n";
  cout << "Number of matched combinations kept: " << combinationMatched << "\n";
  cout << "Number of expected events in new tree: " << eventsWritten << "\n";
  // For measuring performance
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << "The process took: " << duration * 1E-6 << " seconds" << endl;
}

/*
////This could be useful for the events that do not have a match in the other file


// Load the library at macro parsing time: we need this to use its content in the code
R__LOAD_LIBRARY($ROOTSYS/test/libEvent.so)

void copytree2()
{

TString dir = "$ROOTSYS/test/Event.root";
gSystem->ExpandPathName(dir);
const auto filename = gSystem->AccessPathName(dir) ? "./Event.root" : "$ROOTSYS/test/Event.root";

TFile oldfile(filename);
TTree *oldtree;
oldfile.GetObject("T", oldtree);

// Activate only four of them
for (auto activeBranchName : {"event", "fNtrack", "fNseg", "fH"}) {
oldtree->SetBranchStatus(activeBranchName, 1);
}

// Create a new file + a clone of old tree header. Do not copy events
TFile newfile("small.root", "recreate");
auto newtree = oldtree->CloneTree(0);

// Divert branch fH to a separate file and copy all events
newtree->GetBranch("fH")->SetFile("small_fH.root");
newtree->CopyEntries(oldtree);

newtree->Print();
newfile.Write();
}


*/
