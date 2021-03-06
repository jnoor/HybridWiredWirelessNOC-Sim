/*
    Copyright (C) 1999-2008 by Mark D. Hill and David A. Wood for the
    Wisconsin Multifacet Project.  Contact: gems@cs.wisc.edu
    http://www.cs.wisc.edu/gems/

    --------------------------------------------------------------------

    This file is part of Orion (Princeton's interconnect power model),
    a component of the Multifacet GEMS (General Execution-driven 
    Multiprocessor Simulator) software toolset originally developed at 
    the University of Wisconsin-Madison.

    Garnet was developed by Niket Agarwal at Princeton University. Orion was
    developed by Princeton University.

    Substantial further development of Multifacet GEMS at the
    University of Wisconsin was performed by Alaa Alameldeen, Brad
    Beckmann, Jayaram Bobba, Ross Dickson, Dan Gibson, Pacia Harper,
    Derek Hower, Milo Martin, Michael Marty, Carl Mauer, Michelle Moravan,
    Kevin Moore, Andrew Phelps, Manoj Plakal, Daniel Sorin, Haris Volos, 
    Min Xu, and Luke Yen.
    --------------------------------------------------------------------

    If your use of this software contributes to a published paper, we
    request that you (1) cite our summary paper that appears on our
    website (http://www.cs.wisc.edu/gems/) and (2) e-mail a citation
    for your published paper to gems@cs.wisc.edu.

    If you redistribute derivatives of this software, we request that
    you notify us and either (1) ask people to register with us at our
    website (http://www.cs.wisc.edu/gems/) or (2) collect registration
    information and periodically send it to us.

    --------------------------------------------------------------------

    Multifacet GEMS is free software; you can redistribute it and/or
    modify it under the terms of version 2 of the GNU General Public
    License as published by the Free Software Foundation.

    Multifacet GEMS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the Multifacet GEMS; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307, USA

    The GNU General Public License is contained in the file LICENSE.

### END HEADER ###
*/
#include <stdio.h>
#include <math.h>

#include "power_router_init.h"
#include "power_array.h"
#include "power_crossbar.h"
#include "power_arbiter.h"
#include "power_bus.h"
#include "NetworkPower.h"
#include "Router_d.h"
#include "NetworkLink_d.h"
#include "GarnetNetwork_d.h"
#include "SIM_port.h"
#include "parm_technology.h"

/* --------- Static energy calculation functions ------------ */

