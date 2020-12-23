class Inadyn < Formula
  desc "Dynamic DNS client with IPv4, IPv6, and SSL/TLS support"
  homepage "http://troglobit.com/inadyn.html"
  head "https://github.com/troglobit/inadyn.git"
  version "2.4-HEAD"

  depends_on "autoconf"   => :build
  depends_on "automake"   => :build
  depends_on "cmake"      => :build
  depends_on "confuse"
  depends_on "gnutls"
  depends_on "libtool"    => :build
  depends_on "pkg-config" => :build

  def install
    mkdir_p buildpath/"inadyn/m4"
    system "autoreconf", "-W", "portability", "-vif" unless build.stable?
    system "./configure", "--disable-dependency-tracking",
                          "--disable-silent-rules",
                          "--prefix=#{prefix}",
                          "--sysconfdir=#{etc}",
                          "--localstatedir=#{var}"
    system "make", "install"
  end

  def caveats; <<-EOF
    Place your DDNS provider's parameters in #{etc}/inadyn.conf

    Sample configurations can be found in #{HOMEBREW_PREFIX}/share/doc/inadyn/examples/
  EOF
  end

  test do
    system "#{sbin}/inadyn", "--check-config", "--config=#{HOMEBREW_PREFIX}/share/doc/inadyn/examples/inadyn.conf"
  end
end
