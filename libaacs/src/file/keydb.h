
/* encrypted keys */

static const uint32_t internal_device_number = 0;

static const uint8_t internal_dk_list[][21] = {
  {
  },
};

static const uint8_t internal_pk_list[][16] = {
  {
  },
};

static const uint8_t internal_hc_list[][112] = {
  {
  },
};

/* customize this function to "hide" the keys in the binary */

static void decrypt_key(uint8_t *out, const uint8_t *in, size_t size)
{
    memcpy(out, in, size);
}
