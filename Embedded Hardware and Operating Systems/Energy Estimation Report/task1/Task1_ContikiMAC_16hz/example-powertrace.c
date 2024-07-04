/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Demonstrating the powertrace application with broadcasts
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include "sys/energest.h"

#include "powertrace.h"

#include "dev/button-sensor.h"

#include "dev/leds.h"

#include <stdio.h>

static unsigned long rx_start_duration;

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "BROADCAST example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/


/* Voltage and current stats from tmote-sky-datasheet
   Supply voltage: min 2.1V, max 3.6V; avg = 2.85V
   Current during radio transmission: nominal 19.5 mA
   Current during radio receiving: nominal 21.8 mA
   Current during sleep: nominal 5.1 nanoA
*/
static const unsigned long voltage = 3;		// 2.85 rounded	
static const unsigned long itx = 19500;		// Current in nanoA
static const unsigned long irx = 21800;
static const unsigned long ilpm = 5;
static const unsigned long icpu = 500;
static unsigned long rx_start_duration = 0;
unsigned long cpu, lpm, transmit, listen, time, radio;
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  // Initialize Energest
  energest_init();

  broadcast_open(&broadcast, 129, &broadcast_call);

  while (1)
  {

    /* Delay 2-4 seconds */
    etimer_set(&et, (CLOCK_SECOND) * 4 + random_rand() % ((CLOCK_SECOND) * 4));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    packetbuf_copyfrom("Hello", 6);
    broadcast_send(&broadcast);
    printf("Broadcast message sent\n");

 // Wait for the broadcast to be sent

// Wait for a short time assuming the broadcast transmission is completed
    etimer_set(&et, (CLOCK_SECOND) / 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	
    // Measure energy after radio transmission
    energest_flush();
    cpu = energest_type_time(ENERGEST_TYPE_CPU)/RTIMER_SECOND;
    lpm = energest_type_time(ENERGEST_TYPE_LPM)/RTIMER_SECOND;
    transmit = energest_type_time(ENERGEST_TYPE_TRANSMIT)/RTIMER_SECOND ;
    listen = energest_type_time(ENERGEST_TYPE_LISTEN)/RTIMER_SECOND  ;
    time = cpu + lpm;
    radio = transmit + listen;



    // Print or use the energy consumption values
    printf("CPU time spent in sec: %lu, lpm time spent in sec: %lu\n", cpu, lpm);
    unsigned long e_cpu = cpu*icpu*voltage;
    unsigned long e_lpm = lpm*ilpm*voltage;
    unsigned long e_cycle = e_cpu + e_lpm;
    unsigned long e_radio = (transmit * itx + listen *irx) * voltage ;
    printf("Energy consumption: %lu 10^-9 J; including cpu consuption: %lu 10^-9 J and lpm consumption: %lu 10^-9 J \n", e_cycle, e_cpu, e_lpm);
    printf("Radio Energy consumption: %lu 10^-9 J\n",e_radio);

  
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