//Input buffer
double SIM_reg_stat_energy(power_array_info *info, power_array *arr, double n_read, double n_write)
{
  double Eavg = 0, Eatomic, Estruct, Estatic;


  /* decoder */
  if (info->row_dec_model) {
    //row decoder
	Estruct = 0;
    /* assume switch probability 0.5 for address bits */
    //input
	Eatomic = arr->row_dec.e_chg_addr * arr->row_dec.n_bits * SWITCHING_FACTOR * (n_read + n_write);
    Estruct += Eatomic;
    
    //output
	Eatomic = arr->row_dec.e_chg_output * (n_read + n_write);
    Estruct += Eatomic;

    /* assume all 1st-level decoders change output */
	//internal node
    Eatomic = arr->row_dec.e_chg_l1 * arr->row_dec.n_in_2nd * (n_read + n_write);
    Estruct += Eatomic;

    Eavg += Estruct;
  }

  /* wordline */
  Estruct = 0;
  //read 
  Eatomic = arr->data_wordline.e_read * n_read;
  Estruct += Eatomic;
  //write
  Eatomic = arr->data_wordline.e_write * n_write;
  Estruct += Eatomic;

  Eavg += Estruct;

  /* bitlines */
  Estruct = 0;
  //read  
  if (arr->data_bitline.end == 2) {
    Eatomic = arr->data_bitline.e_col_read * info->eff_data_cols * n_read;
		}
  else {
    /* assume switch probability 0.5 for single-ended bitlines */
    Eatomic = arr->data_bitline.e_col_read * info->eff_data_cols * SWITCHING_FACTOR * n_read;
		}
  
  Estruct += Eatomic;
  //write
  /* assume switch probability 0.5 for write bitlines */
  Eatomic = arr->data_bitline.e_col_write * info->data_width * SWITCHING_FACTOR * n_write;
  Estruct += Eatomic;
  //precharge
  Eatomic = arr->data_bitline_pre.e_charge * info->eff_data_cols * n_read;
  Estruct += Eatomic;

  Eavg += Estruct;

  /* memory cells */
  Estruct = 0;
    
  /* assume switch probability 0.5 for memory cells */
  Eatomic = arr->data_mem.e_switch * info->data_width * SWITCHING_FACTOR * n_write;
  Estruct += Eatomic;

  Eavg += Estruct;

  /* sense amplifier */
  if (info->data_end == 2) {
    Estruct = 0;

    Eatomic = arr->data_amp.e_access * info->eff_data_cols * n_read;
    Estruct += Eatomic;

    Eavg += Estruct;
  }

  /* output driver */
  if (info->outdrv_model) {
    Estruct = 0;
    //enable
    Eatomic = arr->outdrv.e_select * n_read;
    Estruct += Eatomic;
    //data
    /* same switch probability as bitlines */
    Eatomic = arr->outdrv.e_chg_data * arr->outdrv.item_width * SWITCHING_FACTOR * info->n_item * info->assoc * n_read;
    Estruct += Eatomic;
    //output 1
    /* assume 1 and 0 are uniformly distributed */
    if (arr->outdrv.e_out_1 >= arr->outdrv.e_out_0 ) {
      Eatomic = arr->outdrv.e_out_1 * arr->outdrv.item_width * SWITCHING_FACTOR * n_read;
      Estruct += Eatomic;
    }
    //output 0
    if (arr->outdrv.e_out_1 < arr->outdrv.e_out_0) {
      Eatomic = arr->outdrv.e_out_0 * arr->outdrv.item_width * SWITCHING_FACTOR * n_read;
      Estruct += Eatomic;
    }

    Eavg += Estruct;
  }

  /* static power */
  // NOT CALCULATED FOR NOW... -Adam
  // Estatic = arr->i_leakage * Vdd * Period * SCALE_S;
  //
  //static energy
  // Eavg += Estatic;
  
  return Eavg;
}

//crossbar
double SIM_crossbar_stat_energy(power_crossbar *crsbar, double n_data)
{
  double Eavg = 0, Eatomic, Estatic;
  
  if (n_data > crsbar->n_out) {
    n_data = crsbar->n_out;
  }


  switch (crsbar->model) {
    case MATRIX_CROSSBAR:
    case CUT_THRU_CROSSBAR:
    case MULTREE_CROSSBAR:
	 /* assume 0.5 data switch probability */
        //input 
		Eatomic = crsbar->e_chg_in * crsbar->data_width * SWITCHING_FACTOR * n_data;
	 	Eavg += Eatomic;

		 //output
         Eatomic = crsbar->e_chg_out * crsbar->data_width * SWITCHING_FACTOR * n_data;
	 	 Eavg += Eatomic;
		
		 //control
         Eatomic = crsbar->e_chg_ctr * n_data;
	     Eavg += Eatomic;

	 if (crsbar->model == MULTREE_CROSSBAR && crsbar->depth > 1) {
	     //internal node
		 Eatomic = crsbar->e_chg_int * crsbar->data_width * (crsbar->depth - 1) * SWITCHING_FACTOR * n_data;
	     Eavg += Eatomic;
	 }
	 break;

    default:	break;/* some error handler */
  }

  return Eavg;
}

