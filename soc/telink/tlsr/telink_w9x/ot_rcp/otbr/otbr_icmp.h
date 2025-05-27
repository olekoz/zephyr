/*
 * Copyright (c) 2025 Telink Semiconductor
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OTBR_ICMP_H
#define OTBR_ICMP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define OTBR_ICMP_CTX_DEFINE(name)                                                                 \
	struct otbr_icmp_ctx name = {                                                              \
		.socket = -1,                                                                      \
	}

struct otbr_icmp_ctx {
	int socket;
	int iface_id;
};

bool otbr_icmp_open(struct otbr_icmp_ctx *ctx, int iface_id);
void otbr_icmp_close(struct otbr_icmp_ctx *ctx);
/* dst_ip - 16 bytes */
bool otbr_icmp_send(struct otbr_icmp_ctx *ctx, const void *dst_ip, const void *data,
		    size_t data_len);
/* src_ip - 16 bytes */
int otbr_icmp_receive(struct otbr_icmp_ctx *ctx, void *src_ip, void *data, size_t data_len);

#endif /* OTBR_ICMP_H */
