/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "int-header.h"
//#include "ns3/address-utils.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (IntHeader);

/* The magic values below are used only for debugging.
 * They can be used to easily detect memory corruption
 * problems so you can see the patterns in memory.
 */
IntHeader::IntHeader ()
  : mode_ (0),
    n_entries_ (0) {
}

IntHeader::~IntHeader () {
  
}

void  IntHeader::SetMode (uint16_t mode) {
  mode_ = mode;
}

void IntHeader::SetNEntries (uint16_t n_entries) {
  n_entries_ = n_entries;
}

uint16_t IntHeader::GetMode (void) const {
  return mode_;
}

uint16_t IntHeader::GetNEntries (void) const {
  return n_entries_;
}

// uint16_t
// IntHeader::CalculateHeaderChecksum (uint16_t size) const
// {
//   Buffer buf = Buffer ((2 * Address::MAX_SIZE) + 8);
//   buf.AddAtStart ((2 * Address::MAX_SIZE) + 8);
//   Buffer::Iterator it = buf.Begin ();
//   uint32_t hdrSize = 0;

//   WriteTo (it, m_source);
//   WriteTo (it, m_destination);
//   if (Ipv4Address::IsMatchingType (m_source))
//     {
//       it.WriteU8 (0); /* protocol */
//       it.WriteU8 (m_protocol); /* protocol */
//       it.WriteU8 (size >> 8); /* length */
//       it.WriteU8 (size & 0xff); /* length */
//       hdrSize = 12;
//     }
//   else if (Ipv6Address::IsMatchingType (m_source))
//     {
//       it.WriteU16 (0);
//       it.WriteU8 (size >> 8); /* length */
//       it.WriteU8 (size & 0xff); /* length */
//       it.WriteU16 (0);
//       it.WriteU8 (0);
//       it.WriteU8 (m_protocol); /* protocol */
//       hdrSize = 40;
//     }

//   it = buf.Begin ();
//   /* we don't CompleteChecksum ( ~ ) now */
//   return ~(it.CalculateIpChecksum (hdrSize));
// }

TypeId IntHeader::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::IntHeader")
    .SetParent<Header> ()
    .SetGroupName ("Internet")
    .AddConstructor<IntHeader> ();
  return tid;
}

TypeId IntHeader::GetInstanceTypeId (void) const {
  return GetTypeId ();
}

void IntHeader::Print (std::ostream &os) const {
  //os << "length: " << m_payloadSize + GetSerializedSize () << " " << m_sourcePort << " > " << m_destinationPort ;
  os << "Paylod with length: " << GetSerializedSize() << ". Mode: " << mode_ << ", NEntries: " << n_entries_;
}

uint32_t IntHeader::GetSerializedSize (void) const {
  // Two 16bit fields = 32 bits = 32/8 = 4 bytes
  return 4;
}

void IntHeader::Serialize (Buffer::Iterator start) const {
  Buffer::Iterator i = start;
  i.WriteHtonU16 (mode_);
  i.WriteHtonU16 (n_entries_);
}

uint32_t IntHeader::Deserialize (Buffer::Iterator start) {
  Buffer::Iterator i = start;
  mode_ = i.ReadNtohU16 ();
  return GetSerializedSize ();
}

} // namespace ns3
