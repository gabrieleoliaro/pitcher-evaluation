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
    // .AddAttribute ("mode_", "The mode of the INT header.",
    //   UintegerValue (0),
    //   MakeUintegerAccessor (&MainIntTag::mode_),
    //   MakeUintegerChecker<uint16_t> ())
    // .AddAttribute ("n_entries_", "The number of entries in the tag.",
    //   UintegerValue (0),
    //   MakeUintegerAccessor (&MainIntTag::n_entries_),
    //   MakeUintegerChecker<uint16_t> ());
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

}