//arbiter
/* stat over one cycle */
/* info is only used by queuing arbiter */
double SIM_arbiter_stat_energy(power_arbiter *arb, power_array_info *info, double n_req)
{
  double Eavg = 0, Estruct, Eatomic;
  double total_pri, n_chg_pri, n_grant;

  /* energy cycle distribution */
  if (n_req > arb->req_width) {
	n_req = arb->req_width;
  }
  if (n_req >= 1) n_grant = 1;
  else n_grant = 1.0 / ceil(1.0 / n_req);

  switch (arb->model) {
    case RR_ARBITER:
	 /* FIXME: we may overestimate request switch */
	 //request
	 Eatomic = arb->e_chg_req * n_req;
	 Eavg += Eatomic;

	 //grant
	 Eatomic = arb->e_chg_grant * n_grant;
	 Eavg += Eatomic;

	 /* assume carry signal propagates half length in average case */
	 /* carry does not propagate in maximum case, i.e. all carrys go down */
	 //carry
	 Eatomic = arb->e_chg_carry * arb->req_width * SWITCHING_FACTOR * n_grant;
	 Eavg += Eatomic;

	 //internal carry
	 Eatomic = arb->e_chg_carry_in * (arb->req_width * SWITCHING_FACTOR - 1) * n_grant;
	 Eavg += Eatomic;

	 /* priority registers */
	 Estruct = 0;
	 //priority
	 
	 //switch
	 Eatomic = arb->pri_ff.e_switch * 2 * n_grant;
	 Estruct += Eatomic;

	 //keep 0
	 Eatomic = arb->pri_ff.e_keep_0 * (arb->req_width - 2 * n_grant);
	 Estruct += Eatomic;

	 //clock
	 Eatomic = arb->pri_ff.e_clock * arb->req_width;
	 Estruct += Eatomic;

	 Eavg += Estruct;
	 break;

    case MATRIX_ARBITER:
	 total_pri = arb->req_width * (arb->req_width - 1) * 0.5;
	 /* assume switch probability 0.5 for priorities */
	 n_chg_pri = (arb->req_width - 1) * SWITCHING_FACTOR;

	 /* FIXME: we may overestimate request switch */
	 //request
	 Eatomic = arb->e_chg_req * n_req;
	 Eavg += Eatomic;

	 //grant
	 Eatomic = arb->e_chg_grant * n_grant;
	 Eavg += Eatomic;

	 /* priority registers */
	 Estruct = 0;
	 //priority
	
	 //switch 
	 Eatomic = arb->pri_ff.e_switch * n_chg_pri * n_grant;
	 Estruct += Eatomic;

	 /* assume 1 and 0 are uniformly distributed */
	 //keep 0
	 if (arb->pri_ff.e_keep_0 >= arb->pri_ff.e_keep_1) {
	   Eatomic = arb->pri_ff.e_keep_0 * (total_pri - n_chg_pri * n_grant) * SWITCHING_FACTOR;
	   Estruct += Eatomic;
	 }

	 //keep 1
	 if (arb->pri_ff.e_keep_0 < arb->pri_ff.e_keep_1) {
	   Eatomic = arb->pri_ff.e_keep_1 * (total_pri - n_chg_pri * n_grant) * SWITCHING_FACTOR;
	   Estruct += Eatomic;
	 }

	 //clock
	 Eatomic = arb->pri_ff.e_clock * total_pri;
	 Estruct += Eatomic;

	 Eavg += Estruct;

	 /* based on above assumptions */
	 //internal node  
	 /* p(n-1)/2 + (n-1)/2 */
	   Eatomic = arb->e_chg_mint * (n_req + 1) * (arb->req_width - 1) * 0.5;
	 Eavg += Eatomic;
	 break;

    case QUEUE_ARBITER:
	 /* FIXME: what if n_req > 1? */
	 Eavg = SIM_reg_stat_energy(info, &arb->queue, n_req, n_grant);
	 break;
	 
    default:	break;/* some error handler */
  }


  return Eavg;
}

double SIM_bus_stat_energy(power_bus *bus, double e_link)
{
  double Ebus;
  Ebus = bus->e_switch * e_link * SWITCHING_FACTOR * bus->bit_width;
  
  return (Ebus);
}  

