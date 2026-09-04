  #include <arpa/inet.h>
  #include <netdb.h>
  #include <netinet/in.h>
  #include <print>

  namespace http {

  void display_addr_info(const addrinfo& addr_info) {
      char ipstr[INET6_ADDRSTRLEN]{};
      const void* network_address = nullptr;
      const char* family_str = nullptr;
      int port = 0;

      if (addr_info.ai_addr == nullptr) {
          std::println("No address available");
          return;
      }

      if (addr_info.ai_family == AF_INET) {
          auto* ipv4 =
              reinterpret_cast<const sockaddr_in*>(addr_info.ai_addr);

          network_address = &ipv4->sin_addr.s_addr;
          port = ntohs(ipv4->sin_port);
          family_str = "IPv4";
      } else if (addr_info.ai_family == AF_INET6) {
          auto* ipv6 =
              reinterpret_cast<const sockaddr_in6*>(addr_info.ai_addr);

          network_address = ipv6->sin6_addr.s6_addr;
          port = ntohs(ipv6->sin6_port);
          family_str = "IPv6";
      } else {
          std::println("Unknown family: {}", addr_info.ai_family);
          return;
      }

      if (inet_ntop(
              addr_info.ai_family,
              network_address,
              ipstr,
              sizeof(ipstr)) == nullptr) {
          std::println("Could not convert address to text");
          return;
      }

      std::println(
          "{} {}:{} socktype={} protocol={} flags={} canon={} addrlen={}",
          family_str,
          ipstr,
          port,
          addr_info.ai_socktype,
          addr_info.ai_protocol,
          addr_info.ai_flags,
          addr_info.ai_canonname ? addr_info.ai_canonname : "(none)",
          addr_info.ai_addrlen);
  }

  } // namespace http
