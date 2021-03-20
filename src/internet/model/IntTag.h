#ifndef INT_TAG_H
#define INT_TAG_H

#include <stdint.h>

#include "ns3/tag.h"

namespace ns3 {

typedef struct {
  uint32_t source_ip;
  uint32_t dest_ip;
  uint16_t source_port;
  uint16_t dest_port;
  uint8_t protocol;
} five_tuple_t;


class MainIntTag : public Tag
{
public:
  MainIntTag ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  uint16_t GetMode (void) const;
  uint16_t GetNEntries (void) const;
  five_tuple_t GetFiveTuple (void) const;
  uint16_t GetCrc1(void) const;
  uint16_t GetCrc2(void) const;

  bool FiveTupleUnInitialized(void) const;
  bool IntInfoEntryUnInitialized(unsigned int index) const;

  void SetMode(uint16_t mode);
  void SetNEntries (uint16_t n_entries);
  void SetFiveTuple(uint32_t source_ip, 
                    uint32_t dest_ip,
                    uint16_t source_port,
                    uint16_t dest_port,
                    uint8_t protocol);
  void SetCrc1(uint16_t crc1);
  void SetCrc2(uint16_t crc2);

private:
  uint16_t mode_;
  uint16_t n_entries_; 

  five_tuple_t five_tuple_;

  uint16_t crc1_;
  uint16_t crc2_;

};



}
#endif


// class IntTag1 : public Tag
// {
// public:
//   IntTag1 ();

//   static TypeId GetTypeId (void);
//   virtual TypeId GetInstanceTypeId (void) const;

//   virtual uint32_t GetSerializedSize (void) const;
//   virtual void Serialize (TagBuffer i) const;
//   virtual void Deserialize (TagBuffer i);
//   virtual void Print (std::ostream &os) const;

//   uint16_t GetMode (void) const;
//   uint32_t GetHopLatency (void) const;
//   uint32_t GetSwitchId (void) const;


//   void SetMode(uint16_t mode);
//   void SetHopLatency (uint32_t hop_latency);
//   void SetSwitchId (uint32_t switch_id);
  

// private:
//   uint16_t mode_;
  
//   uint32_t hop_latency_;
//   uint32_t switch_id_;

// };

// IntTag1::IntTag1 ()
//   : mode_ (0),
//     hop_latency_(0),
//     switch_id_(0)
//     {}

// TypeId IntTag1::GetTypeId (void) {
//   static TypeId tid = TypeId ("ns3::IntTag1")
//     .SetParent<Tag> ()
//     .AddConstructor<IntTag1> ();
//     // .AddAttribute ("mode_", "The mode of the INT header.",
//     //   UintegerValue (0),
//     //   MakeUintegerAccessor (&IntTag1::mode_),
//     //   MakeUintegerChecker<uint16_t> ())
//     // .AddAttribute ("n_entries_", "The number of entries in the tag.",
//     //   UintegerValue (0),
//     //   MakeUintegerAccessor (&IntTag1::n_entries_),
//     //   MakeUintegerChecker<uint16_t> ());
//   return tid;
// }

// TypeId IntTag1::GetInstanceTypeId (void) const {
//   return GetTypeId ();
// }

// uint32_t IntTag1::GetSerializedSize (void) const {
//   return (16+32+32)/8;
// }

// void IntTag1::Serialize (TagBuffer i) const {
//   i.WriteU32 (hop_latency_);
//   i.WriteU32 (switch_id_);
//   i.WriteU16 (mode_);
// }

// void IntTag1::Deserialize (TagBuffer i) {
//   hop_latency_ = i.ReadU32 ();
//   switch_id_ = i.ReadU32 ();
//   mode_ = i.ReadU16 (); 
// }

// void IntTag1::Print (std::ostream &os) const {
//   os << "Mode: " << mode_ << ", SwitchId: " << switch_id_ << ", HopLatency: " << hop_latency_;
// }

// uint16_t IntTag1::GetMode (void) const {
//   return mode_;
// }

// uint32_t IntTag1::GetSwitchId (void) const {
//   return switch_id_;
// }

// uint32_t IntTag1::GetHopLatency (void) const {
//   return hop_latency_;
// }

// void IntTag1::SetMode (uint16_t mode) {
//   mode_ = mode;
// }

// void IntTag1::SetHopLatency (uint32_t hop_latency) {
//   hop_latency_ = hop_latency;
// }

// void IntTag1::SetSwitchId (uint32_t switch_id) {
//   switch_id_ = switch_id;
// }

// }

