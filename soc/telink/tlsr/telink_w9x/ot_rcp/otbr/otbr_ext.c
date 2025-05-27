/*
 * Copyright (c) 2025 Telink Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <openthread/platform/infra_if.h>
#include <openthread/ip6.h>

#include <zephyr/net/net_ip.h>

#include "otbr_icmp.h"

#define LOG_LEVEL LOG_LEVEL_INF /* CONFIG_OPENTHREAD_LOG_LEVEL */
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(otbr_ext);

static OTBR_ICMP_CTX_DEFINE(icmp_ctx);

bool otbr_ext_start(int iface_id)
{
	return otbr_icmp_open(&icmp_ctx, iface_id);
}

void otbr_ext_stop(void)
{
	otbr_icmp_close(&icmp_ctx);
}

otError otPlatInfraIfSendIcmp6Nd(uint32_t aInfraIfIndex, const otIp6Address *aDestAddress,
				 const uint8_t *aBuffer, uint16_t aBufferLength)
{
	otError result = OT_ERROR_NONE;

	if (!otbr_icmp_send(&icmp_ctx, &aDestAddress->mFields, aBuffer, aBufferLength)) {
		result = OT_ERROR_DROP;
		LOG_ERR("send icmp6 nd failed");
	}

	return result;
}
