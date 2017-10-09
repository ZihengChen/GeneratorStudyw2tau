// main07.cc is a part of the PYTHIA event generator.
// Copyright (C) 2017 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Illustration how to generate various two-body channels from
// astroparticle processes, e.g. neutralino annihilation or decay.
// To this end a "blob" of energy is created with unit cross section,
// from the fictitious collision of two non-radiating incoming e+e-.
// In the accompanying main29.cmnd file the decay channels of this
// blob can be set up. Furthermore, only gamma, e+-, p/pbar and
// neutrinos are stable, everything else is set to decay.
// (The "single-particle gun" of main21.cc offers another possible
// approach to the same problem.)
#include "Pythia8/Pythia.h"


using namespace Pythia8;



int main() {
  ofstream eventsfeatures;
  // Pythia generator.
  Pythia pythia;

  // Read in the rest of the settings and data from a separate file.
  pythia.readFile("card/ttbar_mumu.cmnd"); eventsfeatures.open("../data/ttbar_mumu.csv"); 
  // pythia.readFile("card/ttbar_mutau_mumu.cmnd"); eventsfeatures.open("../data/ttbar_mutau_mumu.csv");
  //pythia.readFile("card/ttbar_tautau_mumu.cmnd"); eventsfeatures.open("../data/ttbar_tautau_mumu.csv");

    // lepton
    eventsfeatures << "lepton1_pt,lepton1_energy,lepton1_eta,lepton1_phi,lepton1_charge,lepton1_mother,";
    eventsfeatures << "lepton2_pt,lepton2_energy,lepton2_eta,lepton2_phi,lepton2_charge,lepton2_mother,";
    eventsfeatures << "dilepton_pt,dilepton_energy,dilepton_eta,dilepton_phi,dilepton_m,lepton_deltar,lepton_deltaphi,";
    // bjet
    eventsfeatures << "jet1_pt,jet1_energy,jet1_eta,jet1_phi,jet1_charge,";
    eventsfeatures << "jet2_pt,jet2_energy,jet2_eta,jet2_phi,jet2_charge,";
    eventsfeatures << "dijet_pt,dijet_energy,dijet_eta,dijet_phi,dijet_m,jet_deltar,jet_deltaphi,";
    // lb
    eventsfeatures << "lb1_pt,lb1_energy,lb1_eta,lb1_phi,lb1_m,lb1_deltar,lb1_deltaphi,";
    eventsfeatures << "lb2_pt,lb2_energy,lb2_eta,lb2_phi,lb2_m,lb2_deltar,lb2_deltaphi,";
    eventsfeatures << "dilb_pt,dilb_energy,dilb_eta,dilb_phi,dilb_m,lb_deltar,lb_deltaphi,";
    // met
   eventsfeatures << "met,metphi,met1_pt,met1_eta,met1_phi,met1_mt,met2_pt,met2_eta,met2_phi,met2_mt"<<endl;


  // Initialization.
  pythia.init();

  // Extract settings to be used in the main program.
  int nEvent  = pythia.mode("Main:numberOfEvents");
  int nAbort  = pythia.mode("Main:timesAllowErrors");


  // Begin event loop.
  int iAbort = 0;
  for (int iEvent = 0; iEvent < nEvent; ++iEvent) {

    // Generate events. Quit if many failures.
    if (!pythia.next()) {
      if (++iAbort < nAbort) continue;
      cout << " Event generation aborted prematurely, owing to error!\n";
      break;
    }
  
    Particle lep1,  lep2;
    Particle bjet1, bjet2;
    Vec4 met1, met2;
    Vec4 invisible;

    // Loop over all particles and analyze the final-state ones.
    for (int i = 0; i < pythia.event.size(); ++i){
      if ( ! pythia.event[i].isFinal()) continue;

      if (pythia.event[i].id() == -13 ) lep1 = pythia.event[i];  
      if (pythia.event[i].id() ==  13 ) lep2 = pythia.event[i];
      
      if ( (pythia.event[i].id() ==  14) || 
           (pythia.event[i].id() ==  16 && pythia.event[pythia.event[i].mother1()].id() ==  24) ||
           (pythia.event[i].id() == -16 && pythia.event[pythia.event[i].mother1()].id() == -15) 
          ){
        met1 = met1 + pythia.event[i].p();
      }
      if ( (pythia.event[i].id() == -14) || 
           (pythia.event[i].id() == -16 && pythia.event[pythia.event[i].mother1()].id() == -24) ||
           (pythia.event[i].id() ==  16 && pythia.event[pythia.event[i].mother1()].id() ==  15) 
         ){
        met2 = met2 + pythia.event[i].p();
      }
        
      if (pythia.event[i].id() ==  5 && pythia.event[pythia.event[i].mother1()].id() ==  6) bjet1 = pythia.event[i];
      if (pythia.event[i].id() == -5 && pythia.event[pythia.event[i].mother1()].id() == -6) bjet2 = pythia.event[i];
      if (pythia.event[i].isVisible()) invisible = invisible+pythia.event[i].p();
    }// End of paticle loop.

    Particle temp;
    Vec4 tmet;
    if (lep1.pT() < lep2.pT()){
      temp = lep1; lep1 = lep2; lep2 = temp;
      tmet = met1; met1 = met2; met2 = tmet;
    }
    if (bjet1.pT() < bjet2.pT()){
      temp  = bjet1; bjet1 = bjet2; bjet2 = temp;
    }

    // Calculate if this event passes kinematic cuts
    bool eventselection = true;
    eventselection = eventselection && (lep1.pT()>25) && (abs(lep1.eta())<2.4);
    eventselection = eventselection && (lep2.pT()>3) && (abs(lep2.eta())<2.4);
    eventselection = eventselection && (bjet1.pT()>30) && (abs(bjet1.eta())<2.4);
    eventselection = eventselection && (bjet2.pT()>30) && (abs(bjet2.eta())<2.4);


    //////////////////////////////////////////////////
    ///////////////// write event ////////////////////
    //////////////////////////////////////////////////
    if (eventselection){
      // BLT reader
      // 1. lepton, dilepton
      eventsfeatures<< lep1.pT()  << "," << lep1.e() << "," << lep1.eta() << "," << lep1.phi() << "," << lep1.charge() << "," << pythia.event[lep1.mother1()].id() << ",";
      eventsfeatures<< lep2.pT()  << "," << lep2.e() << "," << lep2.eta() << "," << lep2.phi() << "," << lep2.charge() << "," << pythia.event[lep2.mother1()].id() << ",";
      Vec4 dilepton = lep1.p()+lep2.p();
      eventsfeatures<< dilepton.pT() << ", " << dilepton.e() << "," << dilepton.eta() << "," << dilepton.phi() << "," << dilepton.mCalc() << ",";
      eventsfeatures<< REtaPhi(lep1.p(),lep2.p()) << "," << phi(lep1.p(),lep2.p())<< ",";

      // 2. bjet, dijet
      eventsfeatures<< bjet1.pT() << "," << bjet1.e()<< "," << bjet1.eta()<< "," << bjet1.phi()<< "," << bjet1.charge()<< ",";
      eventsfeatures<< bjet2.pT() << "," << bjet2.e()<< "," << bjet2.eta()<< "," << bjet2.phi()<< "," << bjet2.charge()<< ",";
      Vec4 dijet = bjet1.p()+bjet2.p();
      eventsfeatures<< dijet.pT() << ", " << dijet.e() << "," << dijet.eta() << "," << dijet.phi() << "," << dijet.mCalc() << ",";
      eventsfeatures<< REtaPhi(bjet1.p(),bjet2.p()) << "," << phi(bjet1.p(),bjet2.p())<< ",";
      
      // 3. lb system
      // pairing id as (5&-13) and (-5&13) 
      if (lep1.id()*bjet1.id() < 0){
        Vec4 lb1 = lep1.p()+bjet1.p();
        eventsfeatures<< lb1.pT() << ", " << lb1.e() << "," << lb1.eta() << "," << lb1.phi() << "," << lb1.mCalc() << ",";
        eventsfeatures<< REtaPhi(lep1.p(),bjet1.p()) << "," << phi(lep1.p(),bjet1.p())<< ",";
        Vec4 lb2 = lep2.p()+bjet2.p();
        eventsfeatures<< lb2.pT() << ", " << lb2.e() << "," << lb2.eta() << "," << lb2.phi() << "," << lb2.mCalc() << ",";
        eventsfeatures<< REtaPhi(lep2.p(),bjet2.p()) << "," << phi(lep2.p(),bjet2.p())<< ",";
        
        eventsfeatures<< REtaPhi(lb1,lb2) << "," << phi(lb1,lb2) << ",";
        Vec4 dilb = lb1+lb2;
        eventsfeatures<< dilb.pT() << ", " << dilb.e() << "," << dilb.eta() << "," << dilb.phi() << "," << dilb.mCalc() << ",";
        
      }
      else{  
        Vec4 lb1 = lep1.p()+bjet2.p();
        eventsfeatures<< lb1.pT() << ", " << lb1.e() << "," << lb1.eta() << "," << lb1.phi() << "," << lb1.mCalc() << ",";
        eventsfeatures<< REtaPhi(lep1.p(),bjet2.p()) << "," << phi(lep1.p(),bjet2.p())<< ",";
        Vec4 lb2 = lep2.p()+bjet1.p();
        eventsfeatures<< lb2.pT() << ", " << lb2.e() << "," << lb2.eta() << "," << lb2.phi() << "," << lb2.mCalc() << ",";
        eventsfeatures<< REtaPhi(lep2.p(),bjet1.p()) << "," << phi(lep2.p(),bjet1.p())<< ",";

        eventsfeatures<< REtaPhi(lb1,lb2) << "," << phi(lb1,lb2) << ",";
        Vec4 dilb = lb1+lb2;
        eventsfeatures<< dilb.pT() << ", " << dilb.e() << "," << dilb.eta() << "," << dilb.phi() << "," << dilb.mCalc() << ",";
        
      }
      

      // 4. MET
      eventsfeatures<< invisible.pT() << "," << invisible.phi() << ",";
      eventsfeatures<< met1.pT() << "," << met1.eta() << "," << met1.phi() << "," << sqrt( 2*met1.pT()*lep1.pT()*(1-costheta(met1,lep1.p()))  ) << ",";
      eventsfeatures<< met2.pT() << "," << met2.eta() << "," << met2.phi() << "," << sqrt( 2*met2.pT()*lep2.pT()*(1-costheta(met2,lep2.p()))  ) << endl;
      
    }
    //////////////////////////////////////////////////
    /////////////// finish writing ///////////////////
    //////////////////////////////////////////////////

  }// End of event loop.

  // Final statistics and histograms.
  pythia.stat();
  eventsfeatures.close();
  return 0;
}
