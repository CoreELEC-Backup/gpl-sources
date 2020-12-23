import hid




# import all symbolic constants from hid --------------------------------------
for identifier in dir(hid):
    if identifier.startswith("HID_"):
        locals()[identifier] = hid.__dict__[identifier]




# error handling --------------------------------------------------------------
class HIDError(Exception):
    def __init__(self, code, description):
        self.code = code
        self.description = description
    def __str__(self):
        return repr((self.code, self.description))




def _hid_raise(op, result):
    if isinstance(result, int):
        result_code = result
        retval = None
    elif isinstance(result, (tuple, list)):
        result_code = result[0]
        if len(result) == 2:
            retval = result[1]
        else:
            retval = result[1:]
    else:
        raise ValueError, "result must be either an int or a tuple"

    if result_code != hid.HID_RET_SUCCESS:
        try:
            raise HIDError, (result_code, "%s: %s" % (op, hid.hid_strerror(result_code)))
        except KeyError:
            raise HIDError, (result_code, "%s: Unknown error code" % op)
    else:
        return retval




# debugging -------------------------------------------------------------------
def set_debug(value):
    hid.hid_set_debug(value)

def set_debug_stream(stream):
    hid.hid_set_debug_stream(stream)

def set_usb_debug(level):
    hid.hid_set_usb_debug(level)




# init / cleanup --------------------------------------------------------------
IS_INITIALIZED = [False]

def init():
    import atexit

    _hid_raise("init", hid.hid_init())
    IS_INITIALIZED[0] = True
    atexit.register(_finalize_hid)

def is_initialized():
    return IS_INITIALIZED[0]

def cleanup():
    _hid_raise("cleanup", hid.hid_cleanup())
    IS_INITIALIZED[0] = False

def _finalize_hid():
    if is_initialized():
        cleanup()




# interface -------------------------------------------------------------------
class Interface:
    def __init__(self, vendor_id, product_id, interface_number, 
            force=True, retries=3):
        self.is_open = False

        if not is_initialized():
            init()

        matcher = hid.HIDInterfaceMatcher()
        matcher.vendor_id = vendor_id
        matcher.product_id = product_id

        self.interface = hid.hid_new_HIDInterface()

        if force:
            _hid_raise("force_open", hid.hid_force_open(
                self.interface, interface_number, matcher, retries))
        else:
            _hid_raise("open", hid.hid_open(self.interface, 0, matcher))

        self.is_open = True


    def write_identification(self, stream):
        _hid_raise("write_identification", hid.hid_write_identification(
            stream, self.interface))

    def dump_tree(self, stream):
        _hid_raise("dump_tree", hid.hid_dump_tree(
            stream, self.interface))

    def __del__(self):
        if self.is_open:
            self.close()

    def close(self):
        _hid_raise("close", hid.hid_close(self.interface))

    def get_input_report(self, path, size):
        return _hid_raise("get_input_report",
                hid.hid_get_input_report(self.interface, path, size))

    def set_output_report(self, path, bytes):
        _hid_raise("set_output_report",
                hid.hid_set_output_report(self.interface, path, bytes))

    def get_feature_report(self, path, size):
        return _hid_raise("get_feature_report",
                hid.hid_get_feature_report(self.interface, path, size))

    def set_feature_report(self, path, bytes):
        _hid_raise("set_feature_report",
                hid.hid_set_feature_report(self.interface, path, bytes))

    def get_item_value(self, path):
        return _hid_raise("get_item_value",
                hid.hid_get_item_value(self.interface, path))

    def interrupt_read(self, endpoint, size, timeout=0):
        return _hid_raise("interrupt_read",
                hid.hid_interrupt_read(self.interface, endpoint, size, timeout))

    def interrupt_write(self, endpoint, bytes, timeout=0):
        return _hid_raise("interrupt_write",
                hid.hid_interrupt_write(self.interface, endpoint, bytes, timeout))

    def set_idle(self, duration, report_id):
        return _hid_raise("set_idle",
                hid.hid_set_idle(self.interface, duration, report_id))

