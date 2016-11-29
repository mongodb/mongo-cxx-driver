+++
date = "2016-08-15T16:11:58+05:30"
title = "Thread safety"
[menu.main]
  weight = 15
  parent="mongocxx3"
+++

TLDR: **Always give each thread its own `mongocxx::client`**.

In general each `mongocxx::client` object AND all of its child objects
**should be used by a single thread at a time**.

Even if you create multiple child objects from a single `client`, and
synchronize them individually, that is unsafe as they will concurrently
modify internal structures of the `client`. The same is true if you copy a
child object.

## Never do this

```c++
mongocxx::instance instance{};
mongocxx::client c{};
auto db1 = c["db1"];
auto db2 = c["db2"];
std::mutex db1_mtx{};
std::mutex db2_mtx{};

auto threadfunc = [](mongocxx::database& db, std::mutex& mtx) {
    std::scoped_lock<std::mutex>(mtx);
    db["col"].insert({});
}

// BAD! these two databases are individually synchronized, but they are derived from the same
// client, so they can only be accessed by one thread at a time
std::thread([]() { threadfunc(db1, db1_mtx); threadfunc(db2, db2_mtx); });
std::thread([]() { threadfunc(db2, db2_mtx); threadfunc(db1, db1_mtx); });
```

In the above example, even though the two databases are individually
synchronized, they are derived from the same client. There is shared state
inside the library that is now being modified without synchronization. The
same problem occurs if `db2` is a copy of `db1`.

## A better version

```c++
mongocxx::instance instance{};
mongocxx::client c1{};
mongocxx::client c2{}
std::mutex c1_mtx{};
std::mutex c2_mtx{};

auto threadfunc = [](stdx::string_view dbname, mongocxx::client& client, std::mutex& mtx) {
    std::scoped_lock<std::mutex>(mtx);
    client[dbname]["col"].insert({});
}

// Good! these two clients are individually synchronized, so it is safe to share them between
// threads.
std::thread([]() { threadfunc("db1", c1, c1_mtx); threadfunc("db2", c2, c2_mtx); });
std::thread([]() { threadfunc("db2", c2, c2_mtx); threadfunc("db1", c1, c1_mtx); });
```

## The best version

```c++
mongocxx::instance instance{};
auto threadfunc = [](mongocxx::client& client, stdx::string_view dbname) {
    client[dbname]["col"].insert({});
}
// don't even bother sharing clients. Just give each thread its own,
std::thread([]() {
    mongocxx::client c{};
    threadfunc(c, "db1");
    threadfunc(c, "db2");
});

std::thread([]() {
    mongocxx::client c{};
    threadfunc(c, "db2");
    threadfunc(c, "db1");
});
```

In most programs, clients will be long lived - so its less of a hassle (and
more performant) to make one per-thread. Obviously in this contrived
example, there's quite a bit of overhead because we're doing so little work
with each client - but in a real program this is the best solution.

