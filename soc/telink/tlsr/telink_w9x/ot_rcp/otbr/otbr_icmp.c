/*
 * Copyright (c) 2025 Telink Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "otbr_icmp.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define LOG_LEVEL LOG_LEVEL_INF /* CONFIG_OPENTHREAD_LOG_LEVEL */
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(otbr_icmp);

bool otbr_icmp_open(struct otbr_icmp_ctx *ctx, int iface_id)
{
	bool result = false;

	do {
		otbr_icmp_close(ctx);
		ctx->socket = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
		if (ctx->socket < 0) {
			break;
		}

		int hop_limit = 255;
		if (setsockopt(ctx->socket, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hop_limit,
			       sizeof(hop_limit)) < 0) {
			otbr_icmp_close(ctx);
			break;
		}
		if (setsockopt(ctx->socket, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &hop_limit,
			       sizeof(hop_limit)) < 0) {
			otbr_icmp_close(ctx);
			break;
		}

		ctx->iface_id = iface_id;
		result = true;
	} while (0);
	return result;
}

void otbr_icmp_close(struct otbr_icmp_ctx *ctx)
{
	if (ctx->socket >= 0) {
		close(ctx->socket);
		ctx->socket = -1;
	}
}

bool otbr_icmp_send(struct otbr_icmp_ctx *ctx, const void *dst_ip, const void *data,
		    size_t data_len)
{
	bool result = false;

	if (ctx->socket >= 0) {
		struct sockaddr_in6 dst_addr = {.sin6_family = PF_INET6,
						.sin6_scope_id = ctx->iface_id};

		memcpy(&dst_addr.sin6_addr, dst_ip, sizeof(dst_addr.sin6_addr));

		LOG_INF("send to %s, scope: %u, sock %u", inet_ntop(AF_INET6, &dst_addr.sin6_addr,
			(char[INET6_ADDRSTRLEN]) {}, INET6_ADDRSTRLEN),
			dst_addr.sin6_scope_id, ctx->socket);
		LOG_HEXDUMP_INF(data, data_len, "icmp data");

		if (sendto(ctx->socket, data, data_len, 0, (struct sockaddr *)&dst_addr,
			   sizeof(dst_addr)) == data_len) {
			result = true;
		}
	}
	return result;
}

int otbr_icmp_receive(struct otbr_icmp_ctx *ctx, void *src_ip, void *data, size_t data_len)
{
	int result = -1;

	if (ctx->socket >= 0) {
		struct sockaddr_in6 src_addr;
		socklen_t src_len = sizeof(src_addr);

		result = recvfrom(ctx->socket, data, data_len, 0, (struct sockaddr *)&src_addr,
				  &src_len);
		if (result >= 0) {
			memcpy(src_ip, &src_addr.sin6_addr, sizeof(src_addr.sin6_addr));
		}
	}
	return result;
}
