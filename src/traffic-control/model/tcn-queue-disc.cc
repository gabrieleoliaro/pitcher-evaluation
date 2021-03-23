#include "tcn-queue-disc.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/ipv4-queue-disc-item.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/internet-module.h"

#define DEFAULT_TCN_LIMIT 100
//#define HOP_LATENCY_THRESHOLD 0 // divide hop latency values by 2^7=128 ns


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TCNQueueDisc");

class TCNTimestampTag : public Tag
{
public:
    TCNTimestampTag ();

    static TypeId GetTypeId (void);
    virtual TypeId GetInstanceTypeId (void) const;

    virtual uint32_t GetSerializedSize (void) const;
    virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
    virtual void Print (std::ostream &os) const;

  /**
   * Gets the Tag creation time
   * @return the time object stored in the tag
   */
  Time GetTxTime (void) const;

private:
  uint64_t m_creationTime; //!< Tag creation time
};

TCNTimestampTag::TCNTimestampTag ()
  : m_creationTime (Simulator::Now ().GetTimeStep ()) {
}

TypeId TCNTimestampTag::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::TCNTimestampTag")
    .SetParent<Tag> ()
    .AddConstructor<TCNTimestampTag> ()
    .AddAttribute ("CreationTime",
       "The time at which the timestamp was created",
       StringValue ("0.0s"),
       MakeTimeAccessor (&TCNTimestampTag::GetTxTime),
       MakeTimeChecker ()) ;
  return tid;
}

TypeId TCNTimestampTag::GetInstanceTypeId (void) const {
  return GetTypeId ();
}

uint32_t TCNTimestampTag::GetSerializedSize (void) const {
  return 8;
}

void TCNTimestampTag::Serialize (TagBuffer i) const {
  i.WriteU64 (m_creationTime);
}

void TCNTimestampTag::Deserialize (TagBuffer i) {
  m_creationTime = i.ReadU64 ();
}

void TCNTimestampTag::Print (std::ostream &os) const {
  os << "CreationTime=" << m_creationTime;
}

Time TCNTimestampTag::GetTxTime (void) const {
  return TimeStep (m_creationTime);
}

NS_OBJECT_ENSURE_REGISTERED (TCNQueueDisc);

TypeId TCNQueueDisc::GetTypeId (void) {
    static TypeId tid = TypeId ("ns3::TCNQueueDisc")
        .SetParent<QueueDisc> ()
        .SetGroupName ("TrafficControl")
        .AddConstructor<TCNQueueDisc> ()
        .AddAttribute ("Mode", "Whether to use Bytes (see MaxBytes) or Packets (see MaxPackets) as the maximum queue size metric.",
                        EnumValue (Queue::QUEUE_MODE_BYTES),
                        MakeEnumAccessor (&TCNQueueDisc::m_mode),
                        MakeEnumChecker (Queue::QUEUE_MODE_BYTES, "QUEUE_MODE_BYTES",
                                         Queue::QUEUE_MODE_PACKETS, "QUEUE_MODE_PACKETS"))
        .AddAttribute ("MaxPackets", "The maximum number of packets accepted by this TCNQueueDisc.",
                        UintegerValue (DEFAULT_TCN_LIMIT),
                        MakeUintegerAccessor (&TCNQueueDisc::m_maxPackets),
                        MakeUintegerChecker<uint32_t> ())
        .AddAttribute ("MaxBytes", "The maximum number of bytes accepted by this TCNQueueDisc.",
                        UintegerValue (1500 * DEFAULT_TCN_LIMIT),
                        MakeUintegerAccessor (&TCNQueueDisc::m_maxBytes),
                        MakeUintegerChecker<uint32_t> ())
        .AddAttribute ("Threshold",
                       "Instantaneous sojourn time threshold",
                        StringValue ("10us"),
                        MakeTimeAccessor (&TCNQueueDisc::m_threshold),
                        MakeTimeChecker ());
    return tid;
}

