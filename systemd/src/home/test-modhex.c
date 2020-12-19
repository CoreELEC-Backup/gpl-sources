/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "modhex.h"
#include "alloc-util.h"
#include "string-util.h"

static void test_normalize_recovery_key(const char *t, const char *expected) {
        _cleanup_free_ char *z = NULL;
        int r;

        assert(t);

        r = normalize_recovery_key(t, &z);
        assert_se(expected ?
                  (r >= 0 && streq(z, expected)) :
                  (r == -EINVAL && z == NULL));
}

int main(int argc, char *arv[]) {

        test_normalize_recovery_key("iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj",
                                    "iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj");

        test_normalize_recovery_key("iefgcelhbiduvkjvcjvuncnkvlfchdidjhtuhhdeurkllkegilkjgbrthjkbgktj",
                                    "iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj");

        test_normalize_recovery_key("IEFGCELH-BIDUVKJV-CJVUNCNK-VLFCHDID-JHTUHHDE-URKLLKEG-ILKJGBRT-HJKBGKTJ",
                                    "iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj");

        test_normalize_recovery_key("IEFGCELHBIDUVKJVCJVUNCNKVLFCHDIDJHTUHHDEURKLLKEGILKJGBRTHJKBGKTJ",
                                    "iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj");

        test_normalize_recovery_key("Iefgcelh-Biduvkjv-Cjvuncnk-Vlfchdid-Jhtuhhde-Urkllkeg-Ilkjgbrt-Hjkbgktj",
                                    "iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj");

        test_normalize_recovery_key("Iefgcelhbiduvkjvcjvuncnkvlfchdidjhtuhhdeurkllkegilkjgbrthjkbgktj",
                                    "iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj");

        test_normalize_recovery_key("iefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgkt", NULL);
        test_normalize_recovery_key("iefgcelhbiduvkjvcjvuncnkvlfchdidjhtuhhdeurkllkegilkjgbrthjkbgkt", NULL);
        test_normalize_recovery_key("IEFGCELHBIDUVKJVCJVUNCNKVLFCHDIDJHTUHHDEURKLLKEGILKJGBRTHJKBGKT", NULL);

        test_normalize_recovery_key("xefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj", NULL);
        test_normalize_recovery_key("Xefgcelh-biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj", NULL);
        test_normalize_recovery_key("iefgcelh+biduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj", NULL);
        test_normalize_recovery_key("iefgcelhebiduvkjv-cjvuncnk-vlfchdid-jhtuhhde-urkllkeg-ilkjgbrt-hjkbgktj", NULL);

        test_normalize_recovery_key("", NULL);

        return 0;
}
