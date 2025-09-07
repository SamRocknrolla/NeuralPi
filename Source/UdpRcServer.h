/*
  ==============================================================================

    UdpRcServer.h
    Created: 26 Aug 2025 8:57:07pm
    Author:  Sam

  ==============================================================================
*/

#pragma once

#include "cmn/ElapsedTimer.h"
#include "cmn/IUdpRcListener.h"
#include "cmn/NeuralPiProto.h"
#include "cmn/SimpleOscMsg.h"

class UdpRcServer : public juce::Thread {
public:
    explicit UdpRcServer(int srvPport, const juce::String mcastAddr, IUdpRcServerListener& listener)
        : juce::Thread("UdpRcServerThread")
        , m_srvPort(srvPport)
        , m_clnPort(srvPport + 1)
        , m_mcastAddr(mcastAddr)
        , m_udpSrvSocket(true)
        , m_udpClnSocket(true)
        , m_listener(listener)
        , m_state(IUdpRcListener::EState::Idle)
        , m_sessionId(NpRpcProto::NPRPC_INV_SESS_ID)
        , m_sessionTs(NpRpcProto::NPRPC_INV_SESS_TS)
        , m_hbState(EHbState::Idle)
        , m_hbMissedCount(0)
    {
    }

    ~UdpRcServer() override
    {
        signalThreadShouldExit();
        stopThread(1000);
        m_udpSrvSocket.shutdown();
        m_udpClnSocket.shutdown();
    }

