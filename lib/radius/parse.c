/*
Copyright (c) 2011, Network RADIUS SARL
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/** \file parse.c
 *  \brief Routines to parse strings into internal data structures
 */

#include <networkradius-devel/client.h>
#include <arpa/inet.h>

ssize_t nr_vp_sscanf_value(VALUE_PAIR *vp, const char *value)
{
	char *end;

	switch (vp->da->type) {
	case NR_TYPE_STRING:
		strlcpy(vp->vp_strvalue, value, sizeof(vp->vp_strvalue));
		vp->length = strlen(vp->vp_strvalue);
		return vp->length;

	case NR_TYPE_DATE:
	case NR_TYPE_INTEGER:
		vp->vp_integer = strtoul(value, &end, 10);
		if ((value == end) || (*end != '\0')) {
			nr_debug_error("Invalid value");
			return -NR_ERR_ATTR_VALUE_MALFORMED;
		}
		return (end - value);

	case NR_TYPE_IPADDR:
		if (inet_pton(AF_INET, value, &vp->vp_ipaddr) < 0) {
			return -NR_ERR_SYSTEM;
		}
		return strlen(value);
		
#ifdef NR_TYPE_IPV6ADDR
	case NR_TYPE_IPV6ADDR:
		if (inet_pton(AF_INET6, value, &vp-vp>ipv6addr) < 0) {
			return -NR_ERR_SYSTEM;
		}
		return strlen(value);
#endif

#ifdef NR_TYPE_IFID
	case NR_TYPE_IFID:
	{
		int i, array[8];

		if (sscanf(value, "%02x%02x%02x%02x%02x%02x%02x%02x",
			   &array[0], &array[1], &array[2], &array[3],
			   &array[4], &array[5], &array[6], &array[7]) != 8) {
			return -NR_ERR_SYSTEM;
		}

		for (i = 0; i < 8; i++) vp->vp_ifid[i] = array[i] & 0xff;

	}
		break;
#endif

	default:
		nr_debug_error("Invalid type");
		return -NR_ERR_ATTR_TYPE_UNKNOWN;
	}

	return 0;
}

int nr_vp_sscanf(const char *string, VALUE_PAIR **pvp)
{
	int rcode;
	const char *p;
	char *q;
	const DICT_ATTR *da;
	VALUE_PAIR *vp;
	char buffer[256];

	if (!string || !pvp) return -NR_ERR_INVALID_ARG;

	p = string;
	q = buffer;
	while (*p && (*p != ' ') && (*p != '=')) {
		*(q++) = *(p++);
	}
	*q = '\0';

	if (q == buffer) {
		nr_debug_error("No Attribute name");
		return -NR_ERR_ATTR_BAD_NAME;
	}

	da = nr_dict_attr_byname(buffer);
	if (!da) {
		nr_debug_error("Unknown attribute \"%s\"", buffer);
		return -NR_ERR_ATTR_UNKNOWN;
	}

	while (*p == ' ') p++;
	if (*p != '=') {
		nr_debug_error("Unexpected text after attribute name");
		return -NR_ERR_ATTR_BAD_NAME;
	}

	p++;
	while (*p == ' ') p++;

	vp = nr_vp_alloc(da);
	if (!vp) return -NR_ERR_NO_MEM;

	rcode = nr_vp_sscanf_value(vp, p);
	if (rcode < 0) {
		nr_vp_free(&vp);
		return rcode;
	}

	*pvp = vp;
	return 0;
}
