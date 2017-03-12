import os
import sys
import json

import pytest


SAMPLE_JSON = os.path.join(os.path.dirname(__file__), 'sample.json')

sys.path.insert(
        0,
        os.path.join(os.path.dirname(__file__), '..')
    )


@pytest.fixture()
def json_results():
    return open(SAMPLE_JSON).read()
