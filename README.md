[![Build Status](https://travis-ci.org/insomniacslk/python-dublin-traceroute.svg?branch=master)](https://travis-ci.org/insomniacslk/python-dublin-traceroute)

# What

This repository contains the Python bindings for [Dublin
Traceroute](https://github.com/insomniacslk/dublin-traceroute) . To install
these bindings you have first to install Dublin Traceroute.

# How

## To install

You need the following dependencies:
* libpcap
* libtins
* jsoncpp
* libdublintraceroute

See the following sections for system-specific instructions.


### Debian

* `libtins` is available in the `testing` repo
* `libdublintraceroute` is available in the `experimental` repo

Once you have enabled those repos, install the dependencies as root:
```bash
apt-get install libpcap-dev libjsoncpp-dev libtins-dev libdublintraceroute-dev
```

Then install the Python module as root:

```bash
python setup.py install 
```

If you do not want to install the module as root, just add `--user` to the setup.py invocation.

### OS X

* `libtins` and `jsoncpp` are available in brew
* `libpcap` is already installed on OS X
* `libdublintraceroute` would like to be in brew, but the authors say that we don't have enough stars to accept the formula. Hence it can be installed from source, or using the formula I wrote. See below or see https://github.com/insomniacslk/dublin-traceroute/blob/master/documentation/readme/README.md#building-on-os-x.

```bash
brew install jsoncpp libtins
brew install https://raw.githubusercontent.com/insomniacslk/homebrew/master/Library/Formula/dublin-traceroute.rb
```

If you prefer to install `libdublintraceroute` from source instead, see https://dublin-traceroute.net .

Then install the Python module as root:

```bash
python setup.py install 
```

If you do not want to install the module as root, just add `--user` to the setup.py invocation.


## To run

Let's try to run the same traceroute in the CLI example, but this time using
Python. Remember that you need root permissions in this case, or you need to
manually set the CAP_NET_RAW capability to your Python interpreter (which is not
necessarily a good idea).

Let's use 12345 as source port, 33434 as base destination port, and 8.8.8.8 as
target:

```python
>>> import dublintraceroute
>>> dublin = dublintraceroute.DublinTraceroute("8.8.8.8")
>>> print dublin
<DublinTraceroute (target="8.8.8.8", sport=12345, dport=33434, npaths=20, max_ttl=30)
>>> results = dublin.traceroute()
>>> import pprint
>>> pprint.pprint(results)
{u'flows': {u'33434': [{u'is_last': False,
                        u'name': u'192.168.9.1',
                        u'nat_id': 0,
                        u'received': {u'icmp': {u'code': 11,
                                                u'description': u'TTL expired in transit',
                                                u'type': 0},
                                      u'ip': {u'dst': u'192.168.9.17',
                                              u'src': u'192.168.9.1',
                                              u'ttl': 64}},
                        u'rtt_usec': 1881,
                        u'sent': {u'ip': {u'dst': u'8.8.8.8',
                                          u'src': u'192.168.9.17',
                                          u'ttl': 1},
                                  u'udp': {u'dport': 33434,
                                           u'sport': 12345}}},
...
>>> graph = dublintraceroute.to_graphviz(results)
>>> graph.draw('traceroute.png')
>>> graph.write('traceroute.dot')
```

A naive implementation of the traceroute with a oneliner could be:

```bash
$ sudo python -c "import dublintraceroute as dub; dub.print_results(dub.DublinTraceroute('8.8.8.8', npaths=3).traceroute())"
  ttl  33436                                        33434                                      33435
-----  -------------------------------------------  -----------------------------------------  -------------------------------------------
    1  192.168.9.1 (22491 usec)                     192.168.9.1 (8965 usec)                    192.168.9.1 (15755 usec)
    2  *                                            *                                          *
    3  188-141-126-1.dynamic.upc.ie (30541 usec)    188-141-126-1.dynamic.upc.ie (16934 usec)  188-141-126-1.dynamic.upc.ie (29183 usec)
    4  84.116.238.50 (30549 usec)                   84.116.238.50 (17866 usec)                 84.116.238.50 (30824 usec)
    5  213.46.165.18 (30542 usec)                   213.46.165.18 (17904 usec)                 213.46.165.18 (30862 usec)
    6  66.249.95.135 (32295 usec)                   66.249.95.113 (17913 usec)                 209.85.250.213 (30873 usec)
    7  google-public-dns-a.google.com (31419 usec)  *                                          google-public-dns-a.google.com (30873 usec)
    8  google-public-dns-a.google.com (31455 usec)  *                                          google-public-dns-a.google.com (30873 usec)
    9  google-public-dns-a.google.com (31455 usec)  *                                          google-public-dns-a.google.com (30866 usec)
   10  google-public-dns-a.google.com (31450 usec)  *                                          google-public-dns-a.google.com (30865 usec)
   11  google-public-dns-a.google.com (31444 usec)  *                                          google-public-dns-a.google.com (30862 usec)
   12  google-public-dns-a.google.com (31443 usec)  *                                          google-public-dns-a.google.com (30861 usec)
   13  *                                            *                                          *
   14  *                                            *                                          *
   15  *                                            *                                          *
   16  *                                            *                                          *
   17  *                                            *                                          *
   18  *                                            *                                          *
   19  *                                            *                                          *
   20  *                                            *                                          *
   21  *                                            *                                          *
   22  *                                            *                                          *
   23  *                                            *                                          *
   24  *                                            *                                          *
   25  *                                            *                                          *
   26  *                                            *                                          *
   27  *                                            *                                          *
   28  *                                            *                                          *
   29  *                                            *                                          *
   30  *                                            *                                          *
```

You can also invoke the module directly, with `python -m dublintraceroute --help`.

For example:

```bash
$ sudo python -m dublintraceroute 8.8.8.8
...
```

then generate a PNG from the traceroute:

```bash
python -m dublintraceroute --plot trace.json
```

# Who

Andrea Barberio, find more about me at https://insomniac.slackware.it
