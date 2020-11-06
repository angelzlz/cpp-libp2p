/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_PROTOCOL_KADEMLIA_GETVALUEEXECUTOR
#define LIBP2P_PROTOCOL_KADEMLIA_GETVALUEEXECUTOR

#include <memory>
#include <queue>
#include <unordered_set>

#include <boost/multi_index/hashed_index_fwd.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container_fwd.hpp>
#include <libp2p/common/types.hpp>
#include <libp2p/host/host.hpp>
#include <libp2p/protocol/common/sublogger.hpp>
#include <libp2p/protocol/kademlia/common.hpp>
#include <libp2p/protocol/kademlia/config.hpp>
#include <libp2p/protocol/kademlia/impl/content_routing_table.hpp>
#include <libp2p/protocol/kademlia/impl/executors_factory.hpp>
#include <libp2p/protocol/kademlia/impl/peer_info_with_distance.hpp>
#include <libp2p/protocol/kademlia/impl/response_handler.hpp>
#include <libp2p/protocol/kademlia/impl/session.hpp>
#include <libp2p/protocol/kademlia/impl/session_host.hpp>
#include <libp2p/protocol/kademlia/peer_routing.hpp>

namespace libp2p::protocol::kademlia {

  class GetValueExecutor
      : public ResponseHandler,
        public std::enable_shared_from_this<GetValueExecutor> {
   public:
    GetValueExecutor(const Config &config, std::shared_ptr<Host> host,
                     std::shared_ptr<SessionHost> session_host,
                     std::shared_ptr<PeerRouting> peer_routing,
                     std::shared_ptr<ContentRoutingTable> content_routing_table,
                     std::shared_ptr<ExecutorsFactory> executor_factory,
                     ContentId sought_content_id_,
                     std::unordered_set<PeerInfo> nearest_peer_infos,
                     FoundValueHandler handler);

    ~GetValueExecutor() override;

    outcome::result<void> start();

    /// @see ResponseHandler::responseTimeout
    scheduler::Ticks responseTimeout() const override {
      return scheduler::toTicks(10s);  // TODO(xDimon): Need to read from config
    }

    /// @see ResponseHandler::match
    bool match(const Message &msg) const override;

    /// @see ResponseHandler::onResult
    void onResult(const std::shared_ptr<Session> &session,
                  outcome::result<Message> msg_res) override;

   private:
    /// Spawns new request
    void spawn();

    /// Handles result of connection
    void onConnected(
        outcome::result<std::shared_ptr<connection::Stream>> stream_res);

    static std::atomic_size_t instance_number;

    // Primary
    const Config &config_;
    std::shared_ptr<Host> host_;
    std::shared_ptr<SessionHost> session_host_;
    std::shared_ptr<PeerRouting> peer_routing_;
    std::shared_ptr<ContentRoutingTable> content_routing_table_;
    std::shared_ptr<ExecutorsFactory> executor_factory_;

    // Secondary
    const ContentId sought_content_id_;
    std::unordered_set<PeerInfo, std::hash<PeerInfo>, PeerInfo::EqualByPeerId>
        nearest_peer_infos_;
    FoundValueHandler handler_;

    std::shared_ptr<std::vector<uint8_t>> serialized_request_;
    std::priority_queue<PeerInfoWithDistance> queue_;  // Pn
    size_t requests_in_progress_ = 0;

    struct ByPeerId;
    struct ByValue;
    struct Record {
      PeerId peer;
      Value value;
    };

    /// Table of Record indexed by peer and value
    using Table = boost::multi_index_container<
        Record,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_unique<
                boost::multi_index::tag<ByPeerId>,
                boost::multi_index::member<Record, PeerId, &Record::peer>,
                std::hash<PeerId>>,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<ByValue>,
                boost::multi_index::member<Record, Value, &Record::value>>>>;

    std::unique_ptr<Table> received_records_;  // cnt

    boost::optional<PeerId> best_value_;    // best
    std::vector<PeerId> best_peers_;        // Pb
    std::unordered_set<PeerId> requested_;  // Pq

    bool started_ = false;
    bool done_ = false;

    SubLogger log_;
  };

}  // namespace libp2p::protocol::kademlia

#endif  // LIBP2P_PROTOCOL_KADEMLIA_GETVALUEEXECUTOR
