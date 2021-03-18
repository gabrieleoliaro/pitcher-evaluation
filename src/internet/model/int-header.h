/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */


#ifndef INT_HEADER_H
#define INT_HEADER_H

#include <stdint.h>
//#include <string>
#include "ns3/header.h"
//#include "ns3/ipv4-address.h"
//#include "ns3/ipv6-address.h"

namespace ns3 {

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

  void SetMode (uint16_t mode);
  void SetNEntries (uint16_t n_entries);
  
  uint16_t GetMode (void) const;
  uint16_t GetNEntries (void) const;
  

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);


private:
  
  uint16_t mode_;      
  uint16_t n_entries_; 
};

} // namespace ns3

#endif /* INT_HEADER */
