TESTDIR=$(shell pwd)/tests
.PHONY: clean test

test: test-py3 test-py2

build-ext-py3:
	 python3 setup.py build_ext --inplace

build-ext-py2:
	 python setup.py build_ext --inplace

test-py3: build-ext-py3
	py.test-3 -v --cov=dublintraceroute --cov-report term-missing "$(TESTDIR)"

test-py2: build-ext-py2
	py.test -v --cov=dublintraceroute --cov-report term-missing "$(TESTDIR)"

clean:
	$(RM) -r build dublintraceroute/*.so .cache .coverage \
		./tests/__pycache__ ./dublintraceroute/__pycache__
