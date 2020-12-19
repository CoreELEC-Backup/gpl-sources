#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <linux/netfilter/nf_tables.h>
#include <libnftnl/flowtable.h>

static int test_ok = 1;

static void print_err(const char *msg)
{
	test_ok = 0;
	printf("\033[31mERROR:\e[0m %s\n", msg);
}

static void cmp_nftnl_flowtable(struct nftnl_flowtable *a, struct nftnl_flowtable *b)
{
	if (strcmp(nftnl_flowtable_get_str(a, NFTNL_FLOWTABLE_NAME),
		   nftnl_flowtable_get_str(b, NFTNL_FLOWTABLE_NAME)) != 0)
		print_err("Flowtable name mismatches");
	if (strcmp(nftnl_flowtable_get_str(a, NFTNL_FLOWTABLE_TABLE),
		   nftnl_flowtable_get_str(b, NFTNL_FLOWTABLE_TABLE)) != 0)
		print_err("Flowtable table mismatches");
	if (nftnl_flowtable_get_u32(a, NFTNL_FLOWTABLE_FAMILY) !=
	    nftnl_flowtable_get_u32(b, NFTNL_FLOWTABLE_FAMILY))
		print_err("Flowtable family mismatches");
	if (nftnl_flowtable_get_u32(a, NFTNL_FLOWTABLE_HOOKNUM) !=
	    nftnl_flowtable_get_u32(b, NFTNL_FLOWTABLE_HOOKNUM))
		print_err("Flowtable hooknum mismatches");
	if (nftnl_flowtable_get_s32(a, NFTNL_FLOWTABLE_PRIO) !=
	    nftnl_flowtable_get_s32(b, NFTNL_FLOWTABLE_PRIO))
		print_err("Flowtable prio mismatches");
	if (nftnl_flowtable_get_u32(a, NFTNL_FLOWTABLE_USE) !=
	    nftnl_flowtable_get_u32(b, NFTNL_FLOWTABLE_USE))
		print_err("Flowtable use mismatches");
	if (nftnl_flowtable_get_u32(a, NFTNL_FLOWTABLE_SIZE) !=
	    nftnl_flowtable_get_u32(b, NFTNL_FLOWTABLE_SIZE))
		print_err("Flowtable size mismatches");
	if (nftnl_flowtable_get_u32(a, NFTNL_FLOWTABLE_FLAGS) !=
	    nftnl_flowtable_get_u32(b, NFTNL_FLOWTABLE_FLAGS))
		print_err("Flowtable flags mismatches");
	if (nftnl_flowtable_get_u64(a, NFTNL_FLOWTABLE_HANDLE) !=
	    nftnl_flowtable_get_u64(b, NFTNL_FLOWTABLE_HANDLE))
		print_err("Flowtable handle mismatches");
}

int main(int argc, char *argv[])
{
	struct nftnl_flowtable *a, *b;
	char buf[4096];
	struct nlmsghdr *nlh;

	a = nftnl_flowtable_alloc();
	b = nftnl_flowtable_alloc();
	if (a == NULL || b == NULL)
		print_err("OOM");

	nftnl_flowtable_set_str(a, NFTNL_FLOWTABLE_NAME, "test");
	nftnl_flowtable_set_u32(a, NFTNL_FLOWTABLE_FAMILY, AF_INET);
	nftnl_flowtable_set_str(a, NFTNL_FLOWTABLE_TABLE, "Table");
	nftnl_flowtable_set_u32(a, NFTNL_FLOWTABLE_HOOKNUM, 0x34567812);
	nftnl_flowtable_set_s32(a, NFTNL_FLOWTABLE_PRIO, 0x56781234);
	nftnl_flowtable_set_u32(a, NFTNL_FLOWTABLE_USE, 0x78123456);
	nftnl_flowtable_set_u32(a, NFTNL_FLOWTABLE_SIZE, 0x89016745);
	nftnl_flowtable_set_u32(a, NFTNL_FLOWTABLE_FLAGS, 0x45016723);
	nftnl_flowtable_set_u64(a, NFTNL_FLOWTABLE_HANDLE, 0x2345016789);

	nlh = nftnl_nlmsg_build_hdr(buf, NFT_MSG_NEWFLOWTABLE, AF_INET,
				    0, 1234);
	nftnl_flowtable_nlmsg_build_payload(nlh, a);

	if (nftnl_flowtable_nlmsg_parse(nlh, b) < 0)
		print_err("parsing problems");

	cmp_nftnl_flowtable(a, b);

	nftnl_flowtable_free(a);
	nftnl_flowtable_free(b);

	if (!test_ok)
		exit(EXIT_FAILURE);

	printf("%s: \033[32mOK\e[0m\n", argv[0]);
	return EXIT_SUCCESS;
}
