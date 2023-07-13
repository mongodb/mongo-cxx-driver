# Same function, different pointers

## Problem

Some of the C++ Windows Evergreen tasks failed in CI for the auto download C
driver work, specifically with the following error:

```
[2023/07/12 20:33:32.553]         Start  3: logging
[2023/07/12 20:33:32.553]    3/13 Test  #3: logging ..........................***Failed    0.05 sec
[2023/07/12 20:33:32.553]   2023/07/12 21:33:32.0548: [ 2408]:    ERROR:          foo: bar
[2023/07/12 20:33:32.553]   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
[2023/07/12 20:33:32.553]   test_logging.exe is a Catch v2.13.7 host application.
[2023/07/12 20:33:32.553]   Run with -? for options
[2023/07/12 20:33:32.553]   -------------------------------------------------------------------------------
[2023/07/12 20:33:32.553]   a user-provided log handler will be used for logging output
[2023/07/12 20:33:32.553]   -------------------------------------------------------------------------------
[2023/07/12 20:33:32.553]   C:\data\mci\57bb250dc5293701ff463490ada7d3f9\mongo-cxx-driver\src\mongocxx\test\logging.cpp(50)
[2023/07/12 20:33:32.553]   ...............................................................................
[2023/07/12 20:33:32.553]   C:\data\mci\57bb250dc5293701ff463490ada7d3f9\mongo-cxx-driver\src\mongocxx\test\logging.cpp(63): FAILED:
[2023/07/12 20:33:32.553]     REQUIRE( events.size() == 1 )
[2023/07/12 20:33:32.553]   with expansion:
[2023/07/12 20:33:32.553]     0 == 1
[2023/07/12 20:33:32.553]   ===============================================================================
[2023/07/12 20:33:32.554]   test cases: 1 | 1 failed
[2023/07/12 20:33:32.554]   assertions: 2 | 1 passed | 1 failed
```

## Relevant Code

Looking at the backtrace of where the test failed, we find the function below:

```C++
// src/mongocxx/test/logging.cpp

TEST_CASE("a user-provided log handler will be used for logging output", "[instance]") {
    reset_log_handler_when_done rlhwd;

    std::vector<test_log_handler::event> events;
    mongocxx::instance driver{stdx::make_unique<test_log_handler>(&events)};

    REQUIRE(&mongocxx::instance::current() == &driver);

    // The libmongoc namespace mocking system doesn't play well with varargs
    // functions, so we use a bare mongoc_log call here.
    mongoc_log(::MONGOC_LOG_LEVEL_ERROR, "foo", "bar");

    REQUIRE(events.size() == 1);
    REQUIRE(events[0] == std::make_tuple(log_level::k_error, "foo", "bar"));
}
```

The expectation is that the following happens:
1. The driver is instantiated with a custom log handler, namely
`test_log_handler`.
2. An error level message is then logged.
3. The `events` vector will now contain the error level log message.

What actually happens is:
1. The driver is instantiated with a custom log handler, namely
`test_log_handler`.
2. An error level message is then logged.
3. The `events` vector is **empty**, and contains no messages.


`test_log_handler` is implemented with the following method.
```C++
// src/mongocxx/test/logging.cpp

    void operator()(log_level level,
                    stdx::string_view domain,
                    stdx::string_view message) noexcept final {
        if (level == log_level::k_error)
            _events->emplace_back(level, std::string(domain), std::string(message));
    }
```

The idea is that when an `error` level message is logged, the log message is
pushed onto the `_events` vector. This vector is the same vector that is
instantiated in the test function.

Going deeper, the user log handler is set here:
```C++
// src/mongocxx/instance.cpp

class instance::impl {
   public:
    impl(std::unique_ptr<logger> logger) : _user_logger(std::move(logger)) {
        libmongoc::init();
        if (_user_logger) {
            libmongoc::log_set_handler(user_log_handler, _user_logger.get());
            // The libmongoc namespace mocking system doesn't play well with varargs
            // functions, so we use a bare mongoc_log call here.
            mongoc_log(MONGOC_LOG_LEVEL_INFO, "mongocxx", "libmongoc logging callback enabled");
        } else {
            libmongoc::log_set_handler(null_log_handler, nullptr);
        }
        ...
```