    void run() override {
        setState(IUdpRcListener::EState::Idle);
        // bind OSR receiver
        DBG(reinterpret_cast<uint64>(Thread::getCurrentThreadId()) << String(": ") << __func__);
        if (!m_udpSrvSocket.bindToPort(NpRpcProto::NPRPC_SRV_PORT)) {
            DBG("Error: could not bind UDP port " + NpRpcProto::NPRPC_SRV_PORT);
            setState(IUdpRcListener::EState::Error);
            return;
        }
        // join to multicast group to publish IP via broadcast request
        if (!m_udpSrvSocket.joinMulticast(NpRpcProto::NPRPC_MCAST_ADDR)) {
            DBG("Error: could not join to multicast group: " + NpRpcProto::NPRPC_MCAST_ADDR);
            setState(IUdpRcListener::EState::Error);
            return;
        }

        while (!threadShouldExit())
        {
            juce::MemoryBlock buffer;
            //juce::IPAddress senderIp;
            int clnPort;
            juce::String clnIpStr;
            buffer.setSize(1024, true);
            // Wait until the socket is ready to receive data (blocking with a timeout)
            if (m_udpSrvSocket.waitUntilReady(true, 10))  // Wait for data with a timeout of 10ms
            {
                // Receive a packet (non-blocking)
                int bytesRead = m_udpSrvSocket.read(buffer.getData(), static_cast<int>(buffer.getSize()), false, clnIpStr, clnPort);

                if (bytesRead > 0)
                {
                    // Parse the UDP packet and check header
                    SimpleOscMsg msg;
                    if (msg.DeserializeFrom(buffer, bytesRead) && msg.size() >= 3 &&
                        msg[NpRpcProto::EHeader_Version].isInt32() &&
                        msg[NpRpcProto::EHeader_SessionId].isInt32() &&
                        msg[NpRpcProto::EHeader_Type].isInt32()) {
                        // Check protocol Version and Type
                        int32 _version = msg[NpRpcProto::EHeader_Version].getInt32();
                        int32 _sessionId = msg[NpRpcProto::EHeader_SessionId].getInt32();
                        NpRpcProto::EPacketType _type = static_cast<NpRpcProto::EPacketType>(msg[NpRpcProto::EHeader_Type].getInt32());

                        if (_version == NpRpcProto::NPRPC_VER &&
                            _type >= NpRpcProto::EPacketType::ConnectReq &&
                            _type <= NpRpcProto::EPacketType::BroadcastReq) {

                            // Process broadcast request
                            if (msg.getAddress() == NpRpcProto::NRPC_BCAST_CH &&
                                _sessionId == NpRpcProto::NPRPC_INV_SESS_ID &&
                                _type == NpRpcProto::EPacketType::BroadcastReq) {

                                // Send broadcast response
                                juce::MemoryBlock buf;
                                NpRpcProto::genBroadcastRes().SerializeTo(buf);
                                m_udpClnSocket.write(clnIpStr, m_clnPort, buf.getData(), static_cast<int>(buf.getSize()));

                                // Notify UI about broadcast request
                                //juce::MessageManager::callAsync([&updater = m_listener, resIpStr]() {
                                //    updater.onBrReceived(resIpStr);
                                //    });
                            }
                            // process OCR /NpRps/connect
                            else if (msg.getAddress() == NpRpcProto::NRPC_CONNECT_CH) {

                                switch (_type) {
                                case NpRpcProto::EPacketType::ConnectReq:
                                {
                                    if (m_state.get() != IUdpRcListener::EState::Idle) {
                                        setState(IUdpRcListener::EState::Disconnecting);
                                        setHbState(EHbState::Idle);
                                        DBG("Request to RC connection has been rejected!");
                                    }
                                    else {
                                        //if (_sessionId == NpRpcProto::NPRPC_INV_SESS_ID) {}
                                        m_sessionId = Time::getMillisecondCounter();
                                        m_sessionTs = Time::getMillisecondCounter();
                                        m_clnAddr = clnIpStr;
                                        setState(IUdpRcListener::EState::Connecting);

                                        // Notify UI about connect request
                                        juce::MessageManager::callAsync([&updater = m_listener, clnIpStr]() {
                                            updater.onConnReceived(clnIpStr);
                                            });
                                    }
                                    break;
                                }
                                case NpRpcProto::EPacketType::HeartbeatReq:
                                    if (m_state.get() == IUdpRcListener::EState::Connected) {
                                        setHbState(EHbState::Ready);
                                    }
                                    break;
                                case NpRpcProto::EPacketType::AbortReq:
                                    if (m_state.get() == IUdpRcListener::EState::Connected) {
                                        // Clear current session
                                        m_sessionId = NpRpcProto::NPRPC_INV_SESS_ID;
                                        m_sessionTs = NpRpcProto::NPRPC_INV_SESS_TS;
                                        setState(IUdpRcListener::EState::Idle);
                                    }
                                    break;
                                default:
                                    break;
                                }
                            }
                            // process OCR /NpRps/knob
                            else if (msg.getAddress() == NpRpcProto::NRPC_KNOB_CH &&
                                _sessionId != NpRpcProto::NPRPC_INV_SESS_ID &&
                                (m_sessionId == NpRpcProto::NPRPC_INV_SESS_ID || _sessionId == m_sessionId) &&
                                _type == NpRpcProto::EPacketType::UpdateKnobMsg) {

                                juce::MessageManager::callAsync([&updater = m_listener, msg]() {
                                    updater.updateKnob(msg[NpRpcProto::EUpdateKnobMsg_KnobId].getInt32(),
                                        msg[NpRpcProto::EUpdateKnobMsg_KnobValue].getFloat32());
                                    });
                            }
                            // process OCR /NpRps/model
                            else if (msg.getAddress() == NpRpcProto::NRPC_MODEL_CH &&
                                _sessionId != NpRpcProto::NPRPC_INV_SESS_ID &&
                                (m_sessionId == NpRpcProto::NPRPC_INV_SESS_ID || _sessionId == m_sessionId) &&
                                _type == NpRpcProto::EPacketType::SelectModelMsg) {

                                juce::MessageManager::callAsync([&updater = m_listener, msg]() {
                                    updater.updateModelIndex(msg[NpRpcProto::ESelectModel_ModelId].getInt32(),
                                        msg[NpRpcProto::ESelectModel_ItemIndex].getInt32());
                                    });
                            }
                            // process OCR /NpRps/knob
                            else if (msg.getAddress() == NpRpcProto::NRPC_KNOB_CH &&
                                _sessionId != NpRpcProto::NPRPC_INV_SESS_ID &&
                                (m_sessionId == NpRpcProto::NPRPC_INV_SESS_ID || _sessionId == m_sessionId) &&
                                _type == NpRpcProto::EPacketType::UpdateKnobMsg) {

                                juce::MessageManager::callAsync([&updater = m_listener, msg]() {
                                    updater.updateKnob(msg[NpRpcProto::EUpdateKnobMsg_KnobId].getInt32(),
                                        msg[NpRpcProto::EUpdateKnobMsg_KnobValue].getFloat32());
                                    });
                            }
                            // process OCR /NpRps/model
                            else if (msg.getAddress() == NpRpcProto::NRPC_MODEL_CH &&
                                _sessionId != NpRpcProto::NPRPC_INV_SESS_ID &&
                                (m_sessionId == NpRpcProto::NPRPC_INV_SESS_ID || _sessionId == m_sessionId) &&
                                _type == NpRpcProto::EPacketType::SelectModelMsg) {

                                juce::MessageManager::callAsync([&updater = m_listener, msg]() {
                                    updater.updateModelIndex(msg[NpRpcProto::ESelectModel_ModelId].getInt32(),
                                        msg[NpRpcProto::ESelectModel_ItemIndex].getInt32());
                                    });
                            }
                        }
                    }
                }
            }
            stateStep();
            sendTxQueue();
            //            sendMcastTxQueue();
        }
        DBG("UdpReceiverThread <=");
    }


    void addModelItem(int id, juce::String itemValue, int itemIndex) {
        if (m_state.get() == IUdpRcListener::EState::Connecting) {
            sendUdp(NpRpcProto::genAddModelMsg(m_sessionId, id, itemValue, itemIndex));
        }
    }

