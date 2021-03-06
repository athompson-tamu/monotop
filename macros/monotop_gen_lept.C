#define monotop_gen_lept_cxx
// The class definition in monotop_gen_lept.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("monotop_gen_lept.C")
// Root > T->Process("monotop_gen_lept.C","some options")
// Root > T->Process("monotop_gen_lept.C+")
//

#include "monotop_gen_lept.h"
#include <TH2.h>
#include <TStyle.h>
#include <TParticle.h>

TH1F *h_pt_ratio;
TH1F *h_ratio_with_vm;

void monotop_gen_lept::Begin(TTree * /*tree*/) {
  TString option = GetOption();
}

void monotop_gen_lept::SlaveBegin(TTree * /*tree*/) {
  TString option = GetOption();

  // PT ratio.
  h_pt_ratio = new TH1F("pt_ratio_right", "Pt ratio for RH monotop, W->mu,vm", 100, 0., 1.);
  h_pt_ratio->GetXaxis()->SetTitle("Pt(b)/(Pt(b) + Pt(mu))");
  h_pt_ratio->GetYaxis()->SetTitle("Nb events");

  h_ratio_with_vm = new TH1F("pt_r_vm_right", "", 100, 0., 1.);
}

Bool_t monotop_gen_lept::Process(Long64_t entry) {
  // Get each event.
  GetEntry(entry);

  // P4 for the muon and the b
  TLorentzVector b;
  TLorentzVector mu;

  // Loop over particles.
  Int_t b_i = -1;
  Int_t mu_i = -1;
  Int_t vm_i = -1;
  for (Int_t i = 0; i<Particle_size; i++) {
    // Store the index of the muon.
    if (abs(Particle_PID[i]) == 13) {
      mu_i = i;
    }

    // Store the index of the final state b.
    if (abs(Particle_PID[i]) == 5 && Particle_Status[i] == 1) {
      b_i = i;
    }

    // Get the neutrino.
    if (abs(Particle_PID[i]) == 14) {
      vm_i = i;
    }
  }

  if (vm_i >= 0 && mu_i >= 0 && b_i >= 0) {
    TLorentzVector b(Particle_Px[b_i], Particle_Py[b_i],
                     Particle_Pz[b_i], Particle_E[b_i]);
    TLorentzVector mu(Particle_Px[mu_i], Particle_Py[mu_i],
                      Particle_Pz[mu_i], Particle_E[mu_i]);

    // Define kinematics.
    Double_t delta_phi_bl = abs(b.Phi() - mu.Phi());
    Double_t b_eta = abs(b.Eta());
    Double_t vm_pt = Particle_PT[vm_i];
    Double_t pt_ratio = b.Pt() / (b.Pt() + mu.Pt());
    Double_t pt_ratio_with_vm = b.Pt() / (b.Pt() + mu.Pt() + vm_pt);

    // Make cuts.
    bool cut_b_pt = (b.Pt() > 70.);
    bool cut_l_pt = (mu.Pt() > 30.);
    bool cut_delta_phi = (delta_phi_bl < 1.7);
    bool cut_eta = (b_eta < 2.5);
    bool passed_cuts = (cut_b_pt && cut_eta && cut_l_pt && cut_delta_phi);

    if (passed_cuts) {
      h_pt_ratio->Fill(pt_ratio);
      h_ratio_with_vm->Fill(pt_ratio_with_vm);
    }
  }

  return kTRUE;
}

void monotop_gen_lept::SlaveTerminate() {
  TFile *f_left_gen = new TFile("pt_hist_right_gen.root", "RECREATE");
  h_pt_ratio->Write();
  f_left_gen->Close();

  TFile *f_left_gen_vm = new TFile("pt_ratio_with_vm_right.root", "RECREATE");
  h_ratio_with_vm->Write();
  f_left_gen_vm->Close();
}

void monotop_gen_lept::Terminate() {}


