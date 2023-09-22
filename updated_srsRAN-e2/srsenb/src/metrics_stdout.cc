/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsenb/hdr/metrics_stdout.h"
#include "srsran/phy/utils/vector.h"
#include "srsenb/hdr/stack/mac/sched_grid.h"
#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_dl_cqi.h"

//#include "srsenb/hdr/stack/mac/scheduler_metric_sliced.h"
//#include "srsenb/hdr/stack/mac/sched_ue_ctrl/sched_dl_cqi.h"
//#include "srsenb/hdr/stack/mac/scheduler_metric.h"

#include <tuple>
#include <float.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <bitset>

using namespace std;

namespace srsenb {

char const* const prefixes[2][9] = {
    {
        "",
        "m",
        "u",
        "n",
        "p",
        "f",
        "a",
        "z",
        "y",
    },
    {
        "",
        "k",
        "M",
        "G",
        "T",
        "P",
        "E",
        "Z",
        "Y",
    },
};

metrics_stdout::metrics_stdout() : do_print(false), n_reports(10), enb(NULL) {}

void metrics_stdout::set_handle(enb_metrics_interface* enb_)
{
  enb = enb_;
}

void metrics_stdout::toggle_print(bool b)
{
  do_print = b;
}

// Define iszero() here since it's not defined in some platforms
static bool iszero(float x)
{
  return fabsf(x) < 2 * DBL_EPSILON;
}

void metrics_stdout::set_metrics_helper(uint32_t                          num_ue,
                                        const mac_metrics_t&              mac,
                                        const std::vector<phy_metrics_t>& phy,
                                        bool                              is_nr)
{
  for (size_t i = 0; i < num_ue; i++) {
    // make sure we have stats for MAC and PHY layer too
    if (i >= mac.ues.size() || ((i >= phy.size()) && !is_nr)) {
      break;
    }
    if (mac.ues[i].tx_errors > mac.ues[i].tx_pkts) {
      fmt::print("tx caution errors {} > {}\n", mac.ues[i].tx_errors, mac.ues[i].tx_pkts);
    }
    if (mac.ues[i].rx_errors > mac.ues[i].rx_pkts) {
      fmt::print("rx caution errors {} > {}\n", mac.ues[i].rx_errors, mac.ues[i].rx_pkts);
    }

    fmt::print("{:>3.5}", (is_nr) ? "nr" : "lte");
    fmt::print("{:>5x}", mac.ues[i].rnti);
    if (not iszero(mac.ues[i].dl_cqi)) {
      fmt::print("  {:>3}", int(mac.ues[i].dl_cqi));
    } else {
      fmt::print("  {:>3.3}", "n/a");
    }
    fmt::print("   {:>1}", int(mac.ues[i].dl_ri));
    float dl_mcs = (is_nr) ? mac.ues[i].dl_mcs : phy[i].dl.mcs;
    if (not isnan(dl_mcs)) {
      fmt::print("   {:>2}", int(dl_mcs));
    } else {
      fmt::print("   {:>2}", 0);
    }
    if (mac.ues[i].tx_brate > 0) {
      fmt::print(" {:>6.6}", float_to_eng_string((float)mac.ues[i].tx_brate / (mac.ues[i].nof_tti * 1e-3), 1));
    } else {
      fmt::print(" {:>6}", 0);
    }
    fmt::print(" {:>4}", mac.ues[i].tx_pkts - mac.ues[i].tx_errors);
    fmt::print(" {:>4}", mac.ues[i].tx_errors);
    if (mac.ues[i].tx_pkts > 0 && mac.ues[i].tx_errors) {
      fmt::print(" {:>3}%", int((float)100 * mac.ues[i].tx_errors / mac.ues[i].tx_pkts));
    } else {
      fmt::print(" {:>3}%", 0);
    }

    fmt::print(" |");

    auto clamp_sinr = [](float sinr) {
      if (sinr > 99.9f) {
        return 99.9f;
      }
      if (sinr < -99.9f) {
        return -99.9f;
      }
      return sinr;
    };
    float pusch_sinr = (is_nr) ? mac.ues[i].pusch_sinr : phy[i].ul.pusch_sinr;
    if (not isnan(pusch_sinr) and not iszero(pusch_sinr)) {
      fmt::print(" {:>5.1f}", clamp_sinr(pusch_sinr));
    } else {
      fmt::print(" {:>5.5}", "n/a");
    }
    float pucch_sinr = (is_nr) ? mac.ues[i].pucch_sinr : phy[i].ul.pucch_sinr;
    if (not isnan(pucch_sinr) and not iszero(pucch_sinr)) {
      fmt::print("  {:>5.1f}", clamp_sinr(pucch_sinr));
    } else {
      fmt::print("  {:>5.5}", "n/a");
    }
    int phr = mac.ues[i].phr;
    if (not isnan(phr)) {
      fmt::print("   {:>2}", phr);
    } else {
      fmt::print("   {:>2}", 0);
    }
    float ul_mcs = (is_nr) ? mac.ues[i].ul_mcs : phy[i].ul.mcs;
    if (not isnan(ul_mcs)) {
      fmt::print("   {:>2}", int(ul_mcs));
    } else {
      fmt::print("   {:>2}", 0);
    }
    if (mac.ues[i].rx_brate > 0) {
      fmt::print(" {:>6.6}", float_to_eng_string((float)mac.ues[i].rx_brate / (mac.ues[i].nof_tti * 1e-3), 1));
    } else {
      fmt::print(" {:>6}", 0);
    }
    fmt::print(" {:>4}", mac.ues[i].rx_pkts - mac.ues[i].rx_errors);
    fmt::print(" {:>4}", mac.ues[i].rx_errors);

    if (mac.ues[i].rx_pkts > 0 && mac.ues[i].rx_errors > 0) {
      fmt::print(" {:>3}%", int((float)100 * mac.ues[i].rx_errors / mac.ues[i].rx_pkts));
    } else {
      fmt::print(" {:>3}%", 0);
    }
    fmt::print(" {:>6.6}", float_to_eng_string(mac.ues[i].ul_buffer, 2));
    fmt::print("\n");
  }
}

void metrics_stdout::set_metrics(const enb_metrics_t& metrics, const uint32_t period_usec)
{
  if (!do_print || enb == nullptr) {
    return;
  }

  if (metrics.rf.rf_error) {
    fmt::print("RF status: O={}, U={}, L={}\n", metrics.rf.rf_o, metrics.rf.rf_u, metrics.rf.rf_l);
  }

  if (metrics.stack.rrc.ues.size() == 0) {
    return;
  }

  if (++n_reports > 10) {
    n_reports = 0;
    fmt::print("\n");
    fmt::print("          -----------------DL----------------|-------------------------UL-------------------------\n");
    fmt::print("rat rnti  cqi  ri  mcs  brate   ok  nok  (%) | pusch  pucch  phr  mcs  brate   ok  nok  (%)    bsr\n");
  }
  //std::cout<<" num of active ues = "<<metrics.stack.rrc.ues.size()<<endl;
  set_metrics_helper(metrics.stack.rrc.ues.size(), metrics.stack.mac, metrics.phy, false);
  set_metrics_helper(metrics.nr_stack.mac.ues.size(), metrics.nr_stack.mac, metrics.phy, true);
}

std::tuple<int32_t, std::vector<int>, std::vector<float>> metrics_stdout::cal_metrics(const enb_metrics_t& metrics)
{
  static int step=0;
  int32_t nof_ues;
  //sf_grid_t enb_tti_sched;
  std::vector<int> dl_cqi_ue;
  std::vector<int> fake_dl_cqi_ue;
  //int dl_cqi_ue[10];
  std::vector<int> subband_cqi;
  int single_ue_cqi;
  int single_band_cqi;
  float single_ue_rate;
  std::vector<float> dr;
  std::vector<float> fake_dr;
  //float dr[10];
  sched_dl_cqi                       enb_dl_cqi(50,1,7);
  //sched_dl_cqi*                       enb_dl_cqi;
  static const uint32_t max_nof_subbands    = 9;
  int32_t fake_nof_ues=3;
  //com_rlt=std::make_pair(a,f);
  if (enb == nullptr) {
    //fmt::print("wrong: eNB does not exist!");
    nof_ues=-1;
  }
  else{
  //std::cout<<" New Method, num of active ues = "<<metrics.stack.rrc.ues.size()<<std::endl;
    step++;

    if (step<20) {
      for (int32_t ue_idx=0;ue_idx<fake_nof_ues;++ue_idx) {
        single_ue_rate = 76534*step*(1+ue_idx*0.1);
        single_ue_cqi = 5+ue_idx;
        fake_dl_cqi_ue.push_back(single_ue_cqi);
        fake_dr.push_back(single_ue_rate);
      }
    }

    else if ((step>20)&&(step<40)) {
    for (int32_t ue_idx=0;ue_idx<fake_nof_ues;++ue_idx) {
      single_ue_rate = 176534*(step-20)*(1+ue_idx*0.1);
      single_ue_cqi = 7+ue_idx;
      fake_dl_cqi_ue.push_back(single_ue_cqi);
      fake_dr.push_back(single_ue_rate);
    }
    }
    else if ((step>40)&&(step<60)) {
      for (int32_t ue_idx=0;ue_idx<fake_nof_ues;++ue_idx) {
      single_ue_rate = 146316.609375*(step-40)*(1+ue_idx*0.1);
      single_ue_cqi = 10+ue_idx;
      fake_dl_cqi_ue.push_back(single_ue_cqi);
      fake_dr.push_back(single_ue_rate);
      }
    }
    else {
      for (int32_t ue_idx=0;ue_idx<fake_nof_ues;++ue_idx) {
        single_ue_rate = 342950.21875*(step-50)*(1+ue_idx*0.1);
        single_ue_cqi = 12+ue_idx;
        fake_dl_cqi_ue.push_back(single_ue_cqi);
        fake_dr.push_back(single_ue_rate);
      }

    }


    nof_ues=metrics.stack.rrc.ues.size();
    for (int32_t ue_idx=0;ue_idx<nof_ues;++ue_idx) {
      single_ue_rate = (float)metrics.nr_stack.mac.ues[ue_idx].rx_brate / (metrics.nr_stack.mac.ues[ue_idx].nof_tti * 1e-3);
      single_ue_cqi = (int)metrics.stack.mac.ues[ue_idx].dl_cqi;
     //std::cout<<"in metrics out, rx rate="<<(float)metrics.stack.mac.ues[ue_idx].nof_tti<<std::endl;
     //std::cout<<"in metrics, data rate is"<<std::endl;
     //fmt::print(" {:>6.6}", float_to_eng_string((float)metrics.stack.mac.ues[ue_idx].tx_brate / (metrics.stack.mac.ues[ue_idx].nof_tti * 1e-3), 1));
      dl_cqi_ue.push_back(single_ue_cqi);
      dr.push_back(single_ue_rate);
    }
    //std::cout<<"in stdout, dl rbg size = "<<dl_mask.to_string(mask_string)<<std::endl;
    //if (enb_dl_cqi.subband_cqi_enabled()) std::cout<<"sub cqi is enabled!"<<std::endl;
    for (uint32_t sb_idx=0;sb_idx<max_nof_subbands;++sb_idx) {
        subband_cqi.push_back(enb_dl_cqi.get_subband_cqi(sb_idx));
        //std::cout<<" subband cqi = "<<subband_cqi[sb_idx];
    }
    std::cout<<std::endl;

  }
  //return std::make_tuple(nof_ues,dl_cqi_ue,dr);
  return std::make_tuple(fake_nof_ues,fake_dl_cqi_ue,fake_dr);
}

/*
training_metrics_t metrics_stdout::training_info_collect(const enb_metrics_t& metrics, const uint32_t period_usec, sf_sched* tti_sched, const sched_dl_cqi& dl_cqi)
{
  training_metrics_t training_input;
  //tti_alloc = tti_sched;
  training_input.nof_ues=metrics.stack.rrc.ues.size();
  training_input.dl_mask=tti_sched->get_dl_mask();
  if (training_input.cqi_rbg.size()>0)
      training_input.cqi_rbg.clear();
  for (uint32_t sb_idx=0;sb_idx<max_nof_subbands;++sb_idx) {
    training_input.cqi_rbg.push_back(dl_cqi.get_subband_cqi(sb_idx));
  }
  if (training_input.dl_datarate.size()>0)
      training_input.dl_datarate.clear();
  for (uint32_t ue_idx=0;ue_idx<metrics.stack.rrc.ues.size();++ue_idx) {
    float signle_ue_rate = metrics.stack.mac.ues[ue_idx].tx_brate / (metrics.stack.mac.ues[ue_idx].nof_tti * 1e-3);
    training_input.dl_datarate.push_back(signle_ue_rate);
  }
  return training_input;
}*/

std::string metrics_stdout::float_to_string(float f, int digits, int field_width)
{
  std::ostringstream os;
  int                precision;
  if (isnan(f) or fabs(f) < 0.0001) {
    f         = 0.0;
    precision = digits - 1;
  } else {
    precision = digits - (int)(log10f(fabs(f + 0.0001)) - 2 * DBL_EPSILON);
  }
  if (precision == -1) {
    precision = 0;
  }
  os << std::setw(field_width) << std::fixed << std::setprecision(precision) << f;
  return os.str();
}

std::string metrics_stdout::float_to_eng_string(float f, int digits)
{
  const int degree = (f == 0.0) ? 0 : lrint(floor(log10f(fabs(f)) / 3));

  std::string factor;

  if (abs(degree) < 9) {
    if (degree < 0)
      factor = prefixes[0][abs(degree)];
    else
      factor = prefixes[1][abs(degree)];
  } else {
    return "failed";
  }

  const double scaled = f * pow(1000.0, -degree);
  if (degree != 0) {
    return float_to_string(scaled, digits, 5) + factor;
  } else {
    return " " + float_to_string(scaled, digits, 5 - factor.length()) + factor;
  }
}

} // namespace srsenb
