#include <cstdlib>
#include <cstdio>
#include "tcn-queue-disc.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/enum.h"
#include "ns3/uinteger.h"
#include "ns3/string.h"
#include "ns3/ipv4-queue-disc-item.h"
#include "ns3/drop-tail-queue.h"
#include "../../crc16.c"
#include "ns3/internet-module.h"
#define INT_MODE 784591620
#define PITCHER_MODE 234569283
#define MAX_HOPS 5
#define HOP_LATENCY_THRESHOLD 7 // divide hop latency values by 2^7=128 ns

#define DEFAULT_TCN_LIMIT 100

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
  : m_creationTime (Simulator::Now ().GetTimeStep ())
{
}

TypeId
TCNTimestampTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TCNTimestampTag")
    .SetParent<Tag> ()
    .AddConstructor<TCNTimestampTag> ()
    .AddAttribute ("CreationTime",                                      // name of the attribute 
                   "The time at which the timestamp was created",       // help string
                   StringValue ("0.0s"),                                // initial value
                   MakeTimeAccessor (&TCNTimestampTag::GetTxTime),      // accessor
                   MakeTimeChecker ())                                  // checker
  ;
  return tid;
}

TypeId
TCNTimestampTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
TCNTimestampTag::GetSerializedSize (void) const
{
  return 8;
}

void
TCNTimestampTag::Serialize (TagBuffer i) const
{
  i.WriteU64 (m_creationTime);
}

void
TCNTimestampTag::Deserialize (TagBuffer i)
{
  m_creationTime = i.ReadU64 ();
}

void
TCNTimestampTag::Print (std::ostream &os) const
{
  os << "CreationTime=" << m_creationTime;
}

Time
TCNTimestampTag::GetTxTime (void) const
{
  return TimeStep (m_creationTime);
}

NS_OBJECT_ENSURE_REGISTERED (TCNQueueDisc);

TypeId
TCNQueueDisc::GetTypeId (void)
{
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
                        MakeTimeChecker ())
    ;
    return tid;
}

TCNQueueDisc::TCNQueueDisc ()
    : QueueDisc (),
      m_threshold (0)
{
    NS_LOG_FUNCTION (this);
}

TCNQueueDisc::~TCNQueueDisc ()
{
    NS_LOG_FUNCTION (this);
}

// Thanks to Sivaram for this snippet
namespace string_patch {
    template <typename T> std::string to_string( const T& n ) {
        std::ostringstream stm;
        stm << n ;
        return stm.str();
    }
}


bool
TCNQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
    NS_LOG_FUNCTION (this << item);

    Ptr<Packet> p = item->GetPacket ();
    if (m_mode == Queue::QUEUE_MODE_PACKETS && (GetInternalQueue (0)->GetNPackets () + 1 > m_maxPackets))
    {
        Drop (item);
        return false;
    }

    if (m_mode == Queue::QUEUE_MODE_BYTES && (GetInternalQueue (0)->GetNBytes () + item->GetPacketSize () > m_maxBytes))
    {
        Drop (item);
        return false;
    }

    TCNTimestampTag tag;
    p->AddPacketTag (tag);

    GetInternalQueue (0)->Enqueue (item);

    // if (printed_info == false) {
    //     printed_info = true;
    //     NS_LOG_INFO("PIM " << string_patch::to_string(GetInternalQueue(0)) << " " << )
    // }

    return true;

}



