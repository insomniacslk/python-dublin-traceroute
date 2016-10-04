from __future__ import absolute_import
from __future__ import print_function

import json
import random

from . import _dublintraceroute
from .tracerouteresults import TracerouteResults


class DublinTraceroute(_dublintraceroute.DublinTraceroute):
    def __str__(self):
        return ('<{self.__class__.__name__} (target={target!r}, '
                'sport={sport!r}, dport={dport!r}, '
                'npaths={npaths!r}, min_ttl={min_ttl}, '
                'max_ttl={max_ttl!r}, delay={delay!r}, '
                'broken_nat={broken_nat!r})>'.format(
                    self=self,
                    target=self.target,
                    sport=self.sport,
                    dport=self.dport,
                    npaths=self.npaths,
                    min_ttl=self.min_ttl,
                    max_ttl=self.max_ttl,
                    delay=self.delay,
                    broken_nat=self.broken_nat,
                    )
                )

    def traceroute(self):
        '''Run the traceroute

        Example:
        >>> dub = DublinTraceroute("8.8.8.8", 12345, 33434)
        >>> results = dub.traceroute()
        >>> print(results)
        {u'flows': {u'33434': [{u'is_last': False,
            u'nat_id': 0,
            u'received': {u'icmp': {u'code': 11,
            u'description': u'TTL expired in transit',
            u'type': 0},
            u'ip': {u'dst': u'192.168.0.1', u'src': u'192.168.0.254', u'ttl': 64}},
            u'rtt_usec': 2801,
            u'sent': {u'ip': {u'dst': u'8.8.8.8', u'src': u'192.168.0.1', u'ttl': 1},
            u'udp': {u'dport': 33434, u'sport': 12345}}},
        ...
        >>>'''
        results = _dublintraceroute.DublinTraceroute.traceroute(self)
        json_results = json.loads(results)
        return TracerouteResults(json_results, min_ttl=self.min_ttl)


def to_graphviz(traceroute, no_rtt=False):
    '''
    Convert a traceroute to a graphviz object.

    This method creates a GraphViz object from the output of
    DublinTraceroute.traceroute(), suitable for plotting.

    Example:
    >>> dub = DublinTraceroute(12345, 33434, "8.8.8.8")
    >>> results = dub.traceroute()
    >>> graph = to_graphviz(results)
    >>> graph.draw('traceroute.png')
    >>> graph.write('traceroute.dot')
    '''
    # importing here, so if pygraphviz is not installed it will not fail at
    # import time
    import pygraphviz

    graph = pygraphviz.AGraph(strict=False, directed=True)
    graph.node_attr['shape'] = 'ellipse'
    graph.graph_attr['rankdir'] = 'BT'

    # create a dummy first node to add the source host to the graph
    # FIXME this approach sucks
    for flow, hops in traceroute['flows'].items():
        src_ip = hops[0]['sent']['ip']['src']
        firsthop = {}
        hops = [firsthop] + hops
        color = random.randrange(0, 0xffffff)

        previous_nat_id = 0
        for index, hop in enumerate(hops):

            # add node
            if index == 0:
                # first hop, the source host
                nodename = src_ip
                graph.add_node(nodename, shape='rectangle')
            else:
                # all the other hops
                received = hop['received']
                nodeattrs = {}
                if received is None:
                    nodename = 'NULL{idx}'.format(idx=index)
                    nodeattrs['label'] = '*'
                else:
                    nodename = received['ip']['src']
                    if hop['name'] != nodename:
                        hostname = '\n{h}'.format(h=hop['name'])
                    else:
                        hostname = ''

                    # MPLS labels
                    try:
                        labels = received['icmp']['mpls_labels']
                    except KeyError:
                        labels = []
                    if labels:
                        mpls = 'MPLS labels: \n'
                        for label in labels:
                            mpls += '- {l}, ttl: {t}\n'.format(
                                l=label['label'],
                                t=label['ttl'],
                            )
                    else:
                        mpls = ''
                    nodeattrs['label'] = '{ip}{name}\n{icmp}\n{mpls}'.format(
                        ip=nodename,
                        name=hostname,
                        icmp=received['icmp']['description'],
                        mpls=mpls,
                    )
                if index == 0 or hop['is_last']:
                    nodeattrs['shape'] = 'rectangle'
                graph.add_node(nodename)
                graph.get_node(nodename).attr.update(nodeattrs)

            # add edge
            try:
                nexthop = hops[index + 1]
            except IndexError:
                # This means that we are at the last hop, no further edge
                continue

            next_received = nexthop['received']
            edgeattrs = {'color': '#{c:x}'.format(c=color), 'label': ''}
            if next_received is None:
                next_nodename = 'NULL{idx}'.format(idx=index + 1)
            else:
                next_nodename = next_received['ip']['src']
            if index == 0:
                edgeattrs['label'] = 'dport\n{dp}'.format(dp=flow)
            rtt = nexthop['rtt_usec']
            try:
                if previous_nat_id != nexthop['nat_id']:
                    edgeattrs['label'] += '\nNAT detected'
                previous_nat_id = hop['nat_id']
            except KeyError:
                pass
            if not no_rtt:
                if rtt is not None:
                    edgeattrs['label'] += '\n{sec}.{usec} ms'.format(
                        sec=rtt // 1000, usec=rtt % 1000)
            graph.add_edge(nodename, next_nodename)
            graph.get_edge(nodename, next_nodename).attr.update(edgeattrs)

    graph.layout()
    return graph
