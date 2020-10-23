/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_PROTOCOL_KADEMLIA_ROUTINGTABLE
#define LIBP2P_PROTOCOL_KADEMLIA_ROUTINGTABLE

#include <libp2p/event/bus.hpp>
#include <libp2p/protocol/kademlia/common.hpp>
#include <libp2p/protocol/kademlia/config.hpp>
#include <libp2p/protocol/kademlia/node_id.hpp>

namespace libp2p::protocol::kademlia {

  namespace events {

    using PeerAddedChannel =
        event::channel_decl<struct PeerAdded, peer::PeerId>;

    using PeerRemovedChannel =
        event::channel_decl<struct PeerRemoved, peer::PeerId>;

  }  // namespace events

  /**
   * @class RoutingTable
   *
   * Equivalent implementation of RoutingTable from
   * https://sourcegraph.com/github.com/libp2p/go-libp2p-kbucket
   */
  struct RoutingTable {
    virtual ~RoutingTable() = default;

    /// Adds/moves the given @param peer to the front of its respective bucket
    virtual outcome::result<void> update(const peer::PeerId &peer) = 0;

    /// Removes a peer with given @param peer from the routing table.
    /// This is to be used when we are sure a node has disconnected completely.
    virtual void remove(const peer::PeerId &peer) = 0;

    /// Returns all known peers
    virtual std::vector<peer::PeerId> getAllPeers() const = 0;

    /// Returns upto @param count closest peers to the given @param node
    virtual std::vector<peer::PeerId> getNearestPeers(const NodeId &node,
                                                      size_t count) = 0;

    /// Returns the total number of peers in the routing table
    virtual size_t size() const = 0;
  };

}  // namespace libp2p::protocol::kademlia

#endif  // LIBP2P_PROTOCOL_KADEMLIA_ROUTINGTABLE
