[![Build Status](https://travis-ci.org/insomniacslk/python-dublin-traceroute.svg?branch=master)](https://travis-ci.org/insomniacslk/python-dublin-traceroute)

[![Version](https://img.shields.io/pypi/v/dublintraceroute.svg)](https://pypi.python.org/pypi/dublintraceroute)
[![Downloads](https://insomniac.slackware.it/badges/dublintraceroute)](https://pypi.python.org/pypi/dublintraceroute)

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

You can optionally install `pandas` and `matplotlib` for additional analysis functionality.

See the following sections for system-specific instructions.


### Prerequisites on Debian

* `libtins` is available in the `testing` repo
* `libdublintraceroute` is available in the `unstable` repo

Once you have enabled those repos, install the dependencies as root:
```bash
apt-get install libpcap-dev libjsoncpp-dev libtins-dev libdublintraceroute-dev
```

### Prerequisites on OS X

* `libtins` and `jsoncpp` are available in brew
* `libpcap` is already installed on OS X
* `libdublintraceroute` would like to be in homebrew, but their maintainers say that we don't have enough stars on GitHub to accept the formula. Hence it can be installed from source, or using the formula I wrote. See below or see https://github.com/insomniacslk/dublin-traceroute/blob/master/documentation/readme/README.md#building-on-os-x.

```bash
brew install jsoncpp libtins
https://github.com/insomniacslk/dublin-traceroute/blob/master/homebrew/dublin-traceroute.rb
```

If you prefer to install `libdublintraceroute` from source instead, see https://dublin-traceroute.net .

### Installing dublintraceroute

Once the prerequisites are installed, there are two ways to install `dublintraceroute` itself. From source, or from PyPI (i.e. using `pip` or `easy_install`)

#### From source

```bash
python setup.py install 
```

If you do not want to install the module as root, just add `--user` to the setup.py invocation.

#### From PyPI

```bash
pip install dublintraceroute  # use pip3 for the Python3 version
```

## To run

Let's try to run the same traceroute in the CLI example, but this time using
Python. Remember that you need root permissions in this case, or you need to
manually set the CAP_NET_RAW capability to your Python interpreter (which is not
necessarily a good idea).

Let's use 12345 as source port, 33434 as base destination port, and 8.8.8.8 as
target:

```python
>>> import dublintraceroute
>>> dublin = dublintraceroute.DublinTraceroute('8.8.8.8', npaths=3)
>>> print(dublin)
<DublinTraceroute (target='8.8.8.8', sport=12345, dport=33434, npaths=3, max_ttl=30)>
>>> results = dublin.traceroute()
>>> type(results)
<class 'dublintraceroute.tracerouteresults.TracerouteResults'>
>>> pprint.pprint(results)
{u'flows': {u'33434': [{u'is_last': False,
                        u'name': u'gateway',
                        u'nat_id': 0,
                        u'received': {u'icmp': {u'code': 11,
                                                u'description': u'TTL expired in transit',
                                                u'extensions': [],
                                                u'mpls_labels': [],
                                                u'type': 0},
                                      u'ip': {u'dst': u'192.168.9.20',
                                              u'id': 1999,
                                              u'src': u'192.168.9.1',
                                              u'ttl': 64},
                                      u'timestamp': u'1474069492.648438'},
                        u'rtt_usec': 308179,
                        u'sent': {u'ip': {u'dst': u'8.8.8.8',
                                          u'src': u'192.168.9.20',
                                          u'ttl': 1},
                                  u'timestamp': u'1474069492.340259',
                                  u'udp': {u'dport': 33434,
                                           u'sport': 12345}}},
...
>>> results.pretty_print()
  ttl  33436                                         33434                                         33435
-----  --------------------------------------------  --------------------------------------------  --------------------------------------------
    1  gateway (427110 usec)                         gateway (308179 usec)                         gateway (403034 usec)
    2  *                                             *                                             *
    3  188-141-126-1.dynamic.upc.ie (433980 usec)    188-141-126-1.dynamic.upc.ie (355249 usec)    188-141-126-1.dynamic.upc.ie (414672 usec)
    4  ie-dub01a-ra3-ae34-0.aorta.net (448009 usec)  ie-dub01a-ra3-ae34-0.aorta.net (141946 usec)  ie-dub01a-ra3-ae34-0.aorta.net (414890 usec)
    5  ie-dub01a-ri1-ae50-0.aorta.net (435168 usec)  84-116-130-142.aorta.net (355207 usec)        84-116-130-142.aorta.net (419248 usec)
    6  213.46.165.18 (460317 usec)                   213.46.165.18 (355184 usec)                   213.46.165.18 (420044 usec)
    7  216.239.43.3 (460323 usec)                    216.239.43.3 (355110 usec)                    66.249.95.91 (420293 usec)
    8  *                                             google-public-dns-a.google.com (355082 usec)  google-public-dns-a.google.com (419689 usec)
    9  *                                             google-public-dns-a.google.com (355036 usec)  google-public-dns-a.google.com (419759 usec)
   10  *                                             google-public-dns-a.google.com (354993 usec)  google-public-dns-a.google.com (419943 usec)
   11  *                                             google-public-dns-a.google.com (354945 usec)  google-public-dns-a.google.com (421734 usec)
   12  *                                             google-public-dns-a.google.com (381269 usec)  google-public-dns-a.google.com (423064 usec)
   13  *                                             google-public-dns-a.google.com (381319 usec)  google-public-dns-a.google.com (423643 usec)
   14  *                                             *                                             *
   15  *                                             *                                             *
   16  *                                             *                                             *
   17  *                                             *                                             *
   18  *                                             *                                             *
   19  *                                             *                                             *
   20  *                                             *                                             *
   21  *                                             *                                             *
   22  *                                             *                                             *
   23  *                                             *                                             *
   24  *                                             *                                             *
   25  *                                             *                                             *
   26  *                                             *                                             *
   27  *                                             *                                             *
   28  *                                             *                                             *
   29  *                                             *                                             *
   30  *                                             *                                             *
>>> graph = dublintraceroute.to_graphviz(results)
>>> graph.draw('traceroute.png')
>>> graph.write('traceroute.dot')
```

A naive implementation of the traceroute with a oneliner could be:

```bash
$ sudo python -c "import dublintraceroute; dublintraceroute.DublinTraceroute('8.8.8.8', npaths=3).traceroute().pretty_print()"
  ttl  33436                                          33434                                         33435
-----  ---------------------------------------------  --------------------------------------------  --------------------------------------------
    1  gateway (568664 usec)                          gateway (357115 usec)                         gateway (391580 usec)
    2  *                                              *                                             *
    3  188-141-126-1.dynamic.upc.ie (639608 usec)     188-141-126-1.dynamic.upc.ie (357118 usec)    188-141-126-1.dynamic.upc.ie (458835 usec)
    4  ie-dub01a-ra3-ae34-0.aorta.net (718752 usec)   ie-dub01a-ra3-ae34-0.aorta.net (69098 usec)   ie-dub01a-ra3-ae34-0.aorta.net (416961 usec)
    5  ie-dub01a-ri1-ae50-0.aorta.net (846613 usec)   84-116-130-142.aorta.net (357079 usec)        84-116-130-142.aorta.net (470277 usec)
    6  213.46.165.18 (867147 usec)                    213.46.165.18 (363147 usec)                   213.46.165.18 (470332 usec)
    7  216.239.43.3 (867160 usec)                     216.239.43.3 (363183 usec)                    66.249.95.91 (470292 usec)
    8  *                                              google-public-dns-a.google.com (363209 usec)  google-public-dns-a.google.com (470243 usec)
    9  *                                              google-public-dns-a.google.com (363214 usec)  google-public-dns-a.google.com (470228 usec)
   10  *                                              google-public-dns-a.google.com (363211 usec)  google-public-dns-a.google.com (470172 usec)
   11  *                                              google-public-dns-a.google.com (363204 usec)  google-public-dns-a.google.com (470146 usec)
   12  *                                              google-public-dns-a.google.com (363197 usec)  google-public-dns-a.google.com (470125 usec)
   13  *                                              google-public-dns-a.google.com (363190 usec)  google-public-dns-a.google.com (475719 usec)
   14  *                                              *                                             *
   15  *                                              *                                             *
   16  *                                              *                                             *
   17  *                                              *                                             *
   18  *                                              *                                             *
   19  *                                              *                                             *
   20  *                                              *                                             *
   21  google-public-dns-a.google.com (1359710 usec)  *                                             *
   22  *                                              *                                             *
   23  *                                              *                                             *
   24  *                                              *                                             *
   25  *                                              *                                             *
   26  *                                              *                                             *
   27  *                                              *                                             *
   28  *                                              *                                             *
   29  *                                              *                                             *
   30  *                                              *                                             *
```

You can also invoke the module directly, with `python -m dublintraceroute --help`.

For example:

```bash
$ sudo python -m dublintraceroute trace 8.8.8.8
...
```

then generate a PNG from the traceroute:

```bash
python -m dublintraceroute plot trace.json
```

## Sending probes

You can also send simple probes, without running a full traceroute. This
translates in sending only one packet per path, at a higher TTL. This
functionality is wrapped in the `probe` command:

```
$ sudo python3 -m dublintraceroute probe google.com 
Sending probes to google.com
  Source port: 12345, destination port: 33434, num paths: 20, TTL: 64, delay: 10, broken NAT: False
  #  target           src port    dst port    rtt (usec)
---  -------------  ----------  ----------  ------------
  1  216.58.198.78       12345       33434         15705
  2  216.58.198.78       12345       33435         15902
  3  216.58.198.78       12345       33436         16127
  4  216.58.198.78       12345       33437         16512
  5  216.58.198.78       12345       33438         16465
  6  216.58.198.78       12345       33439         11271
  7  216.58.198.78       12345       33440         21903
  8  216.58.198.78       12345       33441         16811
  9  216.58.198.78       12345       33442         16863
 10  216.58.198.78       12345       33443         16884
 11  216.58.198.78       12345       33444         17225
 12  216.58.198.78       12345       33445         12127
 13  216.58.198.78       12345       33446         12274
 14  216.58.198.78       12345       33447         17783
 15  216.58.198.78       12345       33448         17933
 16  216.58.198.78       12345       33449         12659
 17  216.58.198.78       12345       33450         12749
 18  216.58.198.78       12345       33451         18041
 19  216.58.198.78       12345       33452         12762
 20  216.58.198.78       12345       33453         18404
```


# Data analysis

## Pandas DataFrame

`python-dublin-traceroute` also supports [pandas](http://pandas.pydata.org/) for data analysis. You can easily analyze a traceroute after converting it to a `pandas.DataFrame`:

```python
>>> import dublintraceroute
>>> df = dublintraceroute.DublinTraceroute('8.8.8.8', npaths=3).traceroute().to_dataframe()
>>> import pandas
>>> pandas.set_option('display.width', 180)
>>> df.head()
  is_last                            name  nat_id  received_icmp_code received_icmp_description                           received_icmp_extensions  \
0   False                         gateway       0                  11    TTL expired in transit                                                 []   
1   False                                     NaN                 NaN                       NaN                                                NaN   
2   False    188-141-126-1.dynamic.upc.ie   20530                  11    TTL expired in transit                                                 []   
3   False  ie-dub01a-ra3-ae34-0.aorta.net   20530                  11    TTL expired in transit  [{u'size': 8, u'type': 1, u'class': 1, u'paylo...   
4   False  ie-dub01a-ri1-ae50-0.aorta.net   20530                  11    TTL expired in transit                                                 []   

                           received_icmp_mpls_labels  received_icmp_type received_ip_dst  received_ip_id received_ip_src  received_ip_ttl received_timestamp  rtt_usec  \
0                                                 []                   0    192.168.9.20            1999     192.168.9.1               64  1474070132.380299   1126867   
1                                                NaN                 NaN             NaN             NaN             NaN              NaN                NaN       NaN   
2                                                 []                   0    192.168.9.20               0   188.141.126.1              253  1474070132.380312   1126811   
3  [{u'ttl': 1, u'bottom_of_stack': 1, u'experime...                   0    192.168.9.20            8405   84.116.238.50              252  1474070132.378559   1125021   
4                                                 []                   0    192.168.9.20               0   84.116.130.97              251  1474070132.380326   1126755   

  sent_ip_dst   sent_ip_src  sent_ip_ttl     sent_timestamp  sent_udp_dport  sent_udp_sport  
0     8.8.8.8  192.168.9.20            1  1474070131.253432           33436           12345  
1     8.8.8.8  192.168.9.20            2  1474070131.253468           33436           12345  
2     8.8.8.8  192.168.9.20            3  1474070131.253501           33436           12345  
3     8.8.8.8  192.168.9.20            4  1474070131.253538           33436           12345  
4     8.8.8.8  192.168.9.20            5  1474070131.253571           33436           12345  
```

## RTT chart per path

If you have `matplotlib` installed, you can also create diagrams of various types. For example, let's visualize the RTT hop-by-hop for each network path:

```python
>>> import matplotlib.pyplot as plt
>>> import dublintraceroute
>>> df = dublintraceroute.DublinTraceroute('8.8.8.8').traceroute().to_dataframe()
>>> group = df.groupby('sent_udp_dport')['rtt_usec']
>>> fig, ax = plt.subplots(figsize=(10, 6))
>>> for label, sdf in group:
>>>    sdf.reset_index().plot(ax=ax, label=label, legend=True)
>>> ax.set_title('RTT per destination port')
>>> ax.legend(
>>>    [dport for dport, _ in group],
>>>    title='destination port',
>>>    loc='upper left',
>>> )
>>> plt.show()
>>> fig.savefig('rtt.png')
```

You will see something like this:

![RTT per destination port](images/rtt.png)

This diagram highlights a latency bump around hops with TTL 3 to 5, and devices not responding at TTL 9 to 12.

# Who

Andrea Barberio, find more about me at https://insomniac.slackware.it
