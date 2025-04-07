#ifndef __NRF24_DIAGNOSTICS_H__
#define __NRF24_DIAGNOSTICS_H__

#include <stdint.h>
#include "nrf24l01p.h"


typedef struct {
    uint8_t lost_packets;
    uint8_t retransmissions;
    float link_quality_percent;
} NRF24_LinkStatus;

void NRF24_UpdateLinkStatus(void);
NRF24_LinkStatus NRF24_GetLinkStatus(void);
void NRF24_ReportStatus_USB(void);

#endif
