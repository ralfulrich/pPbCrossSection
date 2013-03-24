#define MAXEVT 50000

#include "TTree.h"
#include "TMath.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include <iostream>
#include <vector>
#include <string>

//#include "style.h"

enum e_type
{
  MC = 0,
  DATA
};

vector<string> sample_fname;
vector<string> sample_name;
vector<e_type> sample_type;
using namespace std;

void csana()
{
  TH1::SetDefaultSumw2();
  gROOT->ProcessLine(".L style.cc+");
  //style();
  //**********************************************INPUT******************************
  sample_fname.push_back("root://eoscms//eos/cms/store/group/phys_heavyions/velicanu/forest/PA2013_HiForest_Express_r210614_autoforest_v51.root"); sample_name.push_back("data"); sample_type.push_back(DATA);
  sample_fname.push_back("root://eoscms//eos/cms/store/group/phys_heavyions/azsigmon/HiForest_pPb_Epos_336800.root"); sample_name.push_back("Epos"); sample_type.push_back(MC); 
  sample_fname.push_back("root://eoscms//eos/cms//store/caf/user/dgulhan/pPb_Hijing_MB/HiForest_v03_mergedv02/merged_forest_0.root"); sample_name.push_back("HIJING"); sample_type.push_back(MC); 
  sample_fname.push_back("root://eoscms//eos/cms/store/group/phys_heavyions/cbaus/pPb_5020_QGSJetII_5_3_8_HI_patch2/forest.root"); sample_name.push_back("QGSJetII"); sample_type.push_back(MC);
  sample_fname.push_back("/afs/cern.ch/user/t/tuos/work/public/pPb/starlight/starlight_pythia_pPb.root"); sample_name.push_back("Starlight_Pythia");  sample_type.push_back(MC);
  sample_fname.push_back("/afs/cern.ch/user/t/tuos/work/public/pPb/starlight/starlight_dpmjet_pPb.root"); sample_name.push_back("Starlight_DPMJet");  sample_type.push_back(MC);

  const int n_cas_rechits = 224;
  const int n_hf_rechits = 1728;
  
  int hf_n;
  float hf_e[n_hf_rechits];
  int hf_depth[n_hf_rechits];
  float hf_eta[n_hf_rechits];

  float cas_e[n_cas_rechits];
  int cas_sat[n_cas_rechits];
  int cas_depth[n_cas_rechits];
  int cas_iphi[n_cas_rechits];

  float fsc_sum_minus;
  float fsc_sum_plus;

  int nTrk;

  int zero_bias;
  int min_bias;
  int random;
  int bptx_p_m;
  int bptx_p_nm;
  int bptx_np_m;

  int lumi;

  //*********************************OUTPUT************************************

  TFile* out_file = new TFile("histos_EM.root","RECREATE");

  TH1D* h_zero_count_zb_coll;
  TH1D* h_zero_count_zb_no_coll;
  TH1D* h_hf_hits_coll;
  TH1D* h_hf_hits_plus;
  TH1D* h_hf_hits_minus;
  TH1D* h_hf_hits_noise;
  TH1D* h_hf_hits_beam_gas;
  TH1D* h_castor_hf_diff_3;
  TH1D* h_castor_hf_diff_5;
  TH1D* h_castor_hf_diff_10;
  TH1D* h_castor_gap_hf;
  TH1D* h_castor_nogap_hf;
  TH1D* h_eff;

  TH2D* h_hf_fsc_p;
  TH2D* h_hf_fsc_m;

  TH1D* h_hf_cut_single;
  TH1D* h_hf_cut_single_noise;
  TH1D* h_hf_cut_double;
  TH1D* h_hf_cut_double_noise;

  TProfile* h_hf_hits_coll_lumi;
  TProfile* h_hf_hits_plus_lumi;
  TProfile* h_hf_hits_minus_lumi;
  TProfile* h_hf_hits_noise_lumi;
  TProfile* h_hf_totE_coll_lumi;
  TProfile* h_hf_totE_plus_lumi;
  TProfile* h_hf_totE_minus_lumi;
  TProfile* h_hf_totE_noise_lumi;
  TH1D* h_lumi;
  TH1D* h_lumi_3GeV;

  TH1D* h_perf_hf_rechits_single_3gev;
  TH1D* h_perf_hf_rechits_double_1dot5gev;
  TH1D* h_perf_hf_totE_single_3gev;
  TH1D* h_perf_hf_totE_double_1dot5gev;
  TH1D* h_perf_hf_totE_eta_single_3gev;
  TH1D* h_perf_hf_totE_eta_double_1dot5gev;

  //**********************************LOOP*************************************

  for (int sample=0; sample<int(sample_name.size()); sample++)
    {
      TFile* file = NULL; //****FILE
      file = TFile::Open(sample_fname[sample].c_str());
      if (!file || !file->IsOpen() || file->IsZombie()) cerr << "Cannot find file" << endl;
      else cout << "file opened" << endl;

      TFile* file2 = NULL; //****FILE
      file2 = TFile::Open("./fsctree_614.root");
      if (!file2 || !file2->IsOpen() || file2->IsZombie()) cerr << "Cannot find file" << endl;
      else cout << "file2 opened" << endl;
      
      TTree* tree = NULL; //****TREE0
      ((TDirectory*)file->Get("rechitanalyzer"))->GetObject("castor",tree);
      if (!tree) cerr << "Cannot find tree" << endl;
      
      TTree* friend_tree0 = NULL; //****TREE1
      ((TDirectory*)file->Get("pfTowers"))->GetObject("hf",friend_tree0);
      if (!friend_tree0) cerr << "Cannot find friend tree" << endl;
      tree->AddFriend(friend_tree0);
      
      TTree* friend_tree1 = NULL; //****TREE2
      ((TDirectory*)file->Get("hltanalysis"))->GetObject("HltTree",friend_tree1);
      if (!friend_tree1) cerr << "Cannot find friend tree" << endl;
      tree->AddFriend(friend_tree1);
      
      TTree* friend_tree2 = NULL; //****TREE3
      ((TDirectory*)file->Get("pixelTrack"))->GetObject("trackTree",friend_tree2);
      if (!friend_tree2) cerr << "Cannot find friend tree" << endl;
      tree->AddFriend(friend_tree2);

      TTree* friend_tree3 = NULL; //****TREE4
      file2->GetObject("fsctree",friend_tree3);
      if (!friend_tree3) cerr << "Cannot find friend tree" << endl;
      tree->AddFriend(friend_tree3); 
     
      TTree* friend_tree4 = NULL; //****TREE3
      ((TDirectory*)file->Get("hiEvtAnalyzer"))->GetObject("HiTree",friend_tree4);
      if (!friend_tree4) cerr << "Cannot find friend tree" << endl;
      tree->AddFriend(friend_tree4);
      //________________________________________________________________________________
      
      tree->SetBranchStatus("*",0);

      tree->SetBranchStatus("hf.n",1);
      tree->SetBranchStatus("hf.e",1);
      tree->SetBranchStatus("hf.depth",1);
      tree->SetBranchStatus("hf.eta",1);

      tree->SetBranchStatus("e",1);
      tree->SetBranchStatus("saturation",1);
      tree->SetBranchStatus("depth",1);
      tree->SetBranchStatus("iphi",1);

      tree->SetBranchStatus("fsc_sum_plus",1);
      tree->SetBranchStatus("fsc_sum_minus",1);

      tree->SetBranchStatus("nTrk",1);

      tree->SetBranchStatus("HLT_PAZeroBias_v1",1);
      tree->SetBranchStatus("HLT_PAL1Tech53_MB_SingleTrack_v1",1);
      tree->SetBranchStatus("HLT_PARandom_v1",1);
      tree->SetBranchStatus("L1Tech_BPTX_plus_AND_minus.v0",1);
      tree->SetBranchStatus("L1Tech_BPTX_plus_AND_NOT_minus.v0",1);
      tree->SetBranchStatus("L1Tech_BPTX_minus_AND_not_plus.v0",1);

      tree->SetBranchStatus("lumi",1);
      //________________________________________________________________________________

      tree->SetBranchAddress("hf.n",&hf_n);
      tree->SetBranchAddress("hf.e",hf_e);
      tree->SetBranchAddress("hf.depth",hf_depth);
      tree->SetBranchAddress("hf.eta",hf_eta);

      tree->SetBranchAddress("e",cas_e);
      tree->SetBranchAddress("saturation",cas_sat);
      tree->SetBranchAddress("depth",cas_depth);
      tree->SetBranchAddress("iphi",cas_iphi);

      tree->SetBranchAddress("fsc_sum_plus",&fsc_sum_plus);
      tree->SetBranchAddress("fsc_sum_minus",&fsc_sum_minus);

      tree->SetBranchAddress("nTrk",&nTrk);

      tree->SetBranchAddress("HLT_PAZeroBias_v1",&zero_bias);
      tree->SetBranchAddress("HLT_PAL1Tech53_MB_SingleTrack_v1",&min_bias);
      tree->SetBranchAddress("HLT_PARandom_v1",&random);
      tree->SetBranchAddress("L1Tech_BPTX_plus_AND_minus.v0",&bptx_p_m);
      tree->SetBranchAddress("L1Tech_BPTX_plus_AND_NOT_minus.v0",&bptx_p_nm);
      tree->SetBranchAddress("L1Tech_BPTX_minus_AND_not_plus.v0",&bptx_np_m);

      tree->SetBranchAddress("lumi",&lumi);
      //________________________________________________________________________________

      out_file->mkdir(sample_name[sample].c_str());
      out_file->cd(sample_name[sample].c_str());
      string add = sample_name[sample];
      h_zero_count_zb_coll      = new TH1D((add + string("_h_zero_count_zb_coll")).c_str(),"",100,728,1728);
      h_zero_count_zb_no_coll   = new TH1D((add + string("_h_zero_count_zb_no_coll")).c_str(),"",100,728,1728);
      h_hf_hits_coll            = new TH1D((add + string("_h_hf_hits_coll")).c_str(),"",200,0,200);
      h_hf_hits_plus            = new TH1D((add + string("_h_hf+_hits_min")).c_str(),"",200,0,200);
      h_hf_hits_minus           = new TH1D((add + string("_h_hf-_hits_min")).c_str(),"",200,0,200);
      h_hf_hits_noise           = new TH1D((add + string("_h_hf_hits_noise")).c_str(),"",200,0,200);
      h_hf_hits_beam_gas        = new TH1D((add + string("_h_hf_hits_beam_gas")).c_str(),"",200,0,200);
      h_castor_hf_diff_3        = new TH1D((add + string("_h_castor_hf_diff_3")).c_str(),"",100,0,10000);
      h_castor_hf_diff_5        = new TH1D((add + string("_h_castor_hf_diff_5")).c_str(),"",100,0,10000);
      h_castor_hf_diff_10       = new TH1D((add + string("_h_castor_hf_diff_10")).c_str(),"",100,0,10000);
      h_castor_gap_hf           = new TH1D((add + string("_h_castor_gap_hf")).c_str(),"",100,0,50);
      h_castor_nogap_hf         = new TH1D((add + string("_h_castor_nogap_hf")).c_str(),"",100,0,50);
      h_eff                     = new TH1D((add + string("_h_eff")).c_str(),"",9,-0.5,8.5);

      h_hf_cut_single           = new TH1D((add + string("_h_hf_cut_single")).c_str(),"",21,-0.125,5.125);
      h_hf_cut_single_noise     = new TH1D((add + string("_h_hf_cut_single_noise")).c_str(),"",21,-0.125,5.125);
      h_hf_cut_double           = new TH1D((add + string("_h_hf_cut_double")).c_str(),"",21,-0.125,5.125);
      h_hf_cut_double_noise     = new TH1D((add + string("_h_hf_cut_double_noise")).c_str(),"",21,-0.125,5.125);

      h_hf_fsc_p                = new TH2D((add + string("_h_hf_fsc_p")).c_str(),"",200,0,200,200,0,80000);
      h_hf_fsc_m                = new TH2D((add + string("_h_hf_fsc_m")).c_str(),"",200,0,200,200,0,80000);

      h_hf_hits_coll_lumi       = new TProfile((add + string("_h_hf_hits_coll_lumi")).c_str(),"",2000,0,2000);
      h_hf_hits_minus_lumi      = new TProfile((add + string("_h_hf+_hits_lumi")).c_str(),"",2000,0,2000);
      h_hf_hits_plus_lumi       = new TProfile((add + string("_h_hf-_hits_lumi")).c_str(),"",2000,0,2000);
      h_hf_hits_noise_lumi      = new TProfile((add + string("_h_hf_hits_noise_lumi")).c_str(),"",2000,0,2000);
      h_hf_totE_coll_lumi       = new TProfile((add + string("_h_hf_totE_coll_lumi")).c_str(),"",2000,0,2000);
      h_hf_totE_minus_lumi      = new TProfile((add + string("_h_hf+_totE_lumi")).c_str(),"",2000,0,2000);
      h_hf_totE_plus_lumi       = new TProfile((add + string("_h_hf-_totE_lumi")).c_str(),"",2000,0,2000);
      h_hf_totE_noise_lumi      = new TProfile((add + string("_h_hf_totE_noise_lumi")).c_str(),"",2000,0,2000);
      h_lumi                    = new TH1D((add + string("_h_lumi")).c_str(),"",2000,0,2000);
      h_lumi_3GeV               = new TH1D((add + string("_h_lumi_3GeV")).c_str(),"",2000,0,2000);

      h_perf_hf_rechits_single_3gev        = new TH1D((add + string("_h_perf_hf_rechits_single_3gev")).c_str(),"",100,0,1000);
      h_perf_hf_rechits_double_1dot5gev    = new TH1D((add + string("_h_perf_hf_rechits_double_1dot5gev")).c_str(),"",100,0,1000);
      h_perf_hf_totE_single_3gev           = new TH1D((add + string("_h_perf_hf_totE_single_3gev")).c_str(),"",500,0,10000);
      h_perf_hf_totE_double_1dot5gev       = new TH1D((add + string("_h_perf_hf_totE_double_1dot5gev")).c_str(),"",500,0,10000);
      h_perf_hf_totE_eta_single_3gev       = new TH1D((add + string("_h_perf_hf_totE_eta_single_3gev")).c_str(),"",100,-5.2,5.2);
      h_perf_hf_totE_eta_double_1dot5gev   = new TH1D((add + string("_h_perf_hf_totE_eta_double_1dot5gev")).c_str(),"",100,-5.2,5.2);
      
      
      h_eff->GetXaxis()->SetBinLabel(1,"MinBias");
      h_eff->GetXaxis()->SetBinLabel(2,"ZeroBias");
      h_eff->GetXaxis()->SetBinLabel(3,"Noise");
      h_eff->GetXaxis()->SetBinLabel(4,"Beam Gas");
      h_eff->GetXaxis()->SetBinLabel(5,"HF double >2GeV");
      h_eff->GetXaxis()->SetBinLabel(6,"HF double >3GeV");
      h_eff->GetXaxis()->SetBinLabel(7,"HF single >3GeV");
      h_eff->GetXaxis()->SetBinLabel(8,"HF double >2GeV (noise or bg)");
      h_eff->GetXaxis()->SetBinLabel(9,"HF double >3GeV (noise or bg)");


      for(int iEvent=0; iEvent<tree->GetEntries(); iEvent++)
        {
          if(iEvent==MAXEVT) break;
          if(iEvent % 10000 == 0) cout << sample+1 << "/" << sample_name.size() << " -- " << sample_name[sample].c_str() << " -- Entry: " << iEvent << " / " << (MAXEVT>0?MAXEVT:tree->GetEntries()) << endl;
          tree->GetEntry(iEvent);
          
          bool coll=0, noise=0, beam_gas=0;

          coll          = zero_bias && bptx_p_m; //double beam
          beam_gas      = (bptx_np_m || bptx_p_nm); // only single beam
          noise         = !bptx_p_m && !bptx_np_m && !bptx_p_nm; //not both and not single beam

          if(sample_type[sample] == MC)
            {
              beam_gas = 0;
              noise = 0;
              coll = 1;
              min_bias = 1;
            }

          if(!coll && !noise && !beam_gas && !min_bias) //not intersted
            continue;
          

          double sum_cas_e_em = 0;
          double sum_cas_e_had = 0;
          double sum_cas_e = 0;
          for(int ch_cas=0; ch_cas<n_cas_rechits; ch_cas++) // no ZS
            {//break;
              if(cas_depth[ch_cas] < 3)
                sum_cas_e_em += cas_e[ch_cas];
              else if(cas_depth[ch_cas] < 5)
                sum_cas_e_had += cas_e[ch_cas];
            }
          sum_cas_e = sum_cas_e_had + sum_cas_e_em;

          
          int hf_zero_count = n_hf_rechits - hf_n;
          double hf_double_energy_max = -1;
          double hf_single_energy_max = -1;
          double hf_m_energy_max = -1;
          double hf_p_energy_max = -1;
          double hf_p_energy = 0;
          double hf_m_energy = 0;
          for(int ch_hf=0; ch_hf<hf_n; ch_hf++)
            {
              if(hf_eta[ch_hf] > 0. && hf_e[ch_hf] > hf_p_energy_max)
                hf_p_energy_max = hf_e[ch_hf];
              if(hf_eta[ch_hf] <= 0. && hf_e[ch_hf] > hf_m_energy_max)
                hf_m_energy_max = hf_e[ch_hf];

              if(hf_eta[ch_hf] > 0.)
                hf_p_energy += hf_e[ch_hf];
              else
                hf_m_energy += hf_e[ch_hf];
                
            }
          double hf_pm_energy = hf_p_energy + hf_m_energy;

          hf_double_energy_max = TMath::Min(hf_m_energy_max,hf_p_energy_max);
          hf_single_energy_max = TMath::Max(hf_m_energy_max,hf_p_energy_max);
          //Filling HISTOS
          if(coll)                                                  h_zero_count_zb_coll->Fill(hf_zero_count);
          if(noise || beam_gas)                                     h_zero_count_zb_no_coll->Fill(hf_zero_count);

          if(coll)                                                  h_hf_hits_coll->Fill(hf_double_energy_max);
          if(coll)                                                  h_hf_hits_plus->Fill(hf_p_energy_max);
          if(coll)                                                  h_hf_hits_minus->Fill(hf_m_energy_max);
          if(noise)                                                 h_hf_hits_noise->Fill(hf_double_energy_max);
          if(beam_gas)                                              h_hf_hits_beam_gas->Fill(hf_double_energy_max);

          if(coll && hf_double_energy_max < 3)                      h_castor_hf_diff_3->Fill(sum_cas_e);
          if(coll && hf_double_energy_max < 5)                      h_castor_hf_diff_5->Fill(sum_cas_e);
          if(coll && hf_double_energy_max < 10)                     h_castor_hf_diff_10->Fill(sum_cas_e);

          if(coll && sum_cas_e <= 700)                              h_castor_gap_hf->Fill(hf_single_energy_max);
          if(coll && sum_cas_e >  700)                              h_castor_nogap_hf->Fill(hf_single_energy_max);

          if(min_bias)                                              h_eff->Fill(0);
          if(coll)                                                  h_eff->Fill(1);
          if(noise)                                                 h_eff->Fill(2);
          if(beam_gas)                                              h_eff->Fill(3);
          if(coll && hf_double_energy_max > 2)                      h_eff->Fill(4);
          if(coll && hf_double_energy_max > 3)                      h_eff->Fill(5);
          if(coll && hf_single_energy_max > 3)                      h_eff->Fill(6);
          if((noise || beam_gas) && hf_double_energy_max > 2)       h_eff->Fill(7);
          if((noise || beam_gas) && hf_double_energy_max > 3)       h_eff->Fill(8);

          for (double cut=0; cut < 5.5; cut+=0.25)
            {
              if(coll && hf_double_energy_max > cut)                h_hf_cut_double->Fill(cut);
              if((noise || beam_gas) && hf_double_energy_max > cut) h_hf_cut_double_noise->Fill(cut);
              if(coll && hf_single_energy_max > cut)                h_hf_cut_single->Fill(cut);
              if((noise || beam_gas) && hf_single_energy_max > cut) h_hf_cut_single_noise->Fill(cut);
            }

          if(coll)                                                  h_hf_fsc_p->Fill(hf_p_energy_max,fsc_sum_plus);
          if(coll)                                                  h_hf_fsc_m->Fill(hf_m_energy_max,fsc_sum_minus);

          if(coll && hf_double_energy_max > 1.5)                    h_hf_hits_coll_lumi->Fill(lumi,hf_double_energy_max);
          if(coll && hf_double_energy_max > 1.5)                    h_hf_hits_minus_lumi->Fill(lumi,hf_m_energy_max);
          if(coll && hf_double_energy_max > 1.5)                    h_hf_hits_plus_lumi->Fill(lumi,hf_p_energy_max);
          if((noise || beam_gas))                                   h_hf_hits_noise_lumi->Fill(lumi,hf_double_energy_max);
          if(coll && hf_double_energy_max > 1.5)                    h_hf_totE_coll_lumi->Fill(lumi,hf_pm_energy);
          if(coll && hf_double_energy_max > 1.5)                    h_hf_totE_minus_lumi->Fill(lumi,hf_m_energy);
          if(coll && hf_double_energy_max > 1.5)                    h_hf_totE_plus_lumi->Fill(lumi,hf_p_energy);
          if((noise || beam_gas))                                   h_hf_totE_noise_lumi->Fill(lumi,hf_pm_energy);
          if(coll && hf_double_energy_max > 1.5)                    h_lumi_3GeV->Fill(lumi);
          if(coll)                                                  h_lumi->Fill(lumi);

          if(coll && hf_single_energy_max > 3)                      h_perf_hf_rechits_single_3gev->Fill(hf_n);
          if(coll && hf_double_energy_max > 1.5)                    h_perf_hf_rechits_double_1dot5gev->Fill(hf_n);

          if(coll && hf_single_energy_max > 3)                      h_perf_hf_totE_single_3gev->Fill(hf_pm_energy);
          if(coll && hf_double_energy_max > 1.5)                    h_perf_hf_totE_double_1dot5gev->Fill(hf_pm_energy);

          for(int ch_hf=0; ch_hf<hf_n; ch_hf++)
            {
              if(coll && hf_double_energy_max > 1.5)
                h_perf_hf_totE_eta_double_1dot5gev->Fill(hf_eta[ch_hf],hf_e[ch_hf]);
              if(coll && hf_single_energy_max > 3)
                h_perf_hf_totE_eta_single_3gev->Fill(hf_eta[ch_hf],hf_e[ch_hf]);
            }
          
        }
  
      //******************************************AFTER EVENT LOOP*******************************************
      double integ_lumi = 1854.344875;// nb^-1
      double n_total = double(tree->GetEntries());
      double n_zb = h_eff->GetBinContent(1) * (MAXEVT>0?n_total/double(MAXEVT):1.) * 40.;
      double n_mb = h_eff->GetBinContent(4) * (MAXEVT>0?n_total/double(MAXEVT):1.) * 40.;

      cout << endl << "Cross section ZB: " << n_zb/integ_lumi << " --- cross section MB: " << n_mb/integ_lumi << endl;
      h_perf_hf_totE_eta_double_1dot5gev->Scale(1./double(MAXEVT>0?MAXEVT:n_total));
      h_perf_hf_totE_eta_single_3gev->Scale(1./double(MAXEVT>0?MAXEVT:n_total));
  
    }

  //********************************************AFTER FILE LOOP************************************************

  out_file->Write();
  out_file->Save();
}