    void finishConfig() {
        if (m_state.get() == IUdpRcListener::EState::Connecting) {
            sendUdp(NpRpcProto::genConnectRes(m_sessionId));
            setState(IUdpRcListener::EState::Connected);
            setHbState(EHbState::Ready);
        }
    }

    void updateKnob(int id, float value) {
        if (m_state.get() == IUdpRcListener::EState::Connected) {
            sendUdp(NpRpcProto::genUpdateKnobMsg(m_sessionId, id, value));
        }
    }

    void updateModelIndex(int id, int index) {
        if (m_state.get() == IUdpRcListener::EState::Connected) {
            sendUdp(NpRpcProto::genSelectModelMsg(m_sessionId, id, index));
        }
    }

private:
    enum class EHbState {
        Idle
        , Ready
        , Wait
        , Timeout = 0xfe
    };

    inline static const std::unordered_map<EHbState, String> EHbStateNames = {
          {EHbState::Idle    , "Idle"}
        , {EHbState::Ready   , "Ready"}
        , {EHbState::Wait    , "Wait"}
        , {EHbState::Timeout , "Timeout"}
    };

    const int HEARTBEAT_PERIOD_MS{ 1000 };
    const int HEARTBEAT_MAX{ 3 };

    void setState(IUdpRcListener::EState newState) {
        IUdpRcListener::EState curState = m_state.get();
        if (newState != curState) {
            DBG("State: " << IUdpRcListener::EStateNames.at(curState) << " => " << IUdpRcListener::EStateNames.at(newState));
            m_state = newState;
            // Thread-safe UI call
            juce::MessageManager::callAsync([&updater = m_listener, curState, newState]() {
                updater.onStateChanged(curState, newState);
                });
        }
        else {
            DBG("Already in state: " << IUdpRcListener::EStateNames.at(newState));
        }
    }

    int sendTxQueue() {
        const juce::ScopedLock lock(m_udpTxLock);
        int ret = 0;
        while (m_udpTxQueue.size() > 0) {
            juce::MemoryBlock buf = m_udpTxQueue.removeAndReturn(0);
            ret += (m_udpClnSocket.write(m_clnAddr, m_clnPort, buf.getData(), static_cast<int>(buf.getSize())) < 0) ? -1 : 0;
        }
        return ret;
    }

    void sendUdp(const SimpleOscMsg& msg)
    {
        juce::MemoryBlock buf;
        msg.SerializeTo(buf);

        const juce::ScopedLock lock(m_udpTxLock);
        m_udpTxQueue.add(buf);
        notify();
    }

    void stateStep() {
        switch (m_state.get()) {
        case IUdpRcListener::EState::Connected:
            stepHbState();
            break;
        case IUdpRcListener::EState::Disconnecting:
        {
            sendUdp(NpRpcProto::genAbortReq(m_sessionId));
            // Clear current session
            m_sessionId = NpRpcProto::NPRPC_INV_SESS_ID;
            m_sessionTs = NpRpcProto::NPRPC_INV_SESS_TS;
            setState(IUdpRcListener::EState::Idle);
            break;
        }
        default:
            break;
        }
    }

    void setHbState(EHbState newState) {
        EHbState curState = m_hbState.get();
        if (newState != curState) {
            //DBG("Heartbeat: " << EHbStateNames.at(curState) << " => " << EHbStateNames.at(newState));
            m_hbState = newState;
        }
        else {
            DBG("Heartbeat: Already in state: " << EHbStateNames.at(newState));
        }
    }

    void stepHbState() {
        switch (m_hbState.get()) {
        case EHbState::Ready:
        {
            sendUdp(NpRpcProto::genHeartbeatRes(m_sessionId, Time::getMillisecondCounter() - m_sessionTs));
            m_hbMissedCount = 0;
            m_hbTimer.start(HEARTBEAT_PERIOD_MS);
            setHbState(EHbState::Wait);
        }
        break;
        case EHbState::Wait:
            if (!m_hbTimer.isValid() || m_hbTimer.IsElapsed()) {
                m_hbMissedCount++;
                if (m_hbMissedCount < HEARTBEAT_MAX) {
                    setHbState(EHbState::Ready);
                }
                else {
                    m_hbMissedCount = 0;
                    setState(IUdpRcListener::EState::Disconnecting);
                }
            }
            break;
        case EHbState::Idle:
        default:
            break;
        }
    }

private:
    int m_srvPort;
    int m_clnPort;
    juce::String m_clnAddr;
    juce::String m_mcastAddr;
    juce::DatagramSocket m_udpSrvSocket;
    juce::DatagramSocket m_udpClnSocket;

    juce::CriticalSection m_udpTxLock;
    juce::Array<juce::MemoryBlock> m_udpTxQueue;

    IUdpRcServerListener& m_listener;

    int32_t m_sessionId;
    int32_t m_sessionTs;
    juce::Atomic<IUdpRcListener::EState> m_state;
    juce::Atomic<EHbState> m_hbState;
    int m_hbSessionCount;
    int m_hbMissedCount;

    ElapsedTimer m_hbTimer;
};