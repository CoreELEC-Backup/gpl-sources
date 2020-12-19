/* Regression test for dbus-glib specialized types
 *
 * Copyright © 2005 Red Hat, Inc.
 * Copyright © 2011 Nokia Corporation
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include <config.h>

#include <string.h>

#include <glib.h>

#include <dbus/dbus-glib.h>
#include <dbus/dbus-glib-lowlevel.h>

typedef struct {
    int dummy;
} Fixture;

static void
setup (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  dbus_g_type_specialized_init ();
}

static void
test_au (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  GType type;
  GArray *instance;

  type = dbus_g_type_get_collection ("GArray", G_TYPE_UINT);
  g_assert (dbus_g_type_is_collection (type));
  g_assert (dbus_g_type_get_collection_specialization (type) == G_TYPE_UINT);

  instance = dbus_g_type_specialized_construct (type);
  g_assert (instance->len == 0);
  g_array_free (instance, TRUE);
}

typedef struct
{
  gboolean seen_foo;
  gboolean seen_baz;
} TestSpecializedHashData;

static void
test_specialized_hash (const GValue *key, const GValue *val, gpointer user_data)
{
  TestSpecializedHashData *data = user_data;

  g_assert (G_VALUE_HOLDS_STRING (key));
  g_assert (G_VALUE_HOLDS_STRING (val));

  if (!strcmp (g_value_get_string (key), "foo"))
    {
      data->seen_foo = TRUE;
      g_assert (!strcmp (g_value_get_string (val), "bar"));
    }
  else if (!strcmp (g_value_get_string (key), "baz"))
    {
      data->seen_baz = TRUE;
      g_assert (!strcmp (g_value_get_string (val), "moo"));
    }
  else
    {
      g_assert_not_reached ();
    }
}

static void
test_specialized_hash_2 (const GValue *key, const GValue *val, gpointer user_data)
{
  TestSpecializedHashData *data = user_data;
  const GValue *realval;

  g_assert (G_VALUE_HOLDS_STRING (key));
  g_assert (G_VALUE_TYPE (val) == G_TYPE_VALUE);

  realval = g_value_get_boxed (val);

  if (!strcmp (g_value_get_string (key), "foo"))
    {
      data->seen_foo = TRUE;
      g_assert (G_VALUE_HOLDS_UINT (realval));
      g_assert (g_value_get_uint (realval) == 20);
    }
  else if (!strcmp (g_value_get_string (key), "baz"))
    {
      data->seen_baz = TRUE;
      g_assert (G_VALUE_HOLDS_STRING (realval));
      g_assert (!strcmp ("bar", g_value_get_string (realval)));
    }
  else
    {
      g_assert_not_reached ();
    }
}

static void
test_map_ss (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  GType type;
  GHashTable *instance;
  GValue val = { 0, };
  TestSpecializedHashData hashdata;

  type = dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_STRING);
  g_assert (dbus_g_type_is_map (type));
  g_assert (dbus_g_type_get_map_key_specialization (type) == G_TYPE_STRING);
  g_assert (dbus_g_type_get_map_value_specialization (type) == G_TYPE_STRING);

  instance = dbus_g_type_specialized_construct (type);

  g_assert (g_hash_table_size (instance) == 0);
  g_hash_table_insert (instance, g_strdup ("foo"), g_strdup ("bar"));
  g_hash_table_insert (instance, g_strdup ("baz"), g_strdup ("moo"));
  g_assert (g_hash_table_size (instance) == 2);

  g_value_init (&val, type);
  g_value_take_boxed (&val, instance);
  hashdata.seen_foo = FALSE;
  hashdata.seen_baz = FALSE;
  dbus_g_type_map_value_iterate (&val,
                                 test_specialized_hash,
                                 &hashdata);

  g_assert (hashdata.seen_foo);
  g_assert (hashdata.seen_baz);

  g_value_unset (&val);
}

static void
test_map_sv (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  GType type;

  type = dbus_g_type_get_map ("GHashTable", G_TYPE_STRING, G_TYPE_VALUE);
  g_assert (dbus_g_type_is_map (type));
  g_assert (dbus_g_type_get_map_key_specialization (type) == G_TYPE_STRING);
  g_assert (dbus_g_type_get_map_value_specialization (type) == G_TYPE_VALUE);
  {
    GHashTable *instance;
    GValue val = { 0, };
    TestSpecializedHashData hashdata;
    DBusGTypeSpecializedAppendContext ctx;
    GValue *eltval;

    instance = dbus_g_type_specialized_construct (type);
    g_value_init (&val, type);
    g_value_take_boxed (&val, instance);

    dbus_g_type_specialized_init_append (&val, &ctx);

    {
      GValue keyval = { 0, };
      GValue valval = { 0, };
      g_value_init (&keyval, G_TYPE_STRING);
      g_value_set_string (&keyval, "foo");

      g_value_init (&valval, G_TYPE_VALUE);
      eltval = g_new0 (GValue, 1);
      g_value_init (eltval, G_TYPE_UINT);
      g_value_set_uint (eltval, 20);
      g_value_take_boxed (&valval, eltval);
      dbus_g_type_specialized_map_append (&ctx, &keyval, &valval);
    }

    {
      GValue keyval = { 0, };
      GValue valval = { 0, };
      g_value_init (&keyval, G_TYPE_STRING);
      g_value_set_string (&keyval, "baz");
      g_value_init (&valval, G_TYPE_VALUE);
      eltval = g_new0 (GValue, 1);
      g_value_init (eltval, G_TYPE_STRING);
      g_value_set_string (eltval, "bar");
      g_value_take_boxed (&valval, eltval);
      dbus_g_type_specialized_map_append (&ctx, &keyval, &valval);
    }

    hashdata.seen_foo = FALSE;
    hashdata.seen_baz = FALSE;
    dbus_g_type_map_value_iterate (&val,
                                   test_specialized_hash_2,
                                   &hashdata);

    g_assert (hashdata.seen_foo);
    g_assert (hashdata.seen_baz);

    g_value_unset (&val);
  }
}

static void
test_ao_slist (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  GType type;

  type = dbus_g_type_get_collection ("GSList", G_TYPE_OBJECT);
  g_assert (dbus_g_type_is_collection (type));
  g_assert (dbus_g_type_get_collection_specialization (type) == G_TYPE_OBJECT);
  {
    GSList *instance, *tmp, *copy;
    GValue val = {0, };
    GValue copyval = {0, };
    DBusGTypeSpecializedAppendContext ctx;
    GObject *objects[3];
    int i;

    instance = dbus_g_type_specialized_construct (type);
    g_assert (instance == NULL);

    g_value_init (&val, type);
    g_value_take_boxed (&val, instance);

    dbus_g_type_specialized_init_append (&val, &ctx);

    for (i = 0; i < 3; i++)
      {
        GValue eltval = { 0, };
        GObject *obj = g_object_new (G_TYPE_OBJECT, NULL);

        g_assert (obj != NULL);
        objects[i] = obj;
        g_object_add_weak_pointer (obj, (gpointer) (objects + i));

        g_value_init (&eltval, G_TYPE_OBJECT);
        g_value_take_object (&eltval, obj);
        dbus_g_type_specialized_collection_append (&ctx, &eltval);
      }

    dbus_g_type_specialized_collection_end_append (&ctx);

    instance = g_value_get_boxed (&val);
    g_assert (g_slist_length (instance) == 3);

    for (tmp = instance; tmp; tmp = tmp->next)
      {
        GObject *obj = tmp->data;
        g_assert (G_IS_OBJECT (obj));
        g_assert (obj->ref_count == 1);
      }

    g_value_init (&copyval, type);
    g_value_copy (&val, &copyval);

    copy = g_value_get_boxed (&copyval);
    g_assert (g_slist_length (copy) == 3);

    for (tmp = copy; tmp; tmp = tmp->next)
      {
        GObject *obj = tmp->data;
        g_assert (G_IS_OBJECT (obj));
        g_assert (obj->ref_count == 2);
      }

    g_value_unset (&copyval);

    for (i = 0; i < 3; i++)
      {
        g_assert (objects[i] != NULL);
      }

    for (tmp = instance; tmp; tmp = tmp->next)
      {
        GObject *obj = tmp->data;
        g_assert (G_IS_OBJECT (obj));
        g_assert (obj->ref_count == 1);
      }

    g_value_unset (&val);

    for (i = 0; i < 3; i++)
      {
        g_assert (objects[i] == NULL);
      }
  }
}

static void
test_as_ptrarray (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  GType type;

  type = dbus_g_type_get_collection ("GPtrArray", G_TYPE_STRING);
  g_assert (dbus_g_type_is_collection (type));
  g_assert (dbus_g_type_get_collection_specialization (type) == G_TYPE_STRING);
  {
    GPtrArray *instance;
    DBusGTypeSpecializedAppendContext ctx;
    GValue val = {0, };
    GValue eltval = {0, };

    instance = dbus_g_type_specialized_construct (type);

    g_assert (instance->len == 0);

    g_value_init (&val, type);
    g_value_take_boxed (&val, instance);

    dbus_g_type_specialized_init_append (&val, &ctx);

    g_value_init (&eltval, G_TYPE_STRING);
    g_value_set_static_string (&eltval, "foo");
    dbus_g_type_specialized_collection_append (&ctx, &eltval);

    g_value_reset (&eltval);
    g_value_set_static_string (&eltval, "bar");
    dbus_g_type_specialized_collection_append (&ctx, &eltval);

    g_value_reset (&eltval);
    g_value_set_static_string (&eltval, "baz");
    dbus_g_type_specialized_collection_append (&ctx, &eltval);

    dbus_g_type_specialized_collection_end_append (&ctx);

    g_assert (instance->len == 3);

    g_assert (!strcmp ("foo", g_ptr_array_index (instance, 0)));
    g_assert (!strcmp ("bar", g_ptr_array_index (instance, 1)));
    g_assert (!strcmp ("baz", g_ptr_array_index (instance, 2)));

    g_value_unset (&val);
  }
}

static void
test_suo (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
  GType type;

  type = dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_UINT, DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);
  g_assert (dbus_g_type_is_struct (type));
  g_assert (dbus_g_type_get_struct_size (type) == 3);
  g_assert (dbus_g_type_get_struct_member_type (type, 0) == G_TYPE_STRING);
  g_assert (dbus_g_type_get_struct_member_type (type, 1) == G_TYPE_UINT);
  g_assert (dbus_g_type_get_struct_member_type (type, 2) == DBUS_TYPE_G_OBJECT_PATH);
  {
    GValueArray *instance;
    GValue val = {0, };
    GValue memval = {0, };

    instance = dbus_g_type_specialized_construct (type);

    g_assert (instance->n_values == 3);

    g_value_init (&val, type);
    g_value_take_boxed (&val, instance);

    g_value_init (&memval, G_TYPE_STRING);
    g_value_set_static_string (&memval, "foo");
    dbus_g_type_struct_set_member (&val, 0, &memval);
    g_value_unset (&memval);

    g_value_init (&memval, G_TYPE_UINT);
    g_value_set_uint (&memval, 42);
    dbus_g_type_struct_set_member (&val, 1, &memval);
    g_value_unset (&memval);

    g_value_init (&memval, DBUS_TYPE_G_OBJECT_PATH);
    g_value_set_static_boxed (&memval, "/bar/moo/foo/baz");
    dbus_g_type_struct_set_member (&val, 2, &memval);
    g_value_unset (&memval);

    g_assert (instance->n_values == 3);

    g_value_init (&memval, G_TYPE_STRING);
    dbus_g_type_struct_get_member (&val, 0, &memval);
    g_assert (0 == strcmp (g_value_get_string (&memval), "foo"));
    g_value_unset (&memval);

    g_value_init (&memval, G_TYPE_UINT);
    dbus_g_type_struct_get_member (&val, 1, &memval);
    g_assert (g_value_get_uint (&memval) == 42);
    g_value_unset (&memval);

    g_value_init (&memval, DBUS_TYPE_G_OBJECT_PATH);
    dbus_g_type_struct_get_member (&val, 2, &memval);
    g_assert (0 == strcmp ((gchar*) g_value_get_boxed (&memval),
                           "/bar/moo/foo/baz"));
    g_value_unset (&memval);

    g_value_unset (&val);
  }

  type = dbus_g_type_get_struct ("GValueArray", G_TYPE_STRING, G_TYPE_UINT, DBUS_TYPE_G_OBJECT_PATH, G_TYPE_INVALID);
  g_assert (dbus_g_type_is_struct (type));
  g_assert (dbus_g_type_get_struct_size (type) == 3);
  g_assert (dbus_g_type_get_struct_member_type (type, 0) == G_TYPE_STRING);
  g_assert (dbus_g_type_get_struct_member_type (type, 1) == G_TYPE_UINT);
  g_assert (dbus_g_type_get_struct_member_type (type, 2) == DBUS_TYPE_G_OBJECT_PATH);
  {
    GValueArray *instance;
    GValue val = {0, };

    instance = dbus_g_type_specialized_construct (type);

    g_assert (instance->n_values == 3);

    g_value_init (&val, type);
    g_value_take_boxed (&val, instance);

    dbus_g_type_struct_set (&val,
                            0,"foo",
                            1, 42,
                            2, "/bar/moo/foo/baz",
                            G_MAXUINT);

    g_assert (instance->n_values == 3);

    {
      gchar *string;
      guint intval;
      gchar *path;

      dbus_g_type_struct_get (&val,
                              0, &string,
                              1, &intval,
                              2, &path,
                              G_MAXUINT);

      g_assert (0 == strcmp (string, "foo"));
      g_assert (intval == 42);
      g_assert (0 == strcmp (path, "/bar/moo/foo/baz"));
    }

    g_value_unset (&val);
  }
}

static void
teardown (Fixture *f G_GNUC_UNUSED,
    gconstpointer context G_GNUC_UNUSED)
{
}

int
main (int argc,
    char **argv)
{
  g_test_init (&argc, &argv, NULL);
  g_type_init ();

  g_test_add ("/specialized-types/au/GArray", Fixture, NULL,
      setup, test_au, teardown);

  g_test_add ("/specialized-types/a{ss}/GHashTable", Fixture, NULL,
      setup, test_map_ss, teardown);

  g_test_add ("/specialized-types/a{sv}/GHashTable", Fixture, NULL,
      setup, test_map_sv, teardown);

  g_test_add ("/specialized-types/ao/GSList", Fixture, NULL,
      setup, test_ao_slist, teardown);

  g_test_add ("/specialized-types/as/GPtrArray", Fixture, NULL,
      setup, test_as_ptrarray, teardown);

  g_test_add ("/specialized-types/suo/GValueArray", Fixture, NULL,
      setup, test_suo, teardown);

  return g_test_run ();
}
