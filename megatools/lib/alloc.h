#ifndef __ALLOC_H__
#define __ALLOC_H__

#include <gio/gio.h>

G_BEGIN_DECLS

#define DEFINE_CLEANUP_FUNCTION_FULL(Type, func, null_safe, args...)                                                   \
	static inline void __cleanup_##func(void *v)                                                                   \
	{                                                                                                              \
		if (null_safe || *(Type *)v)                                                                           \
			func(*(Type *)v, ##args);                                                                      \
	}

#define DEFINE_CLEANUP_FUNCTION(Type, func) DEFINE_CLEANUP_FUNCTION_FULL(Type, func, TRUE)

#define DEFINE_CLEANUP_FUNCTION_NULL(Type, func) DEFINE_CLEANUP_FUNCTION_FULL(Type, func, FALSE)

#define CLEANUP(func) __attribute__((cleanup(__cleanup_##func)))

DEFINE_CLEANUP_FUNCTION(void *, g_free)
#define gc_free CLEANUP(g_free)

DEFINE_CLEANUP_FUNCTION(char **, g_strfreev)
#define gc_strfreev CLEANUP(g_strfreev)

DEFINE_CLEANUP_FUNCTION_NULL(GError *, g_error_free)
#define gc_error_free CLEANUP(g_error_free)

DEFINE_CLEANUP_FUNCTION_NULL(GArray *, g_array_unref)
#define gc_array_unref CLEANUP(g_array_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GPtrArray *, g_ptr_array_unref)
#define gc_ptr_array_unref CLEANUP(g_ptr_array_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GByteArray *, g_byte_array_unref)
#define gc_byte_array_unref CLEANUP(g_byte_array_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GHashTable *, g_hash_table_unref)
#define gc_hash_table_unref CLEANUP(g_hash_table_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GVariant *, g_variant_unref)
#define gc_variant_unref CLEANUP(g_variant_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GVariantIter *, g_variant_iter_free)
#define gc_variant_iter_free CLEANUP(g_variant_iter_free)

DEFINE_CLEANUP_FUNCTION_NULL(GVariantBuilder *, g_variant_builder_unref)
#define gc_variant_builder_unref CLEANUP(g_variant_builder_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GBytes *, g_bytes_unref)
#define gc_bytes_unref CLEANUP(g_bytes_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GRegex *, g_regex_unref)
#define gc_regex_unref CLEANUP(g_regex_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GMatchInfo *, g_match_info_unref)
#define gc_match_info_unref CLEANUP(g_match_info_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GKeyFile *, g_key_file_unref)
#define gc_key_file_unref CLEANUP(g_key_file_unref)

DEFINE_CLEANUP_FUNCTION_NULL(GChecksum *, g_checksum_free)
#define gc_checksum_free CLEANUP(g_checksum_free)

DEFINE_CLEANUP_FUNCTION_NULL(GObject *, g_object_unref)
#define gc_object_unref CLEANUP(g_object_unref)

DEFINE_CLEANUP_FUNCTION_FULL(GString *, g_string_free, FALSE, TRUE)
#define gc_string_free CLEANUP(g_string_free)

G_END_DECLS

#endif
