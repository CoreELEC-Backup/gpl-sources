AC_DEFUN([MD_CHECK_LIBUSB018B],
  [
    AC_CHECK_HEADERS([usb.h])

    LIBUSB_CFLAGS="`pkg-config --cflags libusb`"
    LIBUSB_LIBS="`pkg-config --libs libusb`"

    AC_SUBST(LIBUSB_CFLAGS)
    AC_SUBST(LIBUSB_LIBS)
    
    test "$os_support" = "linux" && {
      AC_MSG_CHECKING(for libusb version)
      AC_LINK_IFELSE(
        [
          #include <usb.h>

          int main(void)
          {
            usb_dev_handle* dev;
            usb_detach_kernel_driver_np(dev, 0);
            return 0;
          }
        ],
        AC_MSG_RESULT(ok (>= 0.1.8beta)),
        AC_MSG_ERROR(libhid requires libusb version 0.1.8beta or greater)
      )
    }
  ])

