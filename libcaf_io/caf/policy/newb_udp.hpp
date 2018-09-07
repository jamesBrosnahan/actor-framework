/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright 2011-2018 Dominik Charousset                                     *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#pragma once

#include "caf/io/network/newb.hpp"
#include "caf/io/network/native_socket.hpp"

namespace caf {
namespace policy {

struct udp_transport : public io::network::transport_policy {
  udp_transport();

  io::network::rw_state read_some(io::network::newb_base* parent) override;

  inline bool should_deliver() override {
    CAF_LOG_TRACE("");
    return received_bytes != 0 && sender == endpoint;
  }

  void prepare_next_read(io::network::newb_base*) override;

  inline void configure_read(io::receive_policy::config) override {
    // nop
  }

  io::network::rw_state write_some(io::network::newb_base* parent) override;

  void prepare_next_write(io::network::newb_base* parent) override;

  io::network::byte_buffer& wr_buf() override;

  void flush(io::network::newb_base* parent) override;

  expected<io::network::native_socket>
  connect(const std::string& host, uint16_t port,
          optional<io::network::protocol::network> preferred = none) override;

  // State for reading.
  size_t maximum;
  bool first_message;

  // State for writing.
  bool writing;
  size_t written;
  size_t offline_sum;
  std::deque<size_t> send_sizes;
  std::deque<size_t> offline_sizes;

  // UDP endpoints.
  io::network::ip_endpoint endpoint;
  io::network::ip_endpoint sender;
};

struct accept_udp : public io::network::accept_policy {
  expected<io::network::native_socket>
  create_socket(uint16_t port, const char* host, bool reuse = false) override;

  std::pair<io::network::native_socket, io::network::transport_policy_ptr>
  accept(io::network::newb_base*) override;

  void init(io::network::newb_base& n) override;
};

template <class T>
using udp_protocol = io::network::generic_protocol<T>;

} // namespace policy
} // namespace caf
