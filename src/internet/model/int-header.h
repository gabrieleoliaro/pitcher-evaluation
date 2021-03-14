/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */

#ifndef INT_HEADER_H
#define INT_HEADER_H

#include <stdint.h>
#include <string>
#include <assert.h>
#include "ns3/header.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"

namespace ns3 {
/**
 * \ingroup udp
 * \brief Packet header for UDP packets
 *
 * This class has fields corresponding to those in a network UDP header
 * (port numbers, payload size, checksum) as well as methods for serialization
 * to and deserialization from a byte buffer.
 */

#define MAX_HOPS 5
#define INT_MODE 784591620
#define PITCHER_MODE 234569283
  
typedef struct {
  int canary_flag;
  uint32_t swid;
  uint32_t sojourn_time;
  uint16_t crc1;
  uint16_t crc2;
} int_info;

class IntHeader : public Header 
{
public:

  /**
   * \brief Constructor
   *
   * Creates a null header
   */
  IntHeader ();
  ~IntHeader ();

 
  void SetNEntries(int n_entries);
  
  void SetMode (int mode);

  int GetNEntries(void) const;

  int GetMode(void) const;

  bool CheckIfEntryExists(int index) const;

  bool GetEntryAtIndex (int index, int_info *dst) const;

  int AddEntry(int canary_flag, uint32_t swid, uint32_t sojourn_time, uint16_t crc1, uint16_t crc2);

  
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);


private:

  int_info int_stuff_[MAX_HOPS];

  int n_entries_;
  int mode_;

};

} // namespace ns3

#endif /* INT_HEADER */