> NOTICE: We see that the libmongoc functions `mongoc_log_set_handler` and
`mongoc_log` are used here. Notice that the code block above is found in the
libmongocxx library and that the code found in `src/mongocxx/test/logging.cpp`
is compiled into an executable that links with libmongocxx. We can therefore
see that `mongoc_log` is being called both from within the libmongocxx library
and also the executable that is linking libmongocxx.

## Debugging

I spent some time debugging this. I had several hypotheses as to what could be
the root cause. Many of which did not pan out, but the hypothesis that lead to
results is the following:

> What if the two different call sites for `mongoc_log` had different pointer
values?

I added the following logs to test this hypothesis:

In `logging.cpp`:
```C++
// src/mongocxx/test/logging.cpp

// The libmongoc namespace mocking system doesn't play well with varargs
// functions, so we use a bare mongoc_log call here.
fprintf(stderr, "%s:%s: mongoc_log is at address: '%p'\n", __FILE__, __FUNCTION__, (void *)mongoc_log);
mongoc_log(::MONGOC_LOG_LEVEL_ERROR, "foo", "bar");
```

In `instance.cpp`:
```C++
// src/mongocxx/instance.cpp

libmongoc::log_set_handler(user_log_handler, _user_logger.get());
// The libmongoc namespace mocking system doesn't play well with varargs
// functions, so we use a bare mongoc_log call here.
fprintf(stderr, "%s:%s: mongoc_log is at address: '%p'\n", __FILE__, __FUNCTION__, (void *)mongoc_log);
mongoc_log(MONGOC_LOG_LEVEL_INFO, "mongocxx", "libmongoc logging callback enabled");
```

Low and behold, we see that `mongoc_log` does indeed have different pointer
values depending on whether it is called from the mongocxx library or the
executable.
```
mongo-cxx-driver\src\mongocxx\instance.cpp:mongocxx::v_noabi::instance::impl::impl: mongoc_log is at address: '00007FFA5E7761C7'
mongo-cxx-driver\src\mongocxx\test\logging.cpp:`anonymous-namespace'::____C_A_T_C_H____T_E_S_T____0: mongoc_log is at address: '00007FF7FD905E02'
```

In addition to the log messages above, I also added the following log message
to libmongoc.

```C
void
mongoc_log (mongoc_log_level_t log_level,
            const char *log_domain,
            const char *format,
            ...)
{
   va_list args;
   char *message;
   int stop_logging;

   fprintf(stderr, "mongoc_log is at address: '%p'\n", (void *)mongoc_log);

   bson_once (&once, &_mongoc_ensure_mutex_once);
   ...

```

When running on Linux, we get the following:
```
mongoc_log is at address: '0x7f846b9c3850'
mongoc_log is at address: '0x7f846b9c3850'
```

Notice that these are the same pointers.

On Windows, it's a different story.
```
mongoc_log is at address: '00007FFC6ED05DB7'
mongoc_log is at address: '00007FFC6F171FA0'
```

Notice that these are different pointers.

## Explanation

On Linux, when a library, *L*, is
statically linked into another library, *L*'s symbols are public by default.
However, on Windows that is not the case. In order for the executable to find
the symbol `mongoc_log`, it needs to also link to `libmongoc`.

Hence, on Windows `mongoc_log` as well as the global variable for which log
handler (In this case the default handler vs the user handler) can be found at
two different addresses in the process's memory image.

Therefore the `mongoc_log` function called from the test function did not have
a reference to the user log handler, so it instead sent a message to the global
log handler, while the `mongoc_log` function called from mongocxx did have a
reference to the user log handler.

## Implications

- Different log handlers are used for the different `mongoc_log` functions.
- Therefore, static and global variables are different between the mongoc
functions used from the executable vs the mongocxx library, since even though
these functions have the same name, as far as the program is concerned they are
different pointers and hence different functions.

The above makes sense once walking through how we got into this situation, but
it is not something that is immediately obvious when comparing the behaviour of
the program to its source code.
