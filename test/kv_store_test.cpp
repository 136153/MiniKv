#include <gtest/gtest.h>
#include "kv_store.h"

TEST(KVStoreTEST, SetAgentGet) {
    KVStore store;

    store.set("wang", "123");
    string val;

    EXPECT_TRUE(store.get("wang", val));
    EXPECT_EQ(val, "123");
}

TEST(KVStoreTEST, DELAgent) {
    KVStore store;
    store.set("sun", "123");
    store.del("sun");
    string val;
    EXPECT_FALSE(store.get("sun", val));
}
