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


