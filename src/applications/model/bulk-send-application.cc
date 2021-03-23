/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Georgia Institute of Technology
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: George F. Riley <riley@ece.gatech.edu>
 */

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/tcp-socket-factory.h"
#include "bulk-send-application.h"
#include "ns3/internet-module.h"
//#include "../../IntTag.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BulkSendApplication");

NS_OBJECT_ENSURE_REGISTERED (BulkSendApplication);

TypeId
BulkSendApplication::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BulkSendApplication")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<BulkSendApplication> ()
    .AddAttribute ("SendSize", "The amount of data to send each time.",
                   UintegerValue (512),
                   MakeUintegerAccessor (&BulkSendApplication::m_sendSize),
                   MakeUintegerChecker<uint32_t> (1))
    .AddAttribute ("Remote", "The address of the destination",
                   AddressValue (),
                   MakeAddressAccessor (&BulkSendApplication::m_peer),
                   MakeAddressChecker ())
    .AddAttribute ("MaxBytes",
                   "The total number of bytes to send. "
                   "Once these bytes are sent, "
                   "no data  is sent again. The value zero means "
                   "that there is no limit.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BulkSendApplication::m_maxBytes),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("DelayThresh",
                   "How many packets can pass before we have delay, 0 for disable",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BulkSendApplication::m_delayThresh),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("DelayTime",
                   "The time for a delay",
                   TimeValue (MicroSeconds (100)),
                   MakeTimeAccessor (&BulkSendApplication::m_delayTime),
                   MakeTimeChecker())
    .AddAttribute ("SimpleTOS",
                   "A simple version of TOS",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BulkSendApplication::m_tos),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("Protocol", "The type of protocol to use.",
                   TypeIdValue (TcpSocketFactory::GetTypeId ()),
                   MakeTypeIdAccessor (&BulkSendApplication::m_tid),
                   MakeTypeIdChecker ())
    .AddAttribute ("IpSource",
                   "The IP address (as an unsigned 32 bit value) of the sender.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BulkSendApplication::ip_source),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("IpDest",
                   "The IP address (as an unsigned 32 bit value) of the destination.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BulkSendApplication::ip_dest),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("SourcePort",
                   "The source port to be used",
                   UintegerValue (1),
                   MakeUintegerAccessor (&BulkSendApplication::source_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("DestPort",
                   "The port of the destination.",
                   UintegerValue (1),
                   MakeUintegerAccessor (&BulkSendApplication::dest_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("FiveTupleProt",
                   "The protocol (as an unsigned 5 bit value) as used in the 5-tuple.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&BulkSendApplication::fivetupleprot),
                   MakeUintegerChecker<uint8_t> ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&BulkSendApplication::m_txTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}


BulkSendApplication::BulkSendApplication ()
  : m_socket (0),
    m_connected (false),
    m_totBytes (0),
    m_isDelay (false),
    m_accumPackets (0),
    ip_source(0),
    ip_dest(0),
    source_port(1),
    dest_port(1),
    fivetupleprot(0)
{
  NS_LOG_FUNCTION (this);
}

BulkSendApplication::~BulkSendApplication ()
{
  NS_LOG_FUNCTION (this);
}

void
BulkSendApplication::SetMaxBytes (uint32_t maxBytes)
{
  NS_LOG_FUNCTION (this << maxBytes);
  m_maxBytes = maxBytes;
}

Ptr<Socket>
BulkSendApplication::GetSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

uint32_t
BulkSendApplication::GetIpSource (void) const
{
  NS_LOG_FUNCTION (this);
  return ip_source;
}

uint32_t
BulkSendApplication::GetIpDest (void) const
{
  NS_LOG_FUNCTION (this);
  return ip_dest;
}


uint16_t
BulkSendApplication::GetSourcePort (void) const
{
  NS_LOG_FUNCTION (this);
  return source_port;
}

uint16_t
BulkSendApplication::GetDestPort (void) const
{
  NS_LOG_FUNCTION (this);
  return dest_port;
}

uint32_t
BulkSendApplication::GetFivetupleProt (void) const
{
  NS_LOG_FUNCTION (this);
  return fivetupleprot;
}

void
BulkSendApplication::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

// Application Methods
void BulkSendApplication::StartApplication (void) // Called at time specified by Start
{
  NS_LOG_FUNCTION (this);

  // Create the socket if not already
  if (!m_socket)
    {
      m_socket = Socket::CreateSocket (GetNode (), m_tid);

      // Fatal error if socket type is not NS3_SOCK_STREAM or NS3_SOCK_SEQPACKET
      if (m_socket->GetSocketType () != Socket::NS3_SOCK_STREAM &&
          m_socket->GetSocketType () != Socket::NS3_SOCK_SEQPACKET)
        {
          NS_FATAL_ERROR ("Using BulkSend with an incompatible socket type. "
                          "BulkSend requires SOCK_STREAM or SOCK_SEQPACKET. "
                          "In other words, use TCP instead of UDP.");
        }

      if (Inet6SocketAddress::IsMatchingType (m_peer))
        {
          m_socket->Bind6 ();
        }
      else if (InetSocketAddress::IsMatchingType (m_peer))
        {
          m_socket->Bind ();
        }

      m_socket->Connect (m_peer);
      m_socket->ShutdownRecv ();
      m_socket->SetConnectCallback (
        MakeCallback (&BulkSendApplication::ConnectionSucceeded, this),
        MakeCallback (&BulkSendApplication::ConnectionFailed, this));
      m_socket->SetSendCallback (
        MakeCallback (&BulkSendApplication::DataSend, this));
    }
  if (m_connected)
  {
    SendData ();
  }
}

void BulkSendApplication::StopApplication (void) // Called at time specified by Stop
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      m_socket->Close ();
      m_connected = false;
    }
  else
    {
      NS_LOG_WARN ("BulkSendApplication found null socket to close in StopApplication");
    }
}


// Private helpers

void BulkSendApplication::SendData (void)
{
  NS_LOG_FUNCTION (this);

  while (m_maxBytes == 0 || m_totBytes < m_maxBytes)
    {
      if (m_isDelay)
        {
          break;
        }

      // Time to send more
      uint32_t toSend = m_sendSize;
      // Make sure we don't send too many
      if (m_maxBytes > 0)
        {
          toSend = std::min (m_sendSize, m_maxBytes - m_totBytes);
        }
      NS_LOG_LOGIC ("sending packet at " << Simulator::Now ());
      
      Ptr<Packet> packet = Create<Packet> (toSend);

      uint32_t uid = packet->GetUid ();
      uint32_t psize = packet->GetSize();

      // Node on which the BulkSendApplication is being run
      Ptr<Node> this_node = GetNode();
      

      NS_LOG_INFO("BulkSendApplication sending packet with uid "<< uid << " and size " << psize 
                  << " from " << Ipv4Address(GetIpSource()) << " to " << Ipv4Address(GetIpDest()) 
                  << " with protocol " << GetFivetupleProt());
      
      
      // Add the IntTag
      MainIntTag tag;
      NS_ASSERT(tag.GetMode() == 0 && tag.GetNEntries() == 0 && tag.FiveTupleUnInitialized() && tag.GetCrc1() == 0 && tag.GetCrc2() == 0);
      tag.SetMode(49721);
      tag.SetFiveTuple(GetIpSource(), GetIpDest(), /*(source port)*/ GetSourcePort(), /*(destination port)*/ GetDestPort(), GetFivetupleProt());
      NS_ASSERT(tag.IsModePitcher());
      //ns3::five_tuple_t five_tuple = tag.GetFiveTuple();
      //NS_ASSERT(tag.GetMode() == 49721 && tag.GetNEntries() == 36085 && five_tuple.source_ip == GetIpSource() && 
      //  five_tuple.dest_ip == GetIpDest() && five_tuple.source_port == 1 && five_tuple.dest_port == 1 && five_tuple.protocol == GetFivetupleProt());
      packet->AddPacketTag (tag);
      
      // verification of the tag
      //uint32_t psize1 = packet->GetSize();
      //NS_ASSERT(psize1 == psize);
      //MainIntTag tag_check;
      //NS_ASSERT(tag_check.GetMode() == 0 && tag_check.GetNEntries() == 0 && tag_check.FiveTupleUnInitialized() && 
      //          tag_check.GetCrc1() == 0 && tag_check.GetCrc2() == 0);
      //packet->PeekPacketTag(tag_check);
      //ns3::five_tuple_t five_tuple_check = tag_check.GetFiveTuple();
      //NS_ASSERT(tag_check.GetMode() == 49721 && tag_check.GetNEntries() == 36085 && tag_check.GetCrc1() == 0 && tag_check.GetCrc2() == 0 && 
      //  five_tuple_check.source_ip == GetIpSource() && five_tuple_check.dest_ip == GetIpDest() && five_tuple_check.source_port == 1 && 
      // five_tuple_check.dest_port == 1 && five_tuple_check.protocol == GetFivetupleProt());

      
      SocketIpTosTag tosTag;
      tosTag.SetTos (m_tos << 2);
      packet->AddPacketTag (tosTag);
      m_txTrace (packet);
      int actual = m_socket->Send (packet);
      if (actual > 0)
        {
          m_totBytes += actual;
          m_accumPackets ++;
        }

      // We exit this loop when actual < toSend as the send side
      // buffer is full. The "DataSent" callback will pop when
      // some buffer space has freed ip.
      if ((unsigned)actual != toSend)
        {
          break;
        }

      if (m_delayThresh != 0 && m_accumPackets > m_delayThresh)
        {
          m_isDelay = true;
          Simulator::Schedule (m_delayTime, &BulkSendApplication::ResumeSend, this);
        }
    }
  // Check if time to close (all sent)
  if (m_totBytes == m_maxBytes && m_connected)
    {
      m_socket->Close ();
      m_connected = false;
    }
}

void BulkSendApplication::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_LOGIC ("BulkSendApplication Connection succeeded");
  m_connected = true;
  SendData ();
}

void BulkSendApplication::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_LOGIC ("BulkSendApplication, Connection Failed");
}

void BulkSendApplication::DataSend (Ptr<Socket>, uint32_t)
{
  NS_LOG_FUNCTION (this);

  if (m_connected)
    { // Only send new data if the connection has completed
      SendData ();
    }
}

void BulkSendApplication::ResumeSend (void)
{
    NS_LOG_FUNCTION (this);

    m_isDelay = false;
    m_accumPackets = 0;

    if (m_connected)
    {
        SendData ();
    }
}

} // Namespace ns3