TCNQueueDisc::TCNQueueDisc ()
    : QueueDisc (),
      printed_once(false),
      m_threshold (0),
      microburst_happening(false) {
    NS_LOG_FUNCTION (this);

    // Read the HOP_LATENCY_THRESHOLD value from env
    char *buff = getenv("HOP_LATENCY_THRESHOLD");
    if (!buff) {
        NS_LOG_ERROR("HOP_LATENCY_THRESHOLD environment variable is not set!");
        HOP_LATENCY_THRESHOLD = 0;
        return;
    }

    HOP_LATENCY_THRESHOLD = std::strtol(buff, NULL, 10);
    NS_LOG_LOGIC("HOP_LATENCY_THRESHOLD=" << HOP_LATENCY_THRESHOLD);

}

TCNQueueDisc::~TCNQueueDisc () {
    NS_LOG_FUNCTION (this);
}

bool 
TCNQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item) {
    NS_LOG_FUNCTION (this << item);

    /*if (!printed_once) {
        PrintInterfaceToIPMapping();
        printed_once = true;
    }*/

    Ptr<Packet> p = item->GetPacket ();

    if (m_mode == Queue::QUEUE_MODE_PACKETS && (GetInternalQueue (0)->GetNPackets () + 1 > m_maxPackets)) {
        //NS_LOG_INFO("\tPacket with uid "<< uid << " and size " << psize << " dropped at " << GetInterfaceName());
        Drop (item);
        return false;
    }

    if (m_mode == Queue::QUEUE_MODE_BYTES && (GetInternalQueue (0)->GetNBytes () + item->GetPacketSize () > m_maxBytes)) {
        //NS_LOG_INFO("\tPacket with uid "<< uid << " and size " << psize << " dropped at " << GetInterfaceName());
        Drop (item);
        return false;
    }

    TCNTimestampTag tag;
    p->AddPacketTag (tag);

    //NS_LOG_INFO("\tPacket with uid "<< uid << " and size " << psize << " enqueued at " << GetInterfaceName());
    GetInternalQueue (0)->Enqueue (item);

    return true;

}

