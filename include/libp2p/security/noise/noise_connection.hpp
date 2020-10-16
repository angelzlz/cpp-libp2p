/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIBP2P_INCLUDE_LIBP2P_SECURITY_NOISE_NOISE_CONNECTION_HPP
#define LIBP2P_INCLUDE_LIBP2P_SECURITY_NOISE_NOISE_CONNECTION_HPP

#include <libp2p/connection/secure_connection.hpp>

#include <libp2p/common/logger.hpp>
#include <libp2p/crypto/crypto_provider.hpp>
#include <libp2p/crypto/key.hpp>
#include <libp2p/crypto/key_marshaller.hpp>
#include <libp2p/crypto/x25519_provider.hpp>
#include <libp2p/security/noise/crypto/state.hpp>
#include <libp2p/security/noise/handshake_message_marshaller_impl.hpp>
#include <libp2p/security/noise/insecure_rw.hpp>

namespace libp2p::connection {
  class NoiseConnection : public SecureConnection,
                          public std::enable_shared_from_this<NoiseConnection> {
   public:
    ~NoiseConnection() override = default;

    NoiseConnection(
        std::shared_ptr<RawConnection> raw_connection,
        crypto::PublicKey localPubkey, crypto::PublicKey remotePubkey,
        std::shared_ptr<crypto::marshaller::KeyMarshaller> key_marshaller,
        std::shared_ptr<security::noise::CipherState> encoder,
        std::shared_ptr<security::noise::CipherState> decoder);

    bool isClosed() const override;

    outcome::result<void> close() override;

    void read(gsl::span<uint8_t> out, size_t bytes,
              ReadCallbackFunc cb) override;

    void readSome(gsl::span<uint8_t> out, size_t bytes,
                  ReadCallbackFunc cb) override;

    void write(gsl::span<const uint8_t> in, size_t bytes,
               WriteCallbackFunc cb) override;

    void writeSome(gsl::span<const uint8_t> in, size_t bytes,
                   WriteCallbackFunc cb) override;

    bool isInitiator() const noexcept override;

    outcome::result<multi::Multiaddress> localMultiaddr() override;

    outcome::result<multi::Multiaddress> remoteMultiaddr() override;

    outcome::result<peer::PeerId> localPeer() const override;

    outcome::result<peer::PeerId> remotePeer() const override;

    outcome::result<crypto::PublicKey> remotePublicKey() const override;

   private:
    std::shared_ptr<RawConnection> raw_connection_;
    crypto::PublicKey local_;
    crypto::PublicKey remote_;
    std::shared_ptr<crypto::marshaller::KeyMarshaller> key_marshaller_;
    std::shared_ptr<security::noise::CipherState> encoder_cs_;
    std::shared_ptr<security::noise::CipherState> decoder_cs_;
    std::shared_ptr<common::ByteArray> frame_buffer_;
    std::shared_ptr<security::noise::InsecureReadWriter> framer_;
    size_t already_read_;
    size_t already_wrote_;
    common::ByteArray writing_;
    common::Logger log_ = common::createLogger("NoiseConn");
  };
}  // namespace libp2p::connection

#endif  // LIBP2P_INCLUDE_LIBP2P_SECURITY_NOISE_NOISE_CONNECTION_HPP