double Router_d::calculate_offline_power(power_router *router, power_router_info *info)
{
	double Eavg = 0;
	double P_in_buf, P_xbar, P_vc_in_arb, P_vc_out_arb, P_sw_in_arb, P_sw_out_arb, P_leakage, P_total;

	double E_in_buf, E_xbar, E_vc_in_arb, E_vc_out_arb, E_sw_in_arb, E_sw_out_arb, E_leakage;  
	double e_in_buf_read, e_in_buf_write, e_crossbar, e_vc_local_arb, e_vc_global_arb, e_sw_local_arb, e_sw_global_arb;
	double sim_cycles;

	sim_cycles = g_eventQueue_ptr->getTime() - m_network_ptr->getRubyStartTime();

	calculate_performance_numbers();
	//counts obtained from perf. simulator 
	e_in_buf_read = (double )(buf_read_count/sim_cycles);
	e_in_buf_write = (double )(buf_write_count/sim_cycles);
	e_crossbar = (double )(crossbar_count/sim_cycles);  
	e_vc_local_arb = (double)(vc_local_arbit_count/sim_cycles);
	e_vc_global_arb = (double)(vc_global_arbit_count/sim_cycles);
	e_sw_local_arb = (double )(sw_local_arbit_count/sim_cycles);
	e_sw_global_arb = (double )(sw_global_arbit_count/sim_cycles);
	//          e_link = (double )(link_traversal_count/sim_cycles);

	/* input buffers */
	if (info->in_buf) 
		E_in_buf = SIM_reg_stat_energy(&info->in_buf_info, &router->in_buf, e_in_buf_read, e_in_buf_write);
	P_in_buf = E_in_buf * PARM_Freq;
	Eavg += E_in_buf;

	/* main crossbar */
	if (info->crossbar_model) 
		E_xbar= SIM_crossbar_stat_energy(&router->crossbar, e_crossbar);
	P_xbar = E_xbar * PARM_Freq; 
	Eavg += E_xbar;

	/* vc input (local) arbiter */
	if (info->vc_in_arb_model) 
		E_vc_in_arb = SIM_arbiter_stat_energy(&router->vc_in_arb, &info->vc_in_arb_queue_info, e_sw_local_arb);
	P_vc_in_arb = E_vc_in_arb * PARM_Freq;
	Eavg += E_vc_in_arb;

	/* vc output (global) arbiter */
	if (info->vc_out_arb_model)
		E_vc_out_arb = SIM_arbiter_stat_energy(&router->vc_out_arb, &info->vc_out_arb_queue_info, e_sw_global_arb); 
	P_vc_out_arb = E_vc_out_arb * PARM_Freq;
	Eavg += E_vc_out_arb;

	/* sw input (local) arbiter */
	if (info->sw_in_arb_model) 
		E_sw_in_arb = SIM_arbiter_stat_energy(&router->sw_in_arb, &info->sw_in_arb_queue_info, e_sw_local_arb);
	P_sw_in_arb = E_sw_in_arb * PARM_Freq;
	Eavg += E_sw_in_arb;

	/* sw output (global) arbiter */
	if (info->sw_out_arb_model)
		E_sw_out_arb = SIM_arbiter_stat_energy(&router->sw_out_arb, &info->sw_out_arb_queue_info, e_sw_global_arb); 
	P_sw_out_arb = E_sw_out_arb * PARM_Freq;
	Eavg += E_sw_out_arb;
	  	
	/* static power */
	// NOT CALCULATED FOR NOW... -Adam
	// E_leakage = router->i_leakage * Vdd * Period * SCALE_S;
	// P_leakage = E_leakage * PARM_Freq;
	// Eavg += E_leakage;

	P_total = Eavg * PARM_Freq;

	return Eavg;
}	

double NetworkLink_d::calculate_offline_power(power_bus* bus)
{	
	double sim_cycles = (double) (g_eventQueue_ptr->getTime() - m_net_ptr->getRubyStartTime());
	double e_link = (double) (m_link_utilized)/ sim_cycles;
	double E_link = SIM_bus_stat_energy(bus, e_link);
	double P_link = E_link * PARM_Freq;
	return P_link;
}

double NetworkLink_d::calculate_power()
{//printf("start:%d\n",m_net_ptr->getRubyStartTime());
	power_bus bus;
	power_bus_init(&bus, GENERIC_BUS, IDENT_ENC, FLITBITLENGTH * m_flit_capacity, 0, 1, 1, PARM_link_length, 0);
	double total_power = calculate_offline_power(&bus);
	return total_power;
}

void Router_d::power_router_initialize(power_router *router, power_router_info *info)
{
	info->n_in = m_input_unit.size();
	info->n_out = m_output_unit.size();
	info->flit_width = PARM_flit_width;
	
	info->n_v_channel = m_num_vcs;
	info->n_v_class = m_virtual_networks;

}

double Router_d::calculate_power()
{
	power_router router;
	power_router_info router_info;
	double total_energy, total_power;

	power_router_initialize(&router, &router_info);
	power_router_init(&router, &router_info);

	total_energy = calculate_offline_power(&router, &router_info);
	total_power = total_energy * PARM_Freq;
	return total_power;
}
