/* SPDX-License-Identifier: ISC */
/*
 * Authors: Dan Williams
 *          Martin Lucina
 *          Felipe Huici <felipe.huici@neclab.eu>
 *          Florian Schmidt <florian.schmidt@neclab.eu>
 *          Simon Kuenzer <simon.kuenzer@neclab.eu>
 *
 * Copyright (c) 2015-2017 IBM
 * Copyright (c) 2016-2017 Docker, Inc.
 * Copyright (c) 2017 NEC Europe Ltd., NEC Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <uk/config.h>
#include <kvm-x86/serial_console.h>
#include <x86/cpu.h>

void _libkvmplat_init_serial_console(void)
{
	outb(COM1_INTR, 0x00);  /* Disable all interrupts */
	outb(COM1_CTRL, DLAB);  /* Enable DLAB (set baudrate divisor) */
	outb(COM1_DIV_LO, COM1_BAUDDIV_LO);/* Div (lo byte) */
	outb(COM1_DIV_HI, COM1_BAUDDIV_HI);/*     (hi byte) */
	outb(COM1_CTRL, PROT);  /* Set 8N1, clear DLAB */
}

static int serial_tx_empty(void)
{
	return inb(COM1_STATUS) & 0x20;
}

static void serial_write(char a)
{
	while (!serial_tx_empty())
		;

	outb(COM1_DATA, a);
}

void _libkvmplat_serial_putc(char a)
{
	if (a == '\n')
		serial_write('\r');
	serial_write(a);
}

static int serial_rx_ready(void)
{
	return inb(COM1_STATUS) & 0x01;
}

int _libkvmplat_serial_getc(void)
{
	if (!serial_rx_ready())
		return -1;
	return (int) inb(COM1_DATA);
}