Ptr<QueueDiscItem>
TCNQueueDisc::DoDequeue (void)
{
    NS_LOG_FUNCTION (this);

    Time now = Simulator::Now ();

    if (GetInternalQueue (0)->IsEmpty ())
    {
        return NULL;
    }

    Ptr<QueueDiscItem> item = StaticCast<QueueDiscItem> (GetInternalQueue (0)->Dequeue ());
    Ptr<Packet> p = item->GetPacket ();

    std::string interface_name = string_patch::to_string(GetInternalQueue(0));
    uint32_t swid = (uint32_t) std::strtoul(interface_name.c_str(), NULL, 0);


    TCNTimestampTag tag;
    bool found = p->RemovePacketTag (tag);
    if (!found)
    {
        NS_LOG_ERROR ("Cannot find the TCN Timestamp Tag");
        return NULL;
    }
    Time sojournTime = now - tag.GetTxTime ();

    uint32_t sojournTimeNS = (uint32_t) sojournTime.GetNanoSeconds();

    // Microburst if the hop latency is larger than 50μs = 50*1000 ns
    if (sojournTimeNS > 50000) {
        std::string interface_name = string_patch::to_string(GetInternalQueue(0));
        NS_LOG_INFO("TCN#QUEUE - Microburst on swid:" << swid << " sojournTime=" << sojournTime << " start_time: " << tag.GetTxTime());
    }

    // remove header, extract info and perform sanity checks
    IntHeader intHeader;
    p->RemoveHeader (intHeader);
    int mode = intHeader.GetMode();
    int nentries = intHeader.GetNEntries();

    uint32_t uid = p->GetUid ();

    NS_LOG_INFO("analyzing packet with uid " << uid);

    NS_LOG_INFO("mode: " << mode << " nentries:" << nentries);

    if (mode == PITCHER_MODE) {
        assert(mode == PITCHER_MODE);
        assert(nentries < MAX_HOPS);
        int_info last_entry;
        intHeader.GetEntryAtIndex(nentries-1, &last_entry);



        unsigned char* crc1_buffer = (unsigned char*) malloc(10);
        memcpy(crc1_buffer, &last_entry.crc1, 2);
        memcpy(crc1_buffer + 2, &swid, 4);
        uint32_t st_threshold1 = sojournTimeNS >> HOP_LATENCY_THRESHOLD;
        memcpy(crc1_buffer + 6, &st_threshold1, 4);
        unsigned char* crc2_buffer = (unsigned char*) malloc(10);
        memcpy(crc1_buffer, &last_entry.crc2, 2);
        memcpy(crc1_buffer + 2, &swid, 4);
        uint32_t st_threshold2 = (sojournTimeNS + (HOP_LATENCY_THRESHOLD >> 2)) >> HOP_LATENCY_THRESHOLD;
        memcpy(crc1_buffer + 6, &st_threshold2, 4);
        

        int result = intHeader.AddEntry(PITCHER_MODE,
            swid, (uint32_t) sojournTimeNS, crc_16(crc1_buffer, 10), crc_16(crc2_buffer, 10));

        p->AddHeader(intHeader);
        
        assert(result!= -1);
        NS_LOG_INFO("Result of inserting: " << result);
    }

    

    if (sojournTime > m_threshold)
    {
        TCNQueueDisc::MarkingECN (item);
    }

    return item;

}

Ptr<const QueueDiscItem>
TCNQueueDisc::DoPeek (void) const
{
    NS_LOG_FUNCTION (this);
    if (GetInternalQueue (0)->IsEmpty ())
    {
        return NULL;
    }

    Ptr<const QueueDiscItem> item = StaticCast<const QueueDiscItem> (GetInternalQueue (0)->Peek ());

    return item;

}

bool
TCNQueueDisc::CheckConfig (void)
{
    if (GetNInternalQueues () == 0)
    {
        Ptr<Queue> queue = CreateObjectWithAttributes<DropTailQueue> ("Mode", EnumValue (m_mode));
        if (m_mode == Queue::QUEUE_MODE_PACKETS)
        {
            queue->SetMaxPackets (m_maxPackets);
        }
        else
        {
            queue->SetMaxBytes (m_maxBytes);
        }
        AddInternalQueue (queue);
    }

    if (GetNInternalQueues () != 1)
    {
        NS_LOG_ERROR ("TCNQueueDisc needs 1 internal queue");
        return false;
    }

    return true;

}

void
TCNQueueDisc::InitializeParams (void)
{
    NS_LOG_FUNCTION (this);
}

bool
TCNQueueDisc::MarkingECN (Ptr<QueueDiscItem> item)
{
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
