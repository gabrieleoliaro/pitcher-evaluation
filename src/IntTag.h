namespace ns3 {

class IntTag : public Tag
{
public:
  IntTag ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual void Print (std::ostream &os) const;

  uint16_t GetMode (void) const;
  uint16_t GetNEntries (void) const;

  void SetMode(uint16_t mode);
  void SetNEntries (uint16_t n_entries);

private:
  uint16_t mode_;
  uint16_t n_entries_; 

};

IntTag::IntTag ()
  : mode_ (0),
    n_entries_(0) {
}

TypeId IntTag::GetTypeId (void) {
  static TypeId tid = TypeId ("ns3::IntTag")
    .SetParent<Tag> ()
    .AddConstructor<IntTag> ()
    .AddAttribute ("mode_", "The mode of the INT header.",
      UintegerValue (0),
      MakeUintegerAccessor (&IntTag::mode_),
      MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("n_entries_", "The number of entries in the tag.",
      UintegerValue (0),
      MakeUintegerAccessor (&IntTag::n_entries_),
      MakeUintegerChecker<uint16_t> ());
  return tid;
}

TypeId IntTag::GetInstanceTypeId (void) const {
  return GetTypeId ();
}

uint32_t IntTag::GetSerializedSize (void) const {
  return 4;
}

void IntTag::Serialize (TagBuffer i) const {
  i.WriteU16 (mode_);
  i.WriteU16 (n_entries_);
}

void IntTag::Deserialize (TagBuffer i) {
  mode_ = i.ReadU16 ();
  n_entries_ = i.ReadU16 ();
}

void IntTag::Print (std::ostream &os) const {
  os << "Mode: " << mode_ << ", NEntries: " << n_entries_;
}

uint16_t IntTag::GetMode (void) const {
  return mode_;
}

uint16_t IntTag::GetNEntries (void) const {
  return n_entries_;
}

void IntTag::SetMode (uint16_t mode) {
  mode_ = mode;
}

void IntTag::SetNEntries (uint16_t n_entries) {
  n_entries_ = n_entries;
}

}