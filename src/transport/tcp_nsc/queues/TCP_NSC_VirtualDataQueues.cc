//
// Copyright (C) 2004 Andras Varga
//               2009 Zoltan Bojthe
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
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifdef WITH_TCP_NSC

#include <omnetpp.h>

#include "TCP_NSC_VirtualDataQueues.h"

#include "TCPCommand_m.h"
#include "TCP_NSC_Connection.h"
#include "TCPSerializer.h"


Register_Class(TCP_NSC_VirtualDataSendQueue);

Register_Class(TCP_NSC_VirtualDataReceiveQueue);


TCP_NSC_VirtualDataSendQueue::TCP_NSC_VirtualDataSendQueue()
    :
    unsentNscBytesM(0)
{
}

TCP_NSC_VirtualDataSendQueue::~TCP_NSC_VirtualDataSendQueue()
{
}

void TCP_NSC_VirtualDataSendQueue::setConnection(TCP_NSC_Connection *connP)
{
    unsentNscBytesM = 0;
    TCP_NSC_SendQueue::setConnection(connP);
}

void TCP_NSC_VirtualDataSendQueue::enqueueAppData(cPacket *msgP)
{
    ASSERT(msgP);

    int bytes = msgP->getByteLength();
    delete msgP;

    unsentNscBytesM += bytes;
}

int TCP_NSC_VirtualDataSendQueue::getNscMsg(void* bufferP, int bufferLengthP)
{
    ASSERT(bufferP);

    return (unsentNscBytesM > bufferLengthP) ? bufferLengthP : unsentNscBytesM;
}

void TCP_NSC_VirtualDataSendQueue::dequeueNscMsg(int msgLengthP)
{
    ASSERT(msgLengthP <= unsentNscBytesM);

    unsentNscBytesM -= msgLengthP;
}

ulong TCP_NSC_VirtualDataSendQueue::getBytesAvailable()
{
    return unsentNscBytesM; // TODO
}

TCPSegment* TCP_NSC_VirtualDataSendQueue::createSegmentWithBytes(
        const void* tcpDataP, int tcpLengthP)
{
    ASSERT(tcpDataP);

    TCPSegment *tcpseg = new TCPSegment("tcp-segment");

    TCPSerializer().parse((const unsigned char *)tcpDataP, tcpLengthP, tcpseg);

    return tcpseg;
}

void TCP_NSC_VirtualDataSendQueue::discardUpTo(uint32 seqNumP)
{
    // nothing to do here
}

////////////////////////////////////////////////////////////////////////////////////////

TCP_NSC_VirtualDataReceiveQueue::TCP_NSC_VirtualDataReceiveQueue()
    :
    bytesInQueueM(0)
{
}

TCP_NSC_VirtualDataReceiveQueue::~TCP_NSC_VirtualDataReceiveQueue()
{
    // nothing to do here
}

void TCP_NSC_VirtualDataReceiveQueue::setConnection(TCP_NSC_Connection *connP)
{
    ASSERT(connP);

    bytesInQueueM = 0;
    TCP_NSC_ReceiveQueue::setConnection(connP);
}

uint32 TCP_NSC_VirtualDataReceiveQueue::insertBytesFromSegment(
        const TCPSegment *tcpsegP, void* bufferP, size_t bufferLengthP)
{
    ASSERT(tcpsegP);
    ASSERT(bufferP);

    return TCPSerializer().serialize(tcpsegP, (unsigned char *)bufferP, bufferLengthP);
}

void TCP_NSC_VirtualDataReceiveQueue::enqueueNscData(void* dataP, int dataLengthP)
{
    bytesInQueueM += dataLengthP;
}

cPacket* TCP_NSC_VirtualDataReceiveQueue::extractBytesUpTo()
{
    ASSERT(connM);

    cPacket *dataMsg = NULL;
    if(bytesInQueueM)
    {
        dataMsg = new cPacket("DATA");
        dataMsg->setKind(TCP_I_DATA);
        dataMsg->setByteLength(bytesInQueueM);
        TCPConnectInfo *tcpConnectInfo = new TCPConnectInfo();
        tcpConnectInfo->setConnId(connM->connIdM);
        tcpConnectInfo->setLocalAddr(connM->inetSockPairM.localM.ipAddrM);
        tcpConnectInfo->setRemoteAddr(connM->inetSockPairM.remoteM.ipAddrM);
        tcpConnectInfo->setLocalPort(connM->inetSockPairM.localM.portM);
        tcpConnectInfo->setRemotePort(connM->inetSockPairM.remoteM.portM);
        dataMsg->setControlInfo(tcpConnectInfo);
        bytesInQueueM -= dataMsg->getByteLength();
    }
    return dataMsg;
}

uint32 TCP_NSC_VirtualDataReceiveQueue::getAmountOfBufferedBytes()
{
    return bytesInQueueM;
}

uint32 TCP_NSC_VirtualDataReceiveQueue::getQueueLength()
{
    return bytesInQueueM ? 1 : 0;
}

void TCP_NSC_VirtualDataReceiveQueue::getQueueStatus()
{
    // TODO
}

void TCP_NSC_VirtualDataReceiveQueue::notifyAboutSending(const TCPSegment *tcpsegP)
{
    // nothing to do
}

#endif // WITH_TCP_NSC
