#include "IntTag.h"

namespace ns3 {

MainIntTag::MainIntTag ()
  : mode_ (0), n_entries_(0),
    crc1_(0), crc2_(0) {
    memset(&five_tuple_, 0, sizeof(five_tuple_t));
}

TypeId MainIntTag::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::MainIntTag")
    .SetParent<Tag> ()
    .AddConstructor<MainIntTag> ();
  return tid;
}

TypeId MainIntTag::GetInstanceTypeId (void) const {
  return GetTypeId ();
}

uint32_t MainIntTag::GetSerializedSize (void) const {
  //return 4*sizeof(uint16_t) + sizeof(five_tuple_t);
  return 8+13;
}

void MainIntTag::Serialize (TagBuffer i) const {

  i.WriteU32 (five_tuple_.source_ip);
  i.WriteU32 (five_tuple_.dest_ip);
  i.WriteU16 (mode_);
  i.WriteU16 (n_entries_);
  i.WriteU16 (crc1_);
  i.WriteU16 (crc2_);
  i.WriteU16 (five_tuple_.source_port);
  i.WriteU16 (five_tuple_.dest_port);
  i.WriteU8 (five_tuple_.protocol);

}

void MainIntTag::Deserialize (TagBuffer i) {

  five_tuple_.source_ip = i.ReadU32 ();
  five_tuple_.dest_ip = i.ReadU32();

  mode_ = i.ReadU16 ();
  n_entries_ = i.ReadU16 ();
  crc1_ = i.ReadU16 ();
  crc2_ = i.ReadU16 ();

  five_tuple_.source_port = i.ReadU16 ();
  five_tuple_.dest_port = i.ReadU16 ();
  
  five_tuple_.protocol = i.ReadU8 ();

}

void MainIntTag::Print (std::ostream &os) const {
  os  << "Mode: " << mode_ << ", NEntries: " << n_entries_
      << "Five Tuple: (" 
                  << five_tuple_.source_ip << ", "
                  << five_tuple_.dest_ip << ", "
                  << five_tuple_.source_port << ", "
                  << five_tuple_.dest_port << ", "
                  << five_tuple_.protocol << ") "
    << "CRCs: (" 
                  << crc1_ << ", "
                  << crc2_ << ") ";
}

uint16_t MainIntTag::GetMode (void) const {
  return mode_;
}

uint16_t MainIntTag::GetNEntries (void) const {
  return n_entries_;
}

five_tuple_t MainIntTag::GetFiveTuple (void) const {
  return five_tuple_;
}

uint16_t MainIntTag::GetCrc1(void) const {
  return crc1_;
}
uint16_t MainIntTag::GetCrc2(void) const {
  return crc2_;
}

bool MainIntTag::FiveTupleUnInitialized(void) const {
  return (five_tuple_.source_ip == 0 && 
          five_tuple_.dest_ip == 0 &&
          five_tuple_.source_port == 0 &&
          five_tuple_.dest_port == 0 &&
          five_tuple_.protocol == 0);
}

bool MainIntTag::IsModePitcher(void) const {
  return (mode_ == 49721);
}

void MainIntTag::SetMode (uint16_t mode) {
  mode_ = mode;
}

void MainIntTag::SetNEntries (uint16_t n_entries) {
  n_entries_ = n_entries;
}

void MainIntTag::SetFiveTuple(uint32_t source_ip, uint32_t dest_ip,
                  uint16_t source_port, uint16_t dest_port,
                  uint8_t protocol) {
  five_tuple_.source_ip = source_ip;
  five_tuple_.dest_ip = dest_ip;
  five_tuple_.source_port = source_port;
  five_tuple_.dest_port = dest_port;
  five_tuple_.protocol = protocol;
}

void MainIntTag::SetCrc1(uint16_t crc1) {
  crc1_ = crc1;
}

void MainIntTag::SetCrc2(uint16_t crc2) {
  crc2_ = crc2;
}



//##############################################################



IntTag1::IntTag1 ()
  : mode_ (0), hop_latency_(0), switch_id_(0) {
}

TypeId IntTag1::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::IntTag1")
    .SetParent<Tag> ()
    .AddConstructor<IntTag1> ();
  return tid;
}

TypeId IntTag1::GetInstanceTypeId (void) const {
  return GetTypeId ();
}

uint32_t IntTag1::GetSerializedSize (void) const {
  return (16+32+32)/8;
}

void IntTag1::Serialize (TagBuffer i) const {
  i.WriteU32 (hop_latency_);
  i.WriteU32 (switch_id_);
  i.WriteU16 (mode_);
}

void IntTag1::Deserialize (TagBuffer i) {
  hop_latency_ = i.ReadU32 ();
  switch_id_ = i.ReadU32 ();
  mode_ = i.ReadU16 (); 
}

void IntTag1::Print (std::ostream &os) const {
  os << "Mode: " << mode_ << ", SwitchId: " << switch_id_ << ", HopLatency: " << hop_latency_;
}

uint16_t IntTag1::GetMode (void) const {
  return mode_;
}

uint32_t IntTag1::GetSwitchId (void) const {
  return switch_id_;
}

uint32_t IntTag1::GetHopLatency (void) const {
  return hop_latency_;
}

bool IntTag1::IsModePitcher(void) const {
  return (mode_ == 49721);
}

