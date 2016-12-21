+++
date = "2016-08-15T16:11:58+05:30"
title = "Configuring the mongocxx driver"
[menu.main]
  weight = 10
  parent="mongocxx3"
+++

# Configuring the Driver

In the mongocxx driver, most configuration is done via the [connection
URI](https://docs.mongodb.com/master/reference/connection-string/).  Some
additional connection options are possible via the
[mongocxx::options::client] ({{< api3ref classmongocxx_1_1options_1_1client
>}}) class.

## Configuring TLS/SSL

To enable TLS (SSL), set `ssl=true` in the URI:

```nohighlight
mongodb://mongodb.example.com/?ssl=true
```

By default, mongocxx will verify server certificates against the local
system CA list.  You can override that by creating a
[mongocxx::options::ssl] ({{< api3ref classmongocxx_1_1options_1_1ssl >}})
object and passing it to `ssl_opts` on mongocxx::options::client.

For example, to use a custom CA or to disable certificate validation,
uncomment the corresponding line in the following example:

```cpp
mongocxx::options::client client_options;
mongocxx::options::ssl ssl_options;

// If the server certificate is not signed by a well-known CA,
// you can set a custom CA file with the `ca_file` option.
// ssl_options.ca_file("/path/to/custom/cert.pem");

// If you want to disable certificate verification, you
// can set the `allow_invalid_certificates` option.
// ssl_options.allow_invalid_certificates(true);

client_options.ssl_opts(ssl_options);
```

## Configuring authentication

### Default authentication mechanism

MongoDB 3.0 changed the default authentication mechanism from MONGODB-CR
to SCRAM-SHA-1. To create a credential that will authenticate properly
regardless of server version, use a connection string with the user and
password directly in the URI and with a parameter specifying the database
to authenticate from:

```cpp
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

auto client = mongocxx::client{uri{"mongodb://user1:pwd1@host1/?authSource=db1"}};
```

### SCRAM-SHA-1

To explicitly create a credential of type SCRAM-SHA-1 use a connection
string as above but with a parameter specifying the authentication
mechanism as "SCRAM-SHA-1":

```cpp
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

auto client = mongocxx::client{
    uri{"mongodb://user1:pwd1@host1/?authSource=db1&authMechanism=SCRAM-SHA-1"}};
```

### MONGODB-CR

To explicitly create a credential of type MONGODB-CR use a connection
string as above but with a parameter specifying the authentication mechanism
as "MONGODB-CR":

```cpp
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

auto client = mongocxx::client{
    uri{"mongodb://user1:pwd1@host1/?authSource=db1&authMechanism=MONGODB-CR"}};
```

Note that this is not recommended as a credential created in this way will
fail to authenticate after an authentication schema upgrade from
MONGODB-CR to SCRAM-SHA-1.

### X.509

The [X.509](https://www.mongodb.org/dochub/core/x509)
mechanism authenticates a user whose name is derived from the distinguished
subject name of the X.509 certificate presented by the driver during SSL
negotiation. This authentication method requires the use of SSL
connections with certificate validation and is available in MongoDB 2.6
and newer. To create a credential of this type, first create a set of
client options specifying the path to the PEM file containing the client
private key and certificate, and then use a connection string with a
parameter specifying the authentication mechanism as "MONGODB-X509" and
with SSL enabled:

```cpp
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/options/client.hpp>
#include <mongocxx/options/ssl.hpp>

mongocxx::options::ssl ssl_opts{};
ssl_opts.pem_file("client.pem");

mongocxx::options::client client_opts{};
client_opts.ssl_opts(ssl_opts);

auto client = mongocxx::client{
    uri{"mongodb://host1/?authMechanism=MONGODB-X509&ssl=true"}, client_opts};
```

See the MongoDB server
[X.509 tutorial](https://www.mongodb.org/dochub/core/x509-subject-name)
for more information about determining the subject name from the
certificate.

### Kerberos (GSSAPI)

[MongoDB Enterprise](https://www.mongodb.com/products/mongodb-enterprise)
supports proxy authentication through Kerberos service. To create a
credential of type [Kerberos (GSSAPI)](https://www.mongodb.org/dochub/core/kerberos)
use a connection string with the username and realm in the URI as well as
a parameter specifying the authentication mechanism as "GSSAPI":

```cpp
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

auto client = mongocxx::client{
    uri{"mongodb://username%40REALM.COM@host1/?authMechanism=GSSAPI"}};
```

Note that the "@" symbol in the URI must be escaped to "%40" as shown in the example above.

### LDAP

[MongoDB Enterprise](http://www.mongodb.com/products/mongodb-enterprise)
supports proxy authentication through a Lightweight Directory Access
Protocol (LDAP) service. To create a credential of type LDAP use a
connection string specifying the user as well as parameters specifying
the authentication source as "$external" and the authentication mechanishm
as "PLAIN":

```cpp
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

auto client = mongocxx::client{
    uri{"mongodb://user1:pwd1@host1/?authSource=$external&authMechanism=PLAIN"}};
```
