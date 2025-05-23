/*
 * Copyright (c) 2025 Telink
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "connections.h"

#include <zephyr/net/openthread.h>
#include <openthread/border_routing.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

void wifi_changed(struct net_if *iface, bool is_connected)
{
	ARG_UNUSED(iface);

	if (is_connected) {
		LOG_INF("** wifi connected");
		otError err = otBorderRoutingInit(openthread_get_default_instance(),
			net_if_get_by_iface(net_if_get_default()), true);

		if (err == OT_ERROR_NONE) {
			err = otBorderRoutingSetEnabled(openthread_get_default_instance(), true);
			if (err == OT_ERROR_NONE) {
				LOG_INF("openthread border router enabled");
			} else {
				LOG_ERR("openthread border router enabling failed %d", err);
			}
		} else {
			LOG_ERR("openthread border router init failed %d", err);
		}
	} else {
		LOG_WRN("** wifi disconnected");
		otError err = otBorderRoutingSetEnabled(openthread_get_default_instance(), false);

		if (err == OT_ERROR_NONE) {
			LOG_INF("openthread border router disabled");
		} else {
			LOG_ERR("openthread border router disabling failed %d", err);
		}
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