void IntTag1::SetMode (uint16_t mode) {
  mode_ = mode;
}

void IntTag1::SetHopLatency (uint32_t hop_latency) {
  hop_latency_ = hop_latency;
}

void IntTag1::SetSwitchId (uint32_t switch_id) {
  switch_id_ = switch_id;
}



//##############################################################



IntTag2::IntTag2 ()
  : mode_ (0), hop_latency_(0), switch_id_(0) {
}

TypeId IntTag2::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::IntTag2")
    .SetParent<Tag> ()
    .AddConstructor<IntTag2> ();
  return tid;
}

TypeId IntTag2::GetInstanceTypeId (void) const {
  return GetTypeId ();
}

uint32_t IntTag2::GetSerializedSize (void) const {
  return (16+32+32)/8;
}

void IntTag2::Serialize (TagBuffer i) const {
  i.WriteU32 (hop_latency_);
  i.WriteU32 (switch_id_);
  i.WriteU16 (mode_);
}

void IntTag2::Deserialize (TagBuffer i) {
  hop_latency_ = i.ReadU32 ();
  switch_id_ = i.ReadU32 ();
  mode_ = i.ReadU16 (); 
}

void IntTag2::Print (std::ostream &os) const {
  os << "Mode: " << mode_ << ", SwitchId: " << switch_id_ << ", HopLatency: " << hop_latency_;
}

uint16_t IntTag2::GetMode (void) const {
  return mode_;
}

uint32_t IntTag2::GetSwitchId (void) const {
  return switch_id_;
}

uint32_t IntTag2::GetHopLatency (void) const {
  return hop_latency_;
}

bool IntTag2::IsModePitcher(void) const {
  return (mode_ == 49721);
}

void IntTag2::SetMode (uint16_t mode) {
  mode_ = mode;
}

void IntTag2::SetHopLatency (uint32_t hop_latency) {
  hop_latency_ = hop_latency;
}

void IntTag2::SetSwitchId (uint32_t switch_id) {
  switch_id_ = switch_id;
}





//##############################################################




IntTag3::IntTag3 ()
  : mode_ (0), hop_latency_(0), switch_id_(0) {
}

TypeId IntTag3::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::IntTag3")
    .SetParent<Tag> ()
    .AddConstructor<IntTag3> ();
  return tid;
}

TypeId IntTag3::GetInstanceTypeId (void) const {
  return GetTypeId ();
}

uint32_t IntTag3::GetSerializedSize (void) const {
  return (16+32+32)/8;
}

void IntTag3::Serialize (TagBuffer i) const {
  i.WriteU32 (hop_latency_);
  i.WriteU32 (switch_id_);
  i.WriteU16 (mode_);
}

void IntTag3::Deserialize (TagBuffer i) {
  hop_latency_ = i.ReadU32 ();
  switch_id_ = i.ReadU32 ();
  mode_ = i.ReadU16 (); 
}

void IntTag3::Print (std::ostream &os) const {
  os << "Mode: " << mode_ << ", SwitchId: " << switch_id_ << ", HopLatency: " << hop_latency_;
}

uint16_t IntTag3::GetMode (void) const {
  return mode_;
}

uint32_t IntTag3::GetSwitchId (void) const {
  return switch_id_;
}

uint32_t IntTag3::GetHopLatency (void) const {
  return hop_latency_;
}

bool IntTag3::IsModePitcher(void) const {
  return (mode_ == 49721);
}

void IntTag3::SetMode (uint16_t mode) {
  mode_ = mode;
}

void IntTag3::SetHopLatency (uint32_t hop_latency) {
  hop_latency_ = hop_latency;
}

void IntTag3::SetSwitchId (uint32_t switch_id) {
  switch_id_ = switch_id;
}



// #######################################################################



MainIntTag CloneMainIntTag(MainIntTag old_tag) {
  MainIntTag new_tag;
  new_tag.SetMode(old_tag.GetMode());
  new_tag.SetNEntries(old_tag.GetNEntries());
  new_tag.SetCrc1(old_tag.GetCrc1());
  new_tag.SetCrc2(old_tag.GetCrc2());

  ns3::five_tuple_t old_5_tuple = old_tag.GetFiveTuple();
  new_tag.SetFiveTuple(old_5_tuple.source_ip, old_5_tuple.dest_ip, old_5_tuple.source_port, old_5_tuple.dest_port, old_5_tuple.protocol);

  return new_tag;
}


IntTag1 CloneIntTag1(IntTag1 old_tag) {
  IntTag1 new_tag;
  new_tag.SetMode(old_tag.GetMode());
  new_tag.SetHopLatency(old_tag.GetHopLatency());
  new_tag.SetSwitchId(old_tag.GetSwitchId());
  return new_tag;
}

IntTag2 CloneIntTag2(IntTag2 old_tag) {
  IntTag2 new_tag;
  new_tag.SetMode(old_tag.GetMode());
  new_tag.SetHopLatency(old_tag.GetHopLatency());
  new_tag.SetSwitchId(old_tag.GetSwitchId());
  return new_tag;
}
IntTag3 CloneIntTag1(IntTag3 old_tag) { 
  IntTag3 new_tag;
  new_tag.SetMode(old_tag.GetMode());
  new_tag.SetHopLatency(old_tag.GetHopLatency());
  new_tag.SetSwitchId(old_tag.GetSwitchId());
  return new_tag;

}


}