Ptr<QueueDiscItem>
TCNQueueDisc::DoDequeue (void) {
    NS_LOG_FUNCTION (this);

    Time now = Simulator::Now ();

    if (GetInternalQueue (0)->IsEmpty ()) {
        return NULL;
    }

    Ptr<QueueDiscItem> item = StaticCast<QueueDiscItem> (GetInternalQueue (0)->Dequeue ());
    Ptr<Packet> p = item->GetPacket ();

    TCNTimestampTag tag;
    bool found = p->RemovePacketTag (tag);
    if (!found) {
        NS_LOG_ERROR ("Cannot find the TCN Timestamp Tag");
        return NULL;
    }

    Time sojournTime = now - tag.GetTxTime ();

    if (sojournTime > m_threshold) {
        TCNQueueDisc::MarkingECN (item);
    }

    uint32_t swid = (uint32_t) std::strtoul(GetInterfaceName().c_str(), NULL, 16);

    uint64_t hop_latency = sojournTime.GetNanoSeconds();
    uint64_t microburst_threshold = 50000; // 50 microseconds = 50,000 ns
    if (hop_latency > microburst_threshold && !microburst_happening) {
        microburst_happening = true;
        microburst_start_time = now.GetNanoSeconds();
    } else if (microburst_happening && hop_latency < microburst_threshold) {
        microburst_happening = false;
        NS_LOG_INFO("CONTROL " << swid << " " << microburst_start_time << " " << now.GetNanoSeconds());
    }

    uint32_t uid = p->GetUid ();
   uint32_t psize = p->GetSize();

    MainIntTag maybeIntTag;
    NS_ASSERT(maybeIntTag.GetMode() == 0 && maybeIntTag.GetNEntries() == 0 && maybeIntTag.FiveTupleUnInitialized() && 
            maybeIntTag.GetCrc1() == 0 && maybeIntTag.GetCrc2() == 0);
    uint32_t tag_size = p->PeekPacketTag(maybeIntTag);
    ns3::five_tuple_t maybe_five_tuple = maybeIntTag.GetFiveTuple();

    if (maybeIntTag.IsModePitcher()) {
        
        tag_size = p->RemovePacketTag(maybeIntTag);
        
        NS_LOG_LOGIC("\tPacket with uid " << uid << " and size " << psize << " had IntTag of size " << tag_size 
            << " and contents: (" << maybeIntTag.GetMode() << ", " <<  maybeIntTag.GetNEntries() 
            << ", " <<  maybeIntTag.GetCrc1() << ", " <<  maybeIntTag.GetCrc2()
            << ", (" <<  Ipv4Address(maybe_five_tuple.source_ip) << ", " <<  Ipv4Address(maybe_five_tuple.dest_ip)
            << ", " <<  maybe_five_tuple.source_port << ", " <<  maybe_five_tuple.dest_port << ", " <<  maybe_five_tuple.protocol
            << ")) at interface " <<  swid);

        // Check that the number of entries is less than 3
        NS_ASSERT(maybeIntTag.GetNEntries() < 3);
        IntTag1 maybeIntTag1; IntTag2 maybeIntTag2; IntTag3 maybeIntTag3;
        IntTag1 new_int_tag1; IntTag2 new_int_tag2; IntTag3 new_int_tag3;

        unsigned char* crc1_buffer = (unsigned char*) malloc(10);
        unsigned char* crc2_buffer = (unsigned char*) malloc(10);
        
        uint16_t old_crc1 = maybeIntTag.GetCrc1();
        uint16_t old_crc2 = maybeIntTag.GetCrc2();
        
        
        uint32_t st_threshold1 = (uint32_t) hop_latency >> HOP_LATENCY_THRESHOLD;
        uint32_t st_threshold2 = ((uint32_t) hop_latency + (HOP_LATENCY_THRESHOLD >> 2)) >> HOP_LATENCY_THRESHOLD;
        
        memcpy(crc1_buffer, &old_crc1, 2);
        memcpy(crc1_buffer + 2, &swid, 4);
        memcpy(crc1_buffer + 6, &st_threshold1, 4);
       
        memcpy(crc1_buffer, &old_crc2, 2);
        memcpy(crc1_buffer + 2, &swid, 4);
        memcpy(crc1_buffer + 6, &st_threshold2, 4);

        uint16_t new_crc1 = crc_16(crc1_buffer, 10);
        uint16_t new_crc2 = crc_16(crc2_buffer, 10);
        free(crc1_buffer); free(crc2_buffer);

        maybeIntTag.SetCrc1(new_crc1);
        maybeIntTag.SetCrc2(new_crc2);

        switch(maybeIntTag.GetNEntries()) {
            case 0: {
                // check that neither IntTag1, nor IntTag2, nor IntTag3 are present
                p->PeekPacketTag(maybeIntTag1);
                p->PeekPacketTag(maybeIntTag2);
                p->PeekPacketTag(maybeIntTag3);
                NS_ASSERT(!maybeIntTag1.IsModePitcher() && !maybeIntTag2.IsModePitcher() && !maybeIntTag3.IsModePitcher());
                // check that the CRCs are all zero
                //NS_ASSERT(maybeIntTag.GetCrc1() == 0 && maybeIntTag.GetCrc2() == 0);
                // add IntTag1
                new_int_tag1.SetMode(49721);
                new_int_tag1.SetHopLatency((uint32_t) hop_latency);
                new_int_tag1.SetSwitchId(swid) ;
                NS_LOG_LOGIC("\t\tAdding IntTag1 with mode: " << new_int_tag1.GetMode() << ", hop_latency: " << new_int_tag1.GetHopLatency() << ", SwitchId: " << new_int_tag1.GetSwitchId());
                p->AddPacketTag(new_int_tag1);

                break;
            }
            case 1: {
                // check that IntTag1 is present, but neither IntTag2, nor IntTag3 are present
                p->PeekPacketTag(maybeIntTag1);
                p->PeekPacketTag(maybeIntTag2);
                p->PeekPacketTag(maybeIntTag3);
                NS_ASSERT(maybeIntTag1.IsModePitcher() && !maybeIntTag2.IsModePitcher() && !maybeIntTag3.IsModePitcher());
                // add IntTag2
                new_int_tag2.SetMode(49721);
                new_int_tag2.SetHopLatency((uint32_t) hop_latency);
                new_int_tag2.SetSwitchId(swid) ;
                NS_LOG_LOGIC("\t\tAdding IntTag2 with mode: " << new_int_tag2.GetMode() << ", hop_latency: " << new_int_tag2.GetHopLatency() << ", SwitchId: " << new_int_tag2.GetSwitchId());
                p->AddPacketTag(new_int_tag2);

                break;
            }
            case 2: {
                // check that IntTag1 and IntTag2 are present, but IntTag3 is not present
                p->PeekPacketTag(maybeIntTag1);
                p->PeekPacketTag(maybeIntTag2);
                p->PeekPacketTag(maybeIntTag3);
                NS_ASSERT(maybeIntTag1.IsModePitcher() && maybeIntTag2.IsModePitcher() && !maybeIntTag3.IsModePitcher());
                // add IntTag2
                new_int_tag3.SetMode(49721);
                new_int_tag3.SetHopLatency((uint32_t) hop_latency);
                new_int_tag3.SetSwitchId(swid);
                NS_LOG_LOGIC("\t\tAdding IntTag3 with mode: " << new_int_tag3.GetMode() << ", hop_latency: " << new_int_tag3.GetHopLatency() << ", SwitchId: " << new_int_tag3.GetSwitchId());
                p->AddPacketTag(new_int_tag3);
                break;
            }
            default: {
                NS_ASSERT(false);
                break;
            }
        }

        maybeIntTag.SetNEntries(maybeIntTag.GetNEntries()+1);
        p->AddPacketTag(maybeIntTag);
        
        

    } else {
        //NS_LOG_INFO("Packet with uid " << uid << " and size " << psize << " did NOT have IntTag of size " << tag_size 
        //    << " and contents: ( " << maybeIntTag.GetMode() << ", " <<  maybeIntTag.GetNEntries() << ") at interface " <<  GetInterfaceName());
    }


    //NS_LOG_INFO("\tPacket with "<< uid << " and size " << psize << " dequeued at " << GetInterfaceName());

    return item;

}

