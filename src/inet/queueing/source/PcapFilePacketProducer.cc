//
// Copyright (C) OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see http://www.gnu.org/licenses/.
//

#include "inet/common/ModuleAccess.h"
#include "inet/common/Simsignals.h"
#include "inet/queueing/source/PcapFilePacketProducer.h"

namespace inet {
namespace queueing {

Define_Module(PcapFilePacketProducer);

void PcapFilePacketProducer::initialize(int stage)
{
    ActivePacketSourceBase::initialize(stage);
    if (stage == INITSTAGE_LOCAL)
        pcapReader.openPcap(par("filename"), par("packetNameFormat"));
    else if (stage == INITSTAGE_QUEUEING)
        schedulePacket();
}

void PcapFilePacketProducer::finish()
{
    if (pcapReader.isOpen())
        pcapReader.closePcap();
}

void PcapFilePacketProducer::handleMessage(cMessage *message)
{
    if (message->isPacket()) {
        auto packet = check_and_cast<Packet *>(message);
        if (consumer == nullptr || consumer->canPushPacket(packet, outputGate->getPathEndGate())) {
            pushOrSendPacket(packet, outputGate, consumer);
            schedulePacket();
        }
    }
    else
        throw cRuntimeError("Unknown message: %s", message->getFullName());
}

void PcapFilePacketProducer::schedulePacket()
{
    auto pair = pcapReader.readPacket();
    auto packet = pair.second;
    emit(packetCreatedSignal, packet);
    if (packet != nullptr) {
        EV << "Scheduling packet " << packet->getFullName() << " from PCAP file.\n";
        scheduleAt(pair.first, packet);
    }
    else
        EV << "End of PCAP file reached.\n";
}

void PcapFilePacketProducer::handleCanPushPacketChanged(cGate *gate)
{
    Enter_Method("handleCanPushPacketChanged");
    if (gate == outputGate)
        schedulePacket();
}

} // namespace queueing
} // namespace inet

