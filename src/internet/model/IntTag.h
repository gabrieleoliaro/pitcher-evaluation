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

typedef struct {
  uint32_t source_ip;
  uint32_t dest_ip;
  uint16_t source_port;
  uint16_t dest_port;
  uint8_t protocol;
  uint16_t crc1;
  uint16_t crc2;
} network_event_t;


// A method to compare two network_event_t elements
bool NewEventOccurred(network_event_t old_event, network_event_t new_event);

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
  bool IsModePitcher(void) const;

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


// ################################################################


class IntTag1 : public Tag
{
public:
  IntTag1 ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  uint16_t GetMode (void) const;
  uint32_t GetHopLatency (void) const;
  uint32_t GetSwitchId (void) const;

  bool IsModePitcher(void) const;

  void SetMode(uint16_t mode);
  void SetHopLatency (uint32_t hop_latency);
  void SetSwitchId (uint32_t switch_id);
  

private:
  uint16_t mode_;
  
  uint32_t hop_latency_;
  uint32_t switch_id_;
};


// ################################################################


class IntTag2 : public Tag
{
public:
  IntTag2 ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  uint16_t GetMode (void) const;
  uint32_t GetHopLatency (void) const;
  uint32_t GetSwitchId (void) const;

  bool IsModePitcher(void) const;

  void SetMode(uint16_t mode);
  void SetHopLatency (uint32_t hop_latency);
  void SetSwitchId (uint32_t switch_id);
  

private:
  uint16_t mode_;
  
  uint32_t hop_latency_;
  uint32_t switch_id_;
};


// ################################################################


class IntTag3 : public Tag
{
public:
  IntTag3 ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  uint16_t GetMode (void) const;
  uint32_t GetHopLatency (void) const;
  uint32_t GetSwitchId (void) const;

  bool IsModePitcher(void) const;

  void SetMode(uint16_t mode);
  void SetHopLatency (uint32_t hop_latency);
  void SetSwitchId (uint32_t switch_id);
  

private:
  uint16_t mode_;
  
  uint32_t hop_latency_;
  uint32_t switch_id_;
};



// ##############################################################


MainIntTag CloneMainIntTag(MainIntTag old_tag);
IntTag1 CloneIntTag1(IntTag1 old_tag);
IntTag2 CloneIntTag2(IntTag2 old_tag);
IntTag3 CloneIntTag1(IntTag3 old_tag);

}
#endif


// }

