from __future__ import absolute_import
import pytest

import dublintraceroute


def test_init():
    d = dublintraceroute.DublinTraceroute('127.0.0.1')
    assert d.__class__.__name__ == 'DublinTraceroute'
    assert d.sport == 12345

def test_init_attrs():
    d = dublintraceroute.DublinTraceroute('127.0.0.1', sport=12345,
            dport=33434, npaths=10, min_ttl=3, max_ttl=15, delay=100,
            broken_nat=False)
    assert d.target == '127.0.0.1'
    assert d.sport == 12345
    assert d.dport == 33434
    assert d.npaths == 10
    assert d.min_ttl == 3
    assert d.max_ttl == 15
    assert d.delay == 100
    assert d.broken_nat == False


def test_str():
    d = dublintraceroute.DublinTraceroute('127.0.0.1', sport=12345,
            dport=33434, npaths=10, min_ttl=3, max_ttl=15, delay=100,
            broken_nat=False)
    assert str(d) == '''<DublinTraceroute (target='127.0.0.1', sport=12345, dport=33434, npaths=10, min_ttl=3, max_ttl=15, delay=100, broken_nat=False, iterate_sport=False)>'''


class MockDublinTraceroute(object):
    @classmethod  # necessary for Python 2
    def traceroute(self, otherself):
        from conftest import json_results
        return json_results()

def test_traceroute(monkeypatch):
    monkeypatch.setattr(dublintraceroute._dublintraceroute, 'DublinTraceroute', MockDublinTraceroute)
    d = dublintraceroute.DublinTraceroute('127.0.0.1')
    result = d.traceroute()
    assert type(result) == dublintraceroute.TracerouteResults
    assert list(result.keys()) == ['flows']
