/*
 * Copyright (c) 2025 Telink
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "connections.h"

#include <zephyr/init.h>
#include <zephyr/net/net_config.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/openthread.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(connections, LOG_LEVEL_INF);

#if CONFIG_OPENTHREAD_MANUAL_START
#error openthread is not auto started
#endif /* CONFIG_OPENTHREAD_MANUAL_START */
#if !CONFIG_NET_DEFAULT_IF_ETHERNET
#error ethernet is not default interface
#endif /* CONFIG_OPENTHREAD_MANUAL_START */

static connections_wifi_changed connections_wifi_changed_cb = NULL;
static connections_thread_changed connections_thread_changed_cb = NULL;

static void wifi_connect(void)
{
	LOG_INF("wifi connecting to '%s'...", CONFIG_BR_WIFI_SSID);

	for(bool con_requested = false; !con_requested;) {
		struct wifi_connect_req_params connect_req_params = {
			.ssid = CONFIG_BR_WIFI_SSID,
			.ssid_length = strlen(CONFIG_BR_WIFI_SSID),
			.psk = CONFIG_BR_WIFI_PASSWORD,
			.psk_length = strlen(CONFIG_BR_WIFI_PASSWORD),
			.security = WIFI_SECURITY_TYPE_PSK};

		if (net_mgmt(NET_REQUEST_WIFI_CONNECT, net_if_get_default(),
			&connect_req_params, sizeof(connect_req_params))) {
			con_requested = true;
		} else {
			k_msleep(100);
		}
	}
}

static void wifi_connection_changed(struct net_mgmt_event_callback *cb, uint32_t mgmt_event,
	struct net_if *iface)
{
	switch (mgmt_event) {
	case NET_EVENT_WIFI_CONNECT_RESULT: {
		const struct wifi_status *status = (const struct wifi_status *)cb->info;

		if (!status->status) {
			if (connections_wifi_changed_cb) {
				connections_wifi_changed_cb(iface, true);
			}
		} else {
			LOG_ERR("wifi connection error %d", status->status);
		}
		} break;
	case NET_EVENT_WIFI_DISCONNECT_RESULT:
		if (connections_wifi_changed_cb) {
			connections_wifi_changed_cb(iface, false);
		}
		LOG_INF("wifi reconnect");
		wifi_connect();
		break;
	}
}

static void ot_connection_changed(otChangedFlags flags,
	struct openthread_context *ot_context, void *user_data)
{
	ARG_UNUSED(user_data);

	if (flags & OT_CHANGED_THREAD_ROLE) {
		if (connections_thread_changed_cb) {
			connections_thread_changed_cb(ot_context->instance,
				otThreadGetDeviceRole(ot_context->instance));
		}
	}
}

void connections_init(connections_wifi_changed on_wifi, connections_thread_changed on_thread)
{
	static const struct in6_addr icmp_rs_addr = {
		.s6_addr = {
			0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02
		}
	};

	net_if_ipv6_maddr_join(net_if_get_default(),
		net_if_ipv6_maddr_add(net_if_get_default(), &icmp_rs_addr));

	connections_wifi_changed_cb = on_wifi;
	connections_thread_changed_cb = on_thread;

	static struct net_mgmt_event_callback wifi_callback;

	net_mgmt_init_event_callback(&wifi_callback, wifi_connection_changed,
		NET_EVENT_WIFI_CONNECT_RESULT | NET_EVENT_WIFI_DISCONNECT_RESULT);
	net_mgmt_add_event_callback(&wifi_callback);

	static struct openthread_state_changed_cb ot_callback = {
		.state_changed_cb = ot_connection_changed
	};

	openthread_state_changed_cb_register(openthread_get_default_context(), &ot_callback);

	if (net_if_is_wifi(net_if_get_default())) {
		wifi_connect();
	} else {
		LOG_ERR("wifi is not default interface");
	}
}

#if !CONFIG_NET_CONFIG_AUTO_INIT

static int init_application(void)
{
	const char *app_info = "Initializing network";

	STRUCT_SECTION_FOREACH(net_if, iface) {
		if (net_if_is_up(iface)) {
			(void)net_config_init_app(net_if_get_device(iface), app_info);
		} else {
			LOG_INF("interface: %s is down\n", iface->config.name);
		}
	}
	return 0;
}

SYS_INIT(init_application, APPLICATION, CONFIG_NET_CONFIG_INIT_PRIO);

#endif /* !CONFIG_NET_CONFIG_AUTO_INIT */