Ptr<const QueueDiscItem>
TCNQueueDisc::DoPeek (void) const {
    NS_LOG_FUNCTION (this);
    if (GetInternalQueue (0)->IsEmpty ()) {
        return NULL;
    }

    Ptr<const QueueDiscItem> item = StaticCast<const QueueDiscItem> (GetInternalQueue (0)->Peek ());

    return item;
}

bool
TCNQueueDisc::CheckConfig (void) {
    if (GetNInternalQueues () == 0) {
        Ptr<Queue> queue = CreateObjectWithAttributes<DropTailQueue> ("Mode", EnumValue (m_mode));
        if (m_mode == Queue::QUEUE_MODE_PACKETS) {
            queue->SetMaxPackets (m_maxPackets);
        } else {
            queue->SetMaxBytes (m_maxBytes);
        }
        AddInternalQueue (queue);
    }

    if (GetNInternalQueues () != 1) {
        NS_LOG_ERROR ("TCNQueueDisc needs 1 internal queue");
        return false;
    }

    return true;

}

void
TCNQueueDisc::InitializeParams (void) {
    NS_LOG_FUNCTION (this);
}

bool
TCNQueueDisc::MarkingECN (Ptr<QueueDiscItem> item) {
    Ptr<Ipv4QueueDiscItem> ipv4Item = DynamicCast<Ipv4QueueDiscItem> (item);
    if (ipv4Item == 0)   {
        NS_LOG_ERROR ("Cannot convert the queue disc item to ipv4 queue disc item");
        return false;
    }

    Ipv4Header header = ipv4Item -> GetHeader ();

    if (header.GetEcn () != Ipv4Header::ECN_ECT1)   {
        NS_LOG_ERROR ("Cannot mark because the ECN field is not ECN_ECT1");
        return false;
    }

    header.SetEcn(Ipv4Header::ECN_CE);
    ipv4Item->SetHeader(header);
    return true;
}

}
