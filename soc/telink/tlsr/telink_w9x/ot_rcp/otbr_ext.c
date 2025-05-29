/*
 * Copyright (c) 2025 Telink Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <openthread/platform/infra_if.h>
#include <openthread/ip6.h>

#include <zephyr/net/net_if.h>
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

bool otPlatInfraIfHasAddress(uint32_t aInfraIfIndex, const otIp6Address *aAddress)
{
	bool result = false;
	struct net_if *iface = net_if_get_by_index(aInfraIfIndex);
	struct in6_addr addr;

	memcpy(&addr, aAddress, sizeof(addr));
	if (iface) {
		if (net_if_ipv6_addr_lookup_by_iface(iface, &addr)) {
			result = true;
		}
	}
	return result;
}
