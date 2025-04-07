#include "nrf24l01p.h"
#include "usbd_cdc_if.h"
#include "NRF24_Diagnostics.h"
#include <stdio.h>

static NRF24_LinkStatus linkStatus;

static uint8_t last_observe = 0xFF;
static uint8_t no_change_count = 0;
#define MAX_NO_CHANGE 15

void NRF24_UpdateLinkStatus(void) {
    uint8_t observe = nrf24_ReadReg(OBSERVE_TX);

    if (observe == last_observe) {
        no_change_count++;
        if (no_change_count >= MAX_NO_CHANGE) {
            return;
        }
    } else {
        no_change_count = 0;
        last_observe = observe;
    }

    linkStatus.retransmissions = observe & 0x0F;
    linkStatus.lost_packets = (observe >> 4) & 0x0F;

    uint8_t total = linkStatus.retransmissions + linkStatus.lost_packets;
    if (total == 0) {
        linkStatus.link_quality_percent = 100.0f;
    } else {
        float quality = 100.0f * (1.0f - ((float)linkStatus.retransmissions / total));
        if (quality >= 0.0f && quality <= 100.0f) {
            linkStatus.link_quality_percent = quality;
        }
    }
}

NRF24_LinkStatus NRF24_GetLinkStatus(void) {
    return linkStatus;
}

void NRF24_ReportStatus_USB(void) {
    NRF24_UpdateLinkStatus();

    char msg[64];
    if (no_change_count >= MAX_NO_CHANGE) {
        snprintf(msg, sizeof(msg), "SEM SINAL ou dados obsoletos\r\n");
    } else {
        snprintf(msg, sizeof(msg),
                 "Lost: %d, Retries: %d, Link: %.1f%%\r\n",
                 linkStatus.lost_packets,
                 linkStatus.retransmissions,
                 linkStatus.link_quality_percent);
    }

    CDC_Transmit_FS((uint8_t *)msg, strlen(msg));
}
