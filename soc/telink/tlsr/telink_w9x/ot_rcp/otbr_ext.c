/*
 * Copyright (c) 2025 Telink Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <openthread/platform/infra_if.h>
#include <openthread/ip6.h>

#include <zephyr/net/net_ip.h>

#define LOG_LEVEL LOG_LEVEL_INF /* CONFIG_OPENTHREAD_LOG_LEVEL */
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(otbr_ext);

otError otPlatInfraIfSendIcmp6Nd(uint32_t aInfraIfIndex, const otIp6Address *aDestAddress,
	const uint8_t *aBuffer, uint16_t aBufferLength)
{
	char ipv6_str[INET6_ADDRSTRLEN];

	otIp6AddressToString(aDestAddress, ipv6_str, sizeof(ipv6_str));

	LOG_INF("ICMPv6 ND using %u to %s", aInfraIfIndex, ipv6_str);
	LOG_HEXDUMP_INF(aBuffer, aBufferLength, "ND data");

	return OT_ERROR_NONE;
}
