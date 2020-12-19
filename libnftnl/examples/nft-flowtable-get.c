#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <netinet/in.h>

#include <linux/netfilter.h>
#include <linux/netfilter/nf_tables.h>

#include <libmnl/libmnl.h>
#include <libnftnl/flowtable.h>

static int table_cb(const struct nlmsghdr *nlh, void *data)
{
	struct nftnl_flowtable *t;
	char buf[4096];
	uint32_t *type = data;

	t = nftnl_flowtable_alloc();
	if (t == NULL) {
		perror("OOM");
		goto err;
	}

	if (nftnl_flowtable_nlmsg_parse(nlh, t) < 0) {
		perror("nftnl_flowtable_nlmsg_parse");
		goto err_free;
	}

	nftnl_flowtable_snprintf(buf, sizeof(buf), t, *type, 0);
	printf("%s\n", buf);

err_free:
	nftnl_flowtable_free(t);
err:
	return MNL_CB_OK;
}

int main(int argc, char *argv[])
{
	struct mnl_socket *nl;
	char buf[MNL_SOCKET_BUFFER_SIZE];
	struct nlmsghdr *nlh;
	uint32_t portid, seq, type = NFTNL_OUTPUT_DEFAULT;
	struct nftnl_flowtable *t = NULL;
	int ret, family;

	seq = time(NULL);

	if (argc < 2 || argc > 5) {
		fprintf(stderr, "Usage: %s <family> [<table> <flowtable>]\n",
			argv[0]);
		exit(EXIT_FAILURE);
	}

	if (strcmp(argv[1], "ip") == 0)
		family = NFPROTO_IPV4;
	else if (strcmp(argv[1], "ip6") == 0)
		family = NFPROTO_IPV6;
	else if (strcmp(argv[1], "bridge") == 0)
		family = NFPROTO_BRIDGE;
	else if (strcmp(argv[1], "arp") == 0)
		family = NFPROTO_ARP;
	else if (strcmp(argv[1], "unspec") == 0)
		family = NFPROTO_UNSPEC;
	else {
		fprintf(stderr, "Unknown family: ip, ip6, bridge, arp, unspec\n");
		exit(EXIT_FAILURE);
	}

	if (argc >= 4) {
		t = nftnl_flowtable_alloc();
		if (t == NULL) {
			perror("OOM");
			exit(EXIT_FAILURE);
		}
		nlh = nftnl_flowtable_nlmsg_build_hdr(buf, NFT_MSG_GETFLOWTABLE, family,
						NLM_F_ACK, seq);
		nftnl_flowtable_set(t, NFTNL_FLOWTABLE_TABLE, argv[2]);
		nftnl_flowtable_set(t, NFTNL_FLOWTABLE_NAME, argv[3]);
		nftnl_flowtable_nlmsg_build_payload(nlh, t);
		nftnl_flowtable_free(t);
	} else if (argc >= 2) {
		nlh = nftnl_flowtable_nlmsg_build_hdr(buf, NFT_MSG_GETFLOWTABLE, family,
						NLM_F_DUMP, seq);
	}

	nl = mnl_socket_open(NETLINK_NETFILTER);
	if (nl == NULL) {
		perror("mnl_socket_open");
		exit(EXIT_FAILURE);
	}

	if (mnl_socket_bind(nl, 0, MNL_SOCKET_AUTOPID) < 0) {
		perror("mnl_socket_bind");
		exit(EXIT_FAILURE);
	}
	portid = mnl_socket_get_portid(nl);

	if (mnl_socket_sendto(nl, nlh, nlh->nlmsg_len) < 0) {
		perror("mnl_socket_send");
		exit(EXIT_FAILURE);
	}

	ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	while (ret > 0) {
		ret = mnl_cb_run(buf, ret, seq, portid, table_cb, &type);
		if (ret <= 0)
			break;
		ret = mnl_socket_recvfrom(nl, buf, sizeof(buf));
	}
	if (ret == -1) {
		perror("error");
		exit(EXIT_FAILURE);
	}
	mnl_socket_close(nl);

	return EXIT_SUCCESS;
}
