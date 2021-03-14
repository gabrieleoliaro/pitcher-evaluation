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

#include "int-header.h"
#include "ns3/address-utils.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (IntHeader);

/* The magic values below are used only for debugging.
 * They can be used to easily detect memory corruption
 * problems so you can see the patterns in memory.
 */
IntHeader::IntHeader ()
  : n_entries_ (0),
    mode_ (0) {
  memset(int_stuff_, 0, sizeof(int_info) * MAX_HOPS);

}

IntHeader::~IntHeader () {
}

void IntHeader::SetNEntries (int n_nentries) {
  n_entries_ = n_nentries;
}

void IntHeader::SetMode (int mode) {
  mode_ = mode;
}

int IntHeader::GetNEntries(void) const {
  return n_entries_;
}

int IntHeader::GetMode(void) const {
  return mode_;
}

bool IntHeader::CheckIfEntryExists(int index) const {
  if (index >= n_entries_) return false;
  assert(int_stuff_[index].canary_flag == INT_MODE || int_stuff_[index].canary_flag == PITCHER_MODE);
  return true;
}

bool IntHeader::GetEntryAtIndex (int index, int_info *dst) const{
  if (CheckIfEntryExists(index)) {
    memcpy(dst, &int_stuff_[index], sizeof(int_info));
    return true;
  }
  return false;
}

int IntHeader::AddEntry(int canary_flag, uint32_t swid, uint32_t sojourn_time, uint16_t crc1, uint16_t crc2) {
  if (n_entries_ == MAX_HOPS) {
    return -1;
  }

  assert(!CheckIfEntryExists(n_entries_));

  int_stuff_[n_entries_].canary_flag = canary_flag;
  int_stuff_[n_entries_].swid = swid;
  int_stuff_[n_entries_].sojourn_time = sojourn_time;
  int_stuff_[n_entries_].crc1 = crc1;
  int_stuff_[n_entries_].crc2 = crc2;

  n_entries_++;
  return n_entries_-1; // returns the index of the newly-inserted entry
}


TypeId IntHeader::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::IntHeader")
    .SetParent<Header> ()
    .AddConstructor<IntHeader> ()
  ;
  return tid;
}

TypeId IntHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void IntHeader::Print (std::ostream &os) const {

  std::string mode_str;
  if (mode_ == PITCHER_MODE) {
    mode_str = "PITCHER";
  } else if (mode_ == INT_MODE) {
    mode_str = "INT";
  } else {
    mode_str = "INVALID";
  }
  

  os << "mode_: " << mode_str << " " << "n_entries_: " << n_entries_;

  for (int i=0; i<MAX_HOPS; i++) {
    os << "Entry " << i << ": canary_flag=" << int_stuff_[i].canary_flag 
    << " swid=" << int_stuff_[i].swid << " sojourn_time=" << int_stuff_[i].sojourn_time
    << " crc1=" << int_stuff_[i].crc1 << " crc2=" << int_stuff_[i].crc2;
  }
}

uint32_t IntHeader::GetSerializedSize (void) const {
  return sizeof(int_info)*MAX_HOPS + sizeof(uint16_t)*2;
}

void IntHeader::Serialize (Buffer::Iterator start) const {
  Buffer::Iterator i = start;

  i.WriteHtonU16 (n_entries_);
  i.WriteHtonU16 (mode_);
  for (int index=0; index < MAX_HOPS; index++) {
    if (index >= n_entries_) {
      assert( int_stuff_[index].canary_flag == 0 && 
              int_stuff_[index].swid == 0 &&
              int_stuff_[index].sojourn_time == 0 &&
              int_stuff_[index].crc1 == 0 &&
              int_stuff_[index].crc2 == 0);
    }
    i.WriteHtonU32(int_stuff_[index].canary_flag);
    i.WriteHtonU32(int_stuff_[index].swid);
    i.WriteHtonU32(int_stuff_[index].sojourn_time);
    i.WriteHtonU16(int_stuff_[index].crc1);
    i.WriteHtonU16(int_stuff_[index].crc2);
  }
}

uint32_t IntHeader::Deserialize (Buffer::Iterator start) {
  Buffer::Iterator i = start;
  n_entries_ = i.ReadNtohU16 ();
  mode_ = i.ReadNtohU16 ();

  for (int index=0; index < MAX_HOPS; index++) {
    int_stuff_[index].canary_flag = i.ReadNtohU32();
    int_stuff_[index].swid = i.ReadNtohU32();
    int_stuff_[index].sojourn_time = i.ReadNtohU32();
    int_stuff_[index].crc1 = i.ReadNtohU16();
    int_stuff_[index].crc2 = i.ReadNtohU16();
    if (index >= n_entries_ && mode_ == PITCHER_MODE) {
      assert( int_stuff_[index].canary_flag == 0 && 
              int_stuff_[index].swid == 0 &&
              int_stuff_[index].sojourn_time == 0 &&
              int_stuff_[index].crc1 == 0 &&
              int_stuff_[index].crc2 == 0);
    }
  }

  return GetSerializedSize ();
}

} // namespace ns3
