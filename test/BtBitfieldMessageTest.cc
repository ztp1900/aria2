#include "BtBitfieldMessage.h"
#include "PeerMessageUtil.h"
#include "Util.h"
#include <cppunit/extensions/HelperMacros.h>

using namespace std;

class BtBitfieldMessageTest:public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(BtBitfieldMessageTest);
  CPPUNIT_TEST(testCreate);
  CPPUNIT_TEST(testGetMessage);
  CPPUNIT_TEST(testDoReceivedAction);
  CPPUNIT_TEST(testToString);
  CPPUNIT_TEST_SUITE_END();
private:

public:
  void setUp() {
  }

  void testCreate();
  void testGetMessage();
  void testDoReceivedAction();
  void testToString();
};


CPPUNIT_TEST_SUITE_REGISTRATION(BtBitfieldMessageTest);

void BtBitfieldMessageTest::testCreate() {
  unsigned char msg[5+2];
  PeerMessageUtil::createPeerMessageString(msg, sizeof(msg), 3, 5);
  unsigned char bitfield[2];
  memset(bitfield, 0xff, sizeof(bitfield));
  memcpy(&msg[5], bitfield, sizeof(bitfield));
  BtBitfieldMessageHandle pm = BtBitfieldMessage::create(&msg[4], 3);
  CPPUNIT_ASSERT_EQUAL((int8_t)5, pm->getId());
  CPPUNIT_ASSERT(memcmp(bitfield, pm->getBitfield(), sizeof(bitfield)) == 0);
  CPPUNIT_ASSERT_EQUAL((int32_t)2, pm->getBitfieldLength());
  // case: payload size is wrong
  try {
    unsigned char msg[5];
    PeerMessageUtil::createPeerMessageString(msg, sizeof(msg), 1, 5);
    BtBitfieldMessage::create(&msg[4], 1);
    CPPUNIT_FAIL("exception must be thrown.");
  } catch(...) {
  }
  // case: id is wrong
  try {
    unsigned char msg[5+2];
    PeerMessageUtil::createPeerMessageString(msg, sizeof(msg), 3, 6);
    BtBitfieldMessage::create(&msg[4], 3);
    CPPUNIT_FAIL("exception must be thrown.");
  } catch(...) {
  }
}

void BtBitfieldMessageTest::testGetMessage() {
  BtBitfieldMessage msg;
  unsigned char bitfield[2];
  memset(bitfield, 0xff, sizeof(bitfield));
  msg.setBitfield(bitfield, sizeof(bitfield));
  unsigned char data[5+2];
  PeerMessageUtil::createPeerMessageString(data, sizeof(data), 3, 5);
  memcpy(&data[5], bitfield, sizeof(bitfield));
  CPPUNIT_ASSERT(memcmp(msg.getMessage(), data, 7) == 0);
}

void BtBitfieldMessageTest::testDoReceivedAction() {
  PeerHandle peer = new Peer("host1", 6969);
  peer->allocateBitfield(16*1024, 16*16*1024);
  BtBitfieldMessage msg;
  msg.setPeer(peer);
  unsigned char bitfield[] = { 0xff, 0xff };
  msg.setBitfield(bitfield, sizeof(bitfield));
  
  CPPUNIT_ASSERT_EQUAL(string("0000"), Util::toHex(peer->getBitfield(),
						   peer->getBitfieldLength()));
  msg.doReceivedAction();
  CPPUNIT_ASSERT_EQUAL(string("ffff"), Util::toHex(peer->getBitfield(),
						   peer->getBitfieldLength()));
}

void BtBitfieldMessageTest::testToString() {
  BtBitfieldMessage msg;
  unsigned char bitfield[] = { 0xff, 0xff };
  msg.setBitfield(bitfield, sizeof(bitfield));

  CPPUNIT_ASSERT_EQUAL(string("bitfield ffff"), msg.toString());
}
