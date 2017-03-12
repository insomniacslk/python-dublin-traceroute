try:
    # Python 3
    import builtins
except ImportError:
    # Python 2
    import __builtin__ as builtins

try:
    # Python 2, json.dump writes a `str`
    from StringIO import StringIO
    StringIO.__enter__ = lambda *args: args[0]
    StringIO.__exit__ = lambda *args: args[0]
except ImportError:
    # Python 3, json.dump writes a `unicode`
    from io import StringIO

import pytest

import dublintraceroute


def test_tracerouteresults_init():
    r = dublintraceroute.TracerouteResults({})
    assert list(r.keys()) == []


def test_tracerouteresults_save(monkeypatch):
    def _open(fname, mode):
        return StringIO()
    monkeypatch.setattr(builtins, 'open', _open)
    r = dublintraceroute.TracerouteResults({})
    r.save('dummy file name')
