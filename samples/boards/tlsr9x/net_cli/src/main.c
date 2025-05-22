/*
 * Copyright (c) 2025 Telink
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "connections.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

void wifi_changed(struct net_if *iface, bool is_connected)
{
	ARG_UNUSED(iface);

	if (is_connected) {
		LOG_INF("** wifi connected");
	} else {
		LOG_WRN("** wifi disconnected");
	}
}

void thread_changed(otInstance *instance, otDeviceRole role)
{
	ARG_UNUSED(instance);

	LOG_INF("** Openthread %s", otThreadDeviceRoleToString(role));
}

int main(void)
{
	LOG_INF("***** Network CLI on Zephyr (%s) *****", net_if_get_default()->config.name);

	connections_init(wifi_changed, thread_changed);

	return 0;